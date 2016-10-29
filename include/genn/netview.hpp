#pragma once

#include <random>
#include <cmath>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QWheelEvent>
#include <QTimer>

#include <genn/network.hpp>

#include <la/vec.hpp>

#include "nodeview.hpp"
#include "linkview.hpp"

class NetView : public QGraphicsView {
	Q_OBJECT
public:
	QGraphicsScene *scene;
	std::map<NodeID, NodeView *> nodes;
	std::map<LinkID, LinkView *> links;
	
	std::minstd_rand re;
	std::uniform_real_distribution<> unif;
	
	bool done = true;
	
	NetView() : QGraphicsView() {
		scene = new QGraphicsScene();
		setScene(scene);
		
		initView();
	}
	
	virtual ~NetView() {
		for(auto &lv : links) {
			delete lv.second;
		}
		for(auto &nv : nodes) {
			delete nv.second;
		}
	}
	
private:
	void init(NodeID id, NodeView *nv) {
		double rf = 3*NodeView::RAD*nodes.size();
		nv->pos = rf*vec2(unif(re), unif(re));
		nv->setZValue(1.0);
	}
	
	void init(LinkID id, LinkView *lv) {
		lv->src = nodes[id.src];
		lv->dst = nodes[id.dst];
		lv->setZValue(0.0);
	}
	
	template <typename K, typename VS, typename VD>
	void sync_map(const std::map<K, VS> &src, std::map<K, VD*> &dst) {
		for(auto &p : dst) {
			p.second->exist = false;
		}
		
		for(const auto &ep : src) {
			const K &id = ep.first;
			const VS &e = ep.second;
			VD *it = nullptr;
			auto ii = dst.find(id);
			if(ii == dst.end()) {
				it = new VD();
				init(id, it);
				ii = dst.insert(std::make_pair(id, it)).first;
				scene->addItem(it);
			} else {
				it = ii->second;
			}
			it->sync(e);
			it->exist = true;
		}
		
		for(auto ii = dst.begin(); ii != dst.end();) {
			if(ii->second->exist) {
				++ii;
			} else {
				auto iv = ii->second;
				scene->removeItem(iv);
				dst.erase(ii++);
				delete iv;
			}
		}
	}
	
	void timer_func() {
		int ms = 40;
		move(1e-3*ms);
		update();
		if(!done) {
			QTimer::singleShot(ms, [this](){timer_func();});
		}
	}
	
public:
	void startAnim() {
		done = false;
		timer_func();
	}
	
	void stopAnim() {
		done = true;
	}
	
	void sync(const NetworkGene &net) {
		sync_map(net.nodes, nodes);
		sync_map(net.links, links);
	}
	
	void move(double dt) {
		for(auto &nv0 : nodes) {
			for(auto &nv1 : nodes) {
				if(nv0.first < nv1.first) {
					LinkView::repulse(nv0.second, nv1.second);
				}
			}
		}
		for(auto &lv : links) {
			lv.second->attract();
		}
		for(auto &nv : nodes) {
			nv.second->move(dt);
		}
		for(auto &lv : links) {
			lv.second->move(dt);
		}
		
		vec2 rb = vec2(0,0), lt = vec2(0,0);
		bool first = true;
		for(auto &nv : nodes) {
			vec2 p = nv.second->pos;
			double r = nv.second->rad;
			bvec2 of;
			
			vec2 nrb = p + vec2(r,r);
			of = (nrb > rb) || bvec2(first, first);
			rb = of*nrb + (!of)*rb;
			
			vec2 nlt = p - vec2(r,r);
			of = (nlt < lt) || bvec2(first, first);
			lt = of*nlt + (!of)*lt;
			
			first = false;
		}
		
		vec2 xy = 0.5*(rb + lt);
		vec2 wh = 0.7*(rb - lt);
		scene->setSceneRect(xy.x() - wh.x(), xy.y() - wh.y(), 2*wh.x(), 2*wh.y());
		
		fit();
	}
	
	void initView() {
		setStyleSheet( "border-style: none;");

		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		setDragMode(ScrollHandDrag);
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		// setAlignment(Qt::AlignCenter);
		
		setRenderHint(QPainter::Antialiasing);
		
		setBackgroundBrush(QBrush(QColor("#CDC0B4")));
	}
	
	virtual void enterEvent(QEvent *event) override {
		QGraphicsView::enterEvent(event);
		viewport()->setCursor(Qt::ArrowCursor);
	}
	
	virtual void mousePressEvent(QMouseEvent *event) override {
		QGraphicsView::mousePressEvent(event);
		viewport()->setCursor(Qt::ArrowCursor);
	}
	
	virtual void mouseReleaseEvent(QMouseEvent *event) override {
		QGraphicsView::mouseReleaseEvent(event);
		viewport()->setCursor(Qt::ArrowCursor);
	}
	
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override {
		QGraphicsView::mouseDoubleClickEvent(event);
		centerOn(0, 0);
	}
	
	void fit() {
		auto t = transform();
		auto r = sceneRect();
		vec2 sv = vec2(t.m11()*r.width(), t.m22()*r.height());
		auto s = size();
		vec2 vv = vec2(s.width(), s.height());
		double z = min(vv/sv);
		if(z > 1.0) {
			scale(z, z);
		}
	}
	
	virtual void wheelEvent(QWheelEvent *event) override {
		// QGraphicsView::wheelEvent(event);
		double z = pow(1.25, event->delta()/120.0);
		scale(z,z);
		fit();
	}
	
	virtual void resizeEvent(QResizeEvent *event) override {
		QGraphicsView::resizeEvent(event);
		fit();
	}
};
