#pragma once

#include <QGraphicsItem>
#include <QPainter>

#include <genn/genetics.hpp>

#include <la/vec.hpp>

#include "nodeview.hpp"

#include "util.hpp"

class LinkView : public QGraphicsItem {
public:
	bool exist = true;
	
	float weight = 0.0f;
	NodeView *src = nullptr;
	NodeView *dst = nullptr;
	
	bool bidir = false;
	vec2 spos, dpos;
	QRectF brect;
	
	static constexpr double FORCE = 1e0;
	static constexpr double RAD = 4*NodeView::RAD;
	
	void sync(const Link &link) {
		weight = link.weight;
		move(0.0);
	}
	
	static void repulse(NodeView *n0, NodeView *n1) {
		vec2 s = n0->pos, d = n1->pos;
		vec2 r = d - s;
		r = (2.72*2.72)*r/exp(dot(r,r)/(2*RAD*RAD));
		n0->vel -= FORCE*r;
		n1->vel += FORCE*r;
	}
	
	void attract() {
		vec2 s = src->pos, d = dst->pos;
		vec2 r = d - s;
		src->vel += FORCE*r;
		dst->vel -= FORCE*r;
	}
	
	void move(double dt) {
		spos = src->pos;
		dpos = dst->pos;
		
		brect.setX(std::min(spos.x(), dpos.x()));
		brect.setWidth(std::max(spos.x(), dpos.x()) - brect.x());
		brect.setY(std::min(spos.y(), dpos.y()));
		brect.setHeight(std::max(spos.y(), dpos.y()) - brect.y());
	}
	
	QRectF boundingRect() const override {
		return brect;
	}
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
		QPen pen;
		pen.setColor(QColor(255*(weight > 0)*(1 - exp(-weight)),0,255*(weight < 0)*(1 - exp(weight))));
		pen.setWidthF(0.25);
		painter->setPen(pen);
		if(src == dst) {
			double lbrad = 1.2*NodeView::RAD;
			painter->drawEllipse(QPointF(spos.x(), spos.y() - 0.8*lbrad), lbrad, lbrad);
		} else {
			if(length(dst->pos - src->pos) > src->rad + dst->rad) {
				vec2 dir = normalize(dpos - spos);
				vec2 ort = vec2(-dir.y(), dir.x());
				vec2 mid = 0.5*(spos + dpos);
				if(bidir) {
					QPainterPath path(qpoint(spos));
					mid += NodeView::RAD*ort;
					path.quadTo(qpoint(mid), qpoint(dpos));
					painter->drawPath(path);
					mid -= 0.5*NodeView::RAD*ort;
				} else {
					painter->drawLine(qpoint(spos), qpoint(dpos));
				}
				painter->drawLine(qpoint(mid + 0.5*NodeView::RAD*(-dir + ort)), qpoint(mid));
				painter->drawLine(qpoint(mid + 0.5*NodeView::RAD*(-dir - ort)), qpoint(mid));
			}
		}
	}
};
