#pragma once

#include <QGraphicsItem>

#include <genn/genetics.hpp>

#include <la/vec.hpp>

#include "nodeview.hpp"

class LinkView : public QGraphicsLineItem {
public:
	bool exist = true;
	
	float weight = 0.0f;
	NodeView *src = nullptr;
	NodeView *dst = nullptr;
	
	static constexpr double fattr = 1e0;
	static constexpr double frep = 1e2;
	static constexpr double eqrad = sqrt(frep/fattr);
	
	void sync(const Link &link) {
		weight = link.weight;
		move(0.0);
		QPen pen;
		pen.setColor(QColor(255*(weight > 0)*(1 - exp(-weight)),0,255*(weight < 0)*(1 - exp(weight))));
		pen.setWidthF(0.25);
		setPen(pen);
	}
	
	static void repulse(NodeView *n0, NodeView *n1) {
		/*
		vec2 r = n1->pos - n0->pos;
		double l = length(r);
		double f = l - 4.0*(n0->rad + n1->rad);
		if(f < 0) {
			vec2 d = normalize(r);
			n0->vel += 2e0*d*f;
			n1->vel -= 2e0*d*f;
		}
		*/
		vec2 s = n0->pos, d = n1->pos;
		vec2 r = d - s;
		r = r/dot(r,r);
		n0->vel -= frep*r;
		n1->vel += frep*r;
	}
	
	void attract() {
		vec2 s = src->pos, d = dst->pos;
		vec2 r = d - s;
		src->vel += fattr*r;
		dst->vel -= fattr*r;
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
