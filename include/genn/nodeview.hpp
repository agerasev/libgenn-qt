#pragma once

#include <QGraphicsItem>
#include <QPainter>

#include <genn/genetics.hpp>

#include <la/vec.hpp>

#include "util.hpp"

class NodeView : public QGraphicsItem {
public:
	constexpr static const double RAD = 1;
	
	NodeID id;
	// QStaticText text;
	
	bool exist = true;
	
	float bias = 0.0f;
	vec2 pos = vec2(0,0);
	vec2 vel = vec2(0,0);
	double rad = RAD;
	
	void sync(const Node &node) {
		bias = node.bias;
	}
	
	void move(double dt) {
		pos += vel*dt;
		vel = vec2(0,0);
	}
	
	void set_id(NodeID id_) {
		id = id_;
	}
	
	QRectF boundingRect() const override {
		return QRectF(pos.x() - rad, pos.y() - rad, 2*rad, 2*rad);
	}
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
		painter->setBrush(QBrush(QColor(255*(bias > 0)*(1 - exp(-bias)),0,255*(bias < 0)*(1 - exp(bias)))));
		painter->setPen(Qt::NoPen);
		painter->drawEllipse(boundingRect());
		
		painter->setPen(QPen(QColor(255,255,255)));
		QFont f = painter->font();
		f.setPointSizeF(RAD);
		painter->setFont(f);
		vec2 tp = vec2(boundingRect().x(), boundingRect().y()) + RAD*vec2(0.7, 1.3);
		painter->drawText(qpoint(tp), std::to_string(id).c_str());
	}
};
