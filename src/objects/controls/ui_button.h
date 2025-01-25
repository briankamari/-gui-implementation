#pragma once
#include "appdefs.h"

#include "ui.h"

struct ui_button : public ui_control {
	ui_button(void);
	~ui_button(void);

	virtual bool init();
	virtual void clear();
	virtual bool update();

	virtual void msgproc(UINT msg, WPARAM wParam, LPARAM lParam);

	void settext(const char* text);

	_vec4 m_alt_color;

	bool genbackgroundbuffer();
	bool genforegroundbuffer();
	_string m_string;

};