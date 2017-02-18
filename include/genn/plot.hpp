#pragma once

#include <vector>
#include <mutex>

#include <QWidget>
#include <QPainter>
#include <QTimer>

#include "common.hpp"
#include "asyncanim.hpp"

class Plot : public QWidget, public AsyncAnim {
	Q_OBJECT
public:
	static const unsigned char
		LOG_SCALE_X = 0x01,
		LOG_SCALE_Y = 0x02;
	
	std::mutex mtx;
	std::vector<std::pair<double, double>> points;
	double xmin, xmax, ymin, ymax;
	bool logx = false, logy = false;
	double border = 10; // px
	
	Plot(unsigned char flags = 0) : QWidget() {
		if (flags & LOG_SCALE_X) {
			logx = true;
		}
		if (flags & LOG_SCALE_Y) {
			logy = true;
		}
		// setStyleSheet("background-color:#CDC0B4;");
	}
	
	void add(double x, double y) {
		mtx.lock();
		{
			if (logx) { x = log(x); }
			if (logy) { y = log(y); }
			if (points.size() < 1) {
				xmin = xmax = x;
				ymin = ymax = y;
			} else {
				if (x > xmax) { xmax = x; }
				if (x < xmin) { xmin = x; }
				if (y > ymax) { ymax = y; }
				if (y < ymin) { ymin = y; }
			}
			points.push_back(std::make_pair(x, y));
		}
		mtx.unlock();
	}
	
	void clear() {
		mtx.lock();
		points.clear();
		mtx.unlock();
	}
	
	void paintEvent(QPaintEvent *event) override {
		QWidget::paintEvent(event);
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		
		QPen pen;
		
		// draw plot area
		pen.setColor(QColor(0xff, 0xff, 0xff));
		pen.setWidthF(0.0);
		painter.setPen(pen);
		painter.setBrush(QBrush(QColor(0xff,0xff,0xff)));
		double br = border - 2.0;
		painter.drawRect(QRectF(br, br, rect().width() - 2*br, rect().height() - 2*br));
		
		// draw ticks
		pen.setColor(QColor(0,0,0));
		pen.setWidthF(2.0);
		painter.setPen(pen);
		
		// draw plot
		
		pen.setColor(QColor(0,0,0));
		pen.setWidthF(2.0);
		painter.setPen(pen);
		
		mtx.lock();
		if (points.size() > 0) {
			double w = rect().width() - 2*border, h = rect().height() - 2*border;
			double px = 0.0, py = 0.0;
			for (int i = 0; i < int(points.size()); ++i) {
				double x = points[i].first, y = points[i].second;
				x = w*(x - xmin)/(xmax - xmin) + border;
				y = h*(ymax - y)/(ymax - ymin) + border;
				// painter.drawEllipse(p, rs, rs);
				if (i > 0) {
					painter.drawLine(QPointF(px, py), QPointF(x, y));
				}
				px = x;
				py = y;
			}
		}
		mtx.unlock();
	}
	
	virtual void anim() override {
		update();
	}
};
