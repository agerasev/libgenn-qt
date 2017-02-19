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
	std::vector<QPointF> buffer;
	double xmin, xmax, ymin, ymax;
	bool logx = false, logy = false;
	double border = 24; // px
	
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
			bool drop = false;
			if (logx) { 
				if (x <= 0.0) { drop = true; }
				x = log10(x); 
			}
			if (logy) {
				if (y <= 0.0) { drop = true; }
				y = log10(y); 
			}
			if (!drop) {
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
				buffer.push_back(QPointF());
			}
		}
		mtx.unlock();
	}
	
	void clear() {
		mtx.lock();
		points.clear();
		buffer.clear();
		mtx.unlock();
	}
	
	double area_width() const {
		return rect().width() - 2*border;
	}
	
	double area_height() const {
		return rect().height() - 2*border;
	}
	
	double map_x(double x) const {
		return area_width()*(x - xmin)/(xmax - xmin) + border;
	}
	
	double map_y(double y) const {
		return area_height()*(ymax - y)/(ymax - ymin) + border;
	}
	
	void paintEvent(QPaintEvent *event) override {
		QWidget::paintEvent(event);
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing, false);
		
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
		pen.setWidthF(1.0);
		painter.setPen(pen);
		
		double lx = log10(xmax - xmin);
		int px = floor(lx);
		double bx = pow(10, px);
		int dxmin = ceil(xmin/bx);
		int dxmax = floor(xmax/bx);
		for (int i = dxmin - 1; i <= dxmax + 1; ++i) {
			for (int j = 1; j < 10; ++j) {
				double vx;
				if (logx) {
					if (j == 1) { continue; }
					vx = map_x((i + log10(j))*bx);
				} else {
					vx = map_x((i + 0.1*j)*bx);
				}
				if(vx >= border && vx <= rect().width() - border) {
					painter.drawLine(QPointF(vx, rect().height() - border), QPointF(vx, rect().height() - 0.6*border));
				}
			}
			if(i >= dxmin && i <= dxmax) {
				std::string text;
				if (!logx) {
					text = std::to_string(i) + 'e' + std::to_string(px);
				} else {
					text = std::to_string(int(i*bx));
				}
				painter.drawText(QPointF(map_x(i*bx) + 2, rect().height()), QString(text.c_str()));
				painter.drawLine(QPointF(map_x(i*bx), rect().height() - border), QPointF(map_x(i*bx), rect().height()));
			}
		}
		
		double ly = log10(ymax - ymin);
		int py = floor(ly);
		double by = pow(10, py);
		int dymin = ceil(ymin/by);
		int dymax = floor(ymax/by);
		for (int i = dymin - 1; i <= dymax + 1; ++i) {
			for (int j = 1; j < 10; ++j) {
				double vy;
				if (logy) {
					if (j == 1) { continue; }
					vy = map_y((i + log10(j))*by);
				} else {
					vy = map_y((i + 0.1*j)*by);
				}
				if(vy >= border && vy <= rect().height() - border) {
					painter.drawLine(QPointF(0.6*border, vy), QPointF(border, vy));
				}
			}
			if(i >= dymin && i <= dymax) {
				std::string text;
				if (!logy) {
					text = std::to_string(i) + 'e' + std::to_string(py);
				} else {
					text = std::to_string(int(i*by));
				}
				painter.drawText(QPointF(0, map_y(i*by) - 2), QString(text.c_str()));
				painter.drawLine(QPointF(0, map_y(i*by)), QPointF(border, map_y(i*by)));
			}
		}
		
		// draw plot
		painter.setRenderHint(QPainter::Antialiasing, true);
		
		pen.setColor(QColor(0xff,0x00,0x00));
		pen.setWidthF(2.0);
		painter.setPen(pen);
		
		mtx.lock();
		if (points.size() > 0) {
			for (int i = 0; i < int(points.size()); ++i) {
				double x = points[i].first, y = points[i].second;
				x = map_x(x);
				y = map_y(y);
				buffer[i] = QPointF(x, y);
				px = x;
				py = y;
			}
		}
		mtx.unlock();
		
		painter.drawPolyline(buffer.data(), buffer.size());
	}
	
	virtual void anim() override {
		update();
	}
};
