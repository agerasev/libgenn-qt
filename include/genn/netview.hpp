#pragma once

#include <random>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QWheelEvent>
#include <QTimer>
#include <QMutex>

#include <genn/genetics.hpp>

#include <la/vec.hpp>

#include "common.hpp"
#include "nodeview.hpp"
#include "linkview.hpp"
#include "asyncanim.hpp"

class NetView : public QGraphicsView, public AsyncAnim {
	Q_OBJECT
public:
	QGraphicsScene *scene;
	std::map<NodeID, NodeView *> nodes;
	std::map<LinkID, LinkView *> links;
	
	std::minstd_rand re;
	std::uniform_real_distribution<> unif;
	
	QMutex mtx;
	NetView();

	virtual ~NetView();
	
private:
	void init(NodeID id, NodeView *nv);
	void init(LinkID id, LinkView *lv);
	void quit(NodeID id, NodeView *nv);
	void quit(LinkID id, LinkView *lv);
	
	// TODO manage states
	
	template <typename K, typename VS, typename VD>
	void sync_map(const GeneMap<K, VS> &src, std::map<K, VD*> &dst) {
		for(auto &p : dst) {
			p.second->fdel = true;
		}
		
		src.iter([&] (K id, const VS &e) {
			VD *it = nullptr;
			auto ii = dst.find(id);
			if(ii == dst.end()) {
				it = new VD();
				it->fadd = true;
				init(id, it);
				ii = dst.insert(std::make_pair(id, it)).first;
			} else {
				it = ii->second;
			}
			it->fdel = false;
			it->sync(e);
		});
		
		for(auto ii = dst.begin(); ii != dst.end();) {
			VD *iv = ii->second;
			if (iv->fadd && iv->fdel) {
				quit(ii->first, iv);
				dst.erase(ii++);
				delete iv;
			} else {
				++ii;
			}
		}
	}
	
	template <typename K, typename V>
	void sync_scene(std::map<K, V*> &map) {
		for(auto ii = map.begin(); ii != map.end();) {
			V *iv = ii->second;
			if (iv->fdel) {
				if (!iv->fadd) {
					scene->removeItem(iv);
				}
				quit(ii->first, iv);
				map.erase(ii++);
				delete iv;
			} else {
				if (iv->fadd) {
					scene->addItem(iv);
					iv->fadd = false;
				}
				++ii;
			}
		}
	}
	
public:
	virtual void anim() override {
		mtx.lock();
		{
			sync_scene(nodes);
			sync_scene(links);
			move(1e-3*delay_ms);
			update();
		}
		mtx.unlock();
	}
	
	void sync(const NetworkGene &net) {
		mtx.lock();
		{
			sync_map(net.nodes, nodes);
			sync_map(net.links, links);
		}
		mtx.unlock();
	}
	
	void move(double dt);
	void initView();
	void fit();
	virtual void resizeEvent(QResizeEvent *event) override;
};
