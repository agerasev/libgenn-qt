#pragma once

#include <QGraphicsItem>

#include <genn/network.hpp>

#include <la/vec.hpp>

class NodeView : public QGraphicsItem {
public:
	constexpr static const double RAD = 1;
	
	bool exist = true;
	
	float bias = 0.0f;
	vec2 pos = vec2(0,0);
	vec2 vel = vec2(0,0);
	double rad = RAD;
	
	void sync(const NodeGene &node) {
		bias = node.bias;
	}
	
	void move(double dt) {
		pos += vel*dt;
		vel = vec2(0,0);
	}
	
	QRectF boundingRect() const override {
		return QRectF(pos.x() - rad, pos.y() - rad, 2*rad, 2*rad);
	}
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
		painter->setBrush(QBrush(QColor(255*(bias > 0)*(1 - exp(-bias)),0,255*(bias < 0)*(1 - exp(bias)))));
		painter->setPen(Qt::NoPen);
		painter->drawEllipse(boundingRect());
	}
};
