#pragma once

#include "appdefs.h"

#define ui_highlight 0x01
#define ui_redraw 0x02
#define ui_mouse_over 0x04
#define ui_left_button_down 0x08

#define font_texture_size  256

struct ui;

struct ui_control : public _object {

	ui_control();


	float m_x , m_y;
	float m_width , m_height;
	float m_font_width;
	float m_font_height;

	uint32_t m_id;
	
	_vec4 m_foreground_color;
	_vec4 m_background_color;

	bool intersection_test();
	IDirect3DTexture9*     m_font_texture;
	IDirect3DVertexBuffer9* m_foreground_vertex_buffer;
	IDirect3DVertexBuffer9* m_background_vertex_buffer;

	static bool chartest(WPARAM wParam);

};

struct ui : public _object {

	ui();

	virtual bool init();
	virtual void clear();
	virtual bool update();

	virtual void onlostdevice();
	virtual void onresetdevice();

	bool addcontrol(ui_control * control);

	bool readbitmap(const char * filename);

	_string getclipboard();
	bool setclipboard(const _string & text);

	int32_t m_current_control;

	uint32_t   m_image_size;
	uint32_t   m_image_width;
	uint32_t   m_image_height;
	uint8_t  * m_image_data;

	IDirect3DTexture9*     m_main_font_texture;

	_array<ui_control*> m_controls;
	_mat4 m_mat;
	_mat4 m_world;
	_mat4 m_view;
	_mat4 m_projection;


	static _vec2 * s_font_vectors;

};
