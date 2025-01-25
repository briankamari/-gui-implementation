#pragma once

#include "ui.h"

struct ui_text : public ui_control {

	ui_text();

	virtual bool init();
	virtual void clear();
	virtual bool update();

	virtual void msgproc(UINT msg, WPARAM wParam, LPARAM lParam);

	void settext(const char* text);

	void start_highlight();
	void end_highlight();
	void position_proc(uint32_t * start,uint32_t * end);

	void stringgeneration();

	void currentposition(uint32_t *x,uint32_t * y);
	void currentposition(uint32_t *x,uint32_t * y,uint32_t position);

	uint32_t getlineposition(bool isup,uint32_t x,uint32_t y);

	bool gencoursorbuffer();
	bool genhighlightbuffer();
	bool genbackgroundbuffer();
	bool genforegroundbuffer();

	uint32_t m_vscroll;
	uint32_t m_hscroll;
	uint32_t m_highlight;
	uint32_t m_text_position;
	uint32_t m_charcount;
	uint32_t m_highlight_vertex_count;
	
	int32_t  m_line_position;

	_string        m_string;
    _string_array  m_strings;

	IDirect3DVertexBuffer9* m_coursor_buffer;
	IDirect3DVertexBuffer9* m_highlight_buffer;

};
