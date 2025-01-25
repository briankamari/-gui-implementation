#pragma once

#include "appdefs.h"

struct image;

struct ui;
struct grayman;
struct ui_static;

struct _app : public _flags {

	_app();

	void run();
	bool init();
	void clear();
	bool update();

	void onlostdevice();
	void onresetdevice();

	ui * m_ui;

	grayman * m_grayman;
	ui_static * m_animation_control;

	_vec3 m_up;
	_vec3 m_look;

	_vec3 m_target;
	_vec3 m_position;

	_mat4 m_view;
	_mat4 m_projection;

	float m_distance;
	float m_x_cursor_pos;
	float m_y_cursor_pos;

	uint32_t m_animation_button_id;

	int   m_keyframe;
	int   m_framespersecond;
	float m_last_frame_seconds;
	float m_last_frame_milliseconds;

	float m_y_pos;
	float m_x_pos;

	float m_yaw;
	float m_pitch;
	float m_yaw_pos;
	float m_pitch_pos;


	static _vec3 UP;

	static _app * _instance;
	static HINSTANCE win32_instance;

	static _array<_object*> object_array;

	static bool readrtmeshfile(const char* path,_mesh* submeshes);

};

