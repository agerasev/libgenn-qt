#include <genn/netview.hpp>

#include <cmath>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QWheelEvent>
#include <QTimer>

#include <la/vec.hpp>


NetView::NetView(const Network *n) : QGraphicsView(), net(n) {
	scene = new QGraphicsScene();
	setScene(scene);
	
	initView();
}

NetView::~NetView() {
	for(auto &lv : links) {
		delete lv.second;
	}
	for(auto &nv : nodes) {
		delete nv.second;
	}
}

void NetView::init(NodeID id, NodeView *nv) {
	double rf = LinkView::RAD*ceil(sqrt(nodes.size()));
	nv->pos = rf*vec2(unif(re), unif(re));
	nv->setZValue(1.0);
}

void NetView::init(LinkID id, LinkView *lv) {
	lv->src = nodes[id.src];
	lv->dst = nodes[id.dst];
	lv->setZValue(0.0);
	
	auto it = links.find(LinkID(id.dst, id.src));
	if(it != links.end()) {
		lv->bidir = true;
		LinkView *olv = it->second;
		olv->bidir = true;
	}
}

void NetView::quit(NodeID id, NodeView *nv) {
	
}

void NetView::quit(LinkID id, LinkView *lv) {
	auto it = links.find(LinkID(id.dst, id.src));
	if(it != links.end()) {
		LinkView *olv = it->second;
		olv->bidir = false;
	}
}

void NetView::move(double dt) {
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
	vec2 wh = 0.6*(rb - lt);
	scene->setSceneRect(xy.x() - wh.x(), xy.y() - wh.y(), 2*wh.x(), 2*wh.y());
	
	fit();
}

void NetView::initView() {
	setStyleSheet( "border-style: none;");

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	setRenderHint(QPainter::Antialiasing);
	
	setBackgroundBrush(QBrush(QColor("#CDC0B4")));
}

void NetView::fit() {
	fitInView(sceneRect(), Qt::KeepAspectRatio);
}

void NetView::resizeEvent(QResizeEvent *event) {
	QGraphicsView::resizeEvent(event);
	fit();
}

#include "../include/genn/moc_netview.cpp"
