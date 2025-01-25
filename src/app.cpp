#include "app.h"

#include "d3d_window.h"
#include "d3d_manager.h"

#include "ui.h"

#include "ui_static.h"
#include "ui_line.h"
#include "ui_text.h"
#include "ui_button.h"
#include "ui_image.h"

#include "grayman.h"


_vec3   _app::UP = _vec3(0.0f,1.0f,0.0f);
_app *  _app::_instance = NULL;
HINSTANCE _app::win32_instance = NULL;

_array<_object*> _app::object_array;

_app::_app(){

	m_ui = NULL;
	m_grayman = NULL;
	m_animation_control = NULL;

	m_distance = 100.0f;

	m_up       = UP;
	m_look     = _vec3(0.0f,-1.0f,0.0f);
	m_target   = _vec3(0.0f,0.0f,0.0f);
	m_position = _vec3(0.0f,30.0f,0.0f);

	m_keyframe = 0;
	m_framespersecond = 0;
	m_last_frame_seconds = 0;
	m_last_frame_milliseconds = 0;

	m_y_pos = 0.0f;
	m_x_pos = 0.0f;

	m_yaw = -45.0f;
	m_pitch = -10.0f;
	m_yaw_pos = -45.0f;
	m_pitch_pos = -10.0f;
	m_x_cursor_pos = 0.0f;
	m_y_cursor_pos = 0.0f;
}

