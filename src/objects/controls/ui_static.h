#pragma once

#include "ui.h"

struct ui_static : public ui_control {
	ui_static();
	~ui_static();

	virtual bool init();
	virtual void clear();
	virtual bool update();

	virtual void msgproc(UINT msg, WPARAM wParam, LPARAM lParam);

	void settext(const char* text);


	bool genbackgroundbuffer();
	bool genforegroundbuffer();
	_string m_string;

};

