#pragma once

#include <QTimer>

class AsyncAnim {
public:
	bool anim_done = true;
	int delay_ms = 40;
	
	virtual void anim() = 0;
	
	void timer_func() {
		anim();
		if(!anim_done) {
			QTimer::singleShot(delay_ms, [this](){timer_func();});
		}
	}
	
	void anim_start() {
		anim_done = false;
		timer_func();
	}
	
	void anim_stop() {
		anim_done = true;
	}
};