void _app::run(){

	m_grayman = new grayman();
	if(!m_grayman->init() ){ return; }
	m_grayman->setbindpose();

	m_keyframe = 1;
	m_grayman->keyframe(1,19);
	m_grayman->m_animation_length =0.04f;

	m_ui = new ui();

	ui_static * display_control = new ui_static();
	display_control->m_x      = 25;
	display_control->m_y      = 5;
	display_control->m_width  = 225;
	display_control->m_height = 20;
	display_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	display_control->m_foreground_color = _vec4(0.0f,0.0f,0.8f,0.8f);
	m_ui->addcontrol(display_control);

	m_animation_control = new ui_static();
	m_animation_control->m_x      = 25;
	m_animation_control->m_y      = 25;
	m_animation_control->m_width  = 225;
	m_animation_control->m_height = 20;
	m_animation_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	m_animation_control->m_foreground_color = _vec4(0.0f,0.0f,0.8f,0.8f);
	m_ui->addcontrol(m_animation_control);
	app->m_animation_control->settext("walk");

	ui_button * switch_control = new ui_button();
	switch_control->m_x      = 25;
	switch_control->m_y      = 45;
	switch_control->m_width  = 225;
	switch_control->m_height = 30;
	switch_control->m_background_color = _vec4(0.0f,0.0f,0.8f,0.8f);
	switch_control->m_foreground_color = _vec4(1.0f,1.0f,1.0f,0.8f);
	switch_control->m_alt_color = _vec4(0.8f,0.0f,0.0f,0.2f);
	m_ui->addcontrol(switch_control);
	m_animation_button_id = switch_control->m_id;
	switch_control->settext("animation switch");

	ui_button * button_control = new ui_button();
	button_control->m_x      = 25;
	button_control->m_y      = 80;
	button_control->m_width  = 225;
	button_control->m_height = 65;
	button_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.2f);
	button_control->m_foreground_color = _vec4(0.0f,0.0f,0.0f,0.8f);
	button_control->m_alt_color = _vec4(0.0f,0.0f,0.4f,0.2f);
	m_ui->addcontrol(button_control);
	button_control->settext("button");

	ui_image * image_control = new ui_image();
	image_control->m_x      = 255;
	image_control->m_y      = 45;
	image_control->m_width  = 320;
	image_control->m_height = 125;
	m_ui->addcontrol(image_control);

	ui_static * static_control = new ui_static();
	static_control->m_x      = 25;
	static_control->m_y      = 150;
	static_control->m_width  = 225;
	static_control->m_height = 20;
	static_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.2f);
	static_control->m_foreground_color = _vec4(0.0f,0.0f,0.0f,0.8f);
	m_ui->addcontrol(static_control);
    static_control->settext("static edit");

	ui_static * imagelabel_control = new ui_static();
	imagelabel_control->m_x      = 255;
	imagelabel_control->m_y      = 25;
	imagelabel_control->m_width  = 320;
	imagelabel_control->m_height = 20;
	imagelabel_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	imagelabel_control->m_foreground_color = _vec4(0.0f,0.0f,0.8f,0.8f);
	m_ui->addcontrol(imagelabel_control);
    imagelabel_control->settext("image control");

	printf(" static control id:  %u ",static_control->m_id);
	ui_line * line_control = new ui_line();
	line_control->m_x      = 25;
	line_control->m_y      = 175;
	line_control->m_width  = 550;
	line_control->m_height = 20;
	line_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.2f);
	line_control->m_foreground_color = _vec4(0.0f,0.0f,0.0f,0.8f);
	m_ui->addcontrol(line_control);
	line_control->settext("line edit");

	printf(" line control id:  %u ",line_control->m_id);
	ui_text * text_control = new ui_text();
	text_control->m_x      = 25;
	text_control->m_y      = 200;
	text_control->m_width  = 550;
	text_control->m_height = 250;
	text_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.2f);
	text_control->m_foreground_color = _vec4(0.0f,0.0f,0.0f,0.8f);
	m_ui->addcontrol(text_control);		
	text_control->settext("text edit");
	printf(" text control id:  %u ",text_control->m_id);

	if(!m_ui->init()){ return; }


	_window->update();

	int64_t tickspersecond = 0;
	int64_t previous_timestamp = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&tickspersecond);
	float secsPerCnt = 1.0f / (float)tickspersecond;

	QueryPerformanceCounter((LARGE_INTEGER*)&previous_timestamp);

	int frames = 0;
	float second =0.0f,coursor_second =0.0f;
	while( testflags(app_running) ){

		//**d3d device test.  error exits app**
		removeflags(app_lostdev);
		HRESULT hr = _api_manager->m_d3ddevice->TestCooperativeLevel();

		if( hr == D3DERR_DEVICELOST ) {  addflags( app_lostdev );  }
		else if( hr == D3DERR_DRIVERINTERNALERROR ) {
			addflags( app_deverror );
			app_error("d3d device error");
		}else if( hr == D3DERR_DEVICENOTRESET ){
			_api_manager->reset();
			addflags( app_lostdev );
		}
		//*************************************

		int64_t currenttimestamp = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&currenttimestamp);

		m_last_frame_seconds = float(currenttimestamp - previous_timestamp) * secsPerCnt;
		m_last_frame_milliseconds = m_last_frame_seconds*1000.0f;

		coursor_second += m_last_frame_seconds;
		second += m_last_frame_seconds;
		frames++;

		previous_timestamp = currenttimestamp;
		static _string stats;
		if(second >=1.0f){
			stats = _string("mspf: ")+_utility::floattostring(m_last_frame_milliseconds,true);
			stats = stats + _string(" fps: ");
			stats = stats + _utility::inttostring(frames);
			display_control->settext(stats.m_data);
			second = 0.0f;
			frames = 0;

		}

		if(coursor_second >= 0.5f){
			if(testflags(app_coursor_on)){ removeflags(app_coursor_on); }
			else{ addflags(app_coursor_on); }
			coursor_second = 0.0f;
		}

		if( !(testflags(app_lostdev)) ) {
			update();
			app_error_hr(_api_manager->m_d3ddevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0));
			app_error_hr(_api_manager->m_d3ddevice->BeginScene());

			m_grayman->update();
			m_ui->update(); 
			app_error_hr(_api_manager->m_d3ddevice->EndScene());
			app_error_hr(_api_manager->m_d3ddevice->Present(0, 0, 0, 0));
		}

		if( testflags(app_deverror) ) { removeflags(app_running); }
		else { _window->update(); }
	}
	m_ui->clear();
	clear();
}

bool _app::init(){

	win32_instance = GetModuleHandle(NULL);

	_window = new d3d_window();
	_api_manager = new d3d_manager();

	addflags(app_running);
	if(!_window->init()) { return false;}
	if(!_api_manager->init()) { return false; }


	float w = (float)_api_manager->m_d3dpp.BackBufferWidth;
	float h = (float)_api_manager->m_d3dpp.BackBufferHeight;
	m_projection = _perspectivefovrh(D3DX_PI * 0.25f, w,h, 1.0f, 1000.0f);

	return true;
}

