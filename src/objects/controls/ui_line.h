#pragma once

#include "ui.h"

struct ui_line : public ui_control {

	ui_line();

	virtual bool init();
	virtual void clear();
	virtual bool update();

	virtual void msgproc(UINT msg, WPARAM wParam, LPARAM lParam);

	void settext(const char* text);


	bool gencoursorbuffer();
	bool genhighlightbuffer();
	bool genbackgroundbuffer();
	bool genforegroundbuffer();

	uint32_t m_scroll;
	uint32_t m_highlight;
	uint32_t m_text_position;
	uint32_t m_count;

	_string m_string;

	IDirect3DVertexBuffer9* m_coursor_buffer;
	IDirect3DVertexBuffer9* m_highlight_buffer;

};

