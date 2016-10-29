#pragma once

#include <QGraphicsItem>

#include <genn/network.hpp>

#include <la/vec.hpp>

#include "nodeview.hpp"

class LinkView : public QGraphicsLineItem {
public:
	bool exist = true;
	
	float weight = 0.0f;
	NodeView *src = nullptr;
	NodeView *dst = nullptr;
	
	void sync(const LinkGene &link) {
		weight = link.weight;
		move(0.0);
	}
	
	static void repulse(NodeView *n0, NodeView *n1) {
		vec2 r = n1->pos - n0->pos;
		double l = length(r);
		double f = l - 2.0*(n0->rad + n1->rad);
		if(f < 0) {
			vec2 d = normalize(r);
			n0->vel += 2e0*d*f;
			n1->vel -= 2e0*d*f;
		}
		/*
		double a = 10.0*(n0->rad + n1->rad);
		n0->vel -= a*r/(l*l);
		n1->vel += a*r/(l*l);
		*/
	}
	
	void attract() {
		vec2 s = src->pos, d = dst->pos;
		vec2 r = d - s;
		src->vel += 1e-1*r;
		dst->vel -= 1e-1*r;
	}
	
	void move(double dt) {
		vec2 s = src->pos, d = dst->pos;
		vec2 r = d - s;
		vec2 nr = normalize(r);
		s += nr*src->rad;
		d -= nr*dst->rad;
		setLine(s.x(), s.y(), d.x(), d.y());
	}
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
		if(length(dst->pos - src->pos) > src->rad + dst->rad) {
			QGraphicsLineItem::paint(painter, option, widget);
		}
	}
};