void _app::clear(){
	if(_api_manager ) { _api_manager->clear();}
}

bool _app::update(){

	//camera*********************************************
	m_pitch = (m_pitch >  45.0f ) ? 45.0f  : m_pitch;
	m_yaw   = (m_yaw   >  180.0f) ? 180.0f : m_yaw;
	m_pitch = (m_pitch < -90.0f ) ?-90.0f  : m_pitch;
	m_yaw   = (m_yaw   < -180.0f) ?-180.0f : m_yaw;

	_mat4  r = _yawpitchroll(float(_radians(m_yaw)),float(_radians(m_pitch)),0.0f);
	_vec4 t  = r*_vec4(0,0,m_distance,0.0f);
	_vec4 up = r*_vec4(UP.x,UP.y,UP.z,0.0f);

	m_up = _vec3(up.x,up.y,up.z);
	m_position = m_target + _vec3(t.x,t.y,t.z);
	m_look = _normalize(m_target-m_position);

	m_view = rt_lookatrh(m_position, m_target, m_up);
	//***************************************************


	POINT cursor_position;
	if (GetCursorPos(&cursor_position)) {
		m_x_cursor_pos = float(cursor_position.x);
		m_y_cursor_pos = float(cursor_position.y);
	}else{ app_error("cursor pos");}

	return true;
}

void _app::onlostdevice() {
	app_error_hr(_fx->OnLostDevice());
}

void _app::onresetdevice() {
	app_error_hr(_fx->OnResetDevice());
}

bool _app::readrtmeshfile(const char* path,_mesh * mesh){

    FILE * file = NULL;
    fopen_s(&file,path,"rb");
    if(!file){ app_throw("readrtmeshfile"); }

    char  header_[9];
    app_zero(header_,9);

    if( fread(header_,1,6,file) != 6 ){ app_throw("fread"); }
    if(!app_scm(header_,"_mesh_")) { app_throw("not _mesh_ file"); }

    uint16_t submesh_count_ = 0;
    if( fread((&submesh_count_),2,1,file) != 1 ){ app_throw("fread"); }
    printf("smcount : %u \n",submesh_count_);

    for(uint32_t i=0;i<submesh_count_;i++){
        _submesh submesh_;
        uint32_t index_count_ = 0;
        if( fread((&index_count_),4,1,file) != 1 ){ app_throw("fread"); }

        int32_t * indices = new int32_t[index_count_];
        if( fread(indices,4,index_count_,file) != index_count_ ){ app_throw("fread"); }
        for(uint32_t ii=0;ii<index_count_;ii++){
            submesh_.m_indices.pushback(indices[ii],true);
        }

        uint32_t vertex_count_ = 0;
        if( fread((&vertex_count_),4,1,file) != 1 ){ app_throw("fread"); }

        _vertex * vertices = new _vertex[vertex_count_];
        if( fread(vertices,sizeof(_vertex),vertex_count_,file) != vertex_count_ ){ app_throw("fread"); }
        for(uint32_t ii=0;ii<vertex_count_;ii++){
            submesh_.m_vertices.pushback(vertices[ii],true);
        }
        mesh->m_submeshes.pushback(submesh_,true);
    }
    uint16_t bone_count = 0;
    if( fread((&bone_count),2,1,file) != 1 ){ app_throw("fread"); }
    if(bone_count){

        printf("bone_count: %i \n",bone_count);
        mesh->m_bones.allocate(bone_count);
        if( fread( (&(mesh->m_bones[0])) ,sizeof(_mat4 ),bone_count,file) != bone_count ){ app_throw("fread"); }

        uint16_t keyframe_count = 0;
        if( fread( (&keyframe_count) ,2,1,file) != 1 ){ app_throw("fread"); }
        if( keyframe_count){
            printf("keyframes: %i \n",keyframe_count);
            for(uint32_t ii=0;ii<keyframe_count;ii++){
                _matrix_array keyframe;
                keyframe.allocate(bone_count);
                if( fread( &(keyframe[0]) ,sizeof(_mat4 ),bone_count,file) != bone_count ){ app_throw("fread"); }
                mesh->m_keyframes.pushback(keyframe);
            }
        }
    }
    fclose(file);
    return true;
}
