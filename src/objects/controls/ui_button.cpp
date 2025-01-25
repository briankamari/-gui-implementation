#include "ui_button.h"

#include "app.h"
#include "d3d_window.h"
#include "d3d_manager.h"

#include "grayman.h"
#include "ui_static.h"

ui_button::ui_button(){
	m_foreground_vertex_buffer = NULL;
	m_background_vertex_buffer = NULL;
	m_x = m_y = 0.0f;
	m_width  = 10.0f;
	m_height = 10.0f;

}

bool ui_button::genbackgroundbuffer(){


	app_releasecom(m_foreground_vertex_buffer);

	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(6 * sizeof(_vec3),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_background_vertex_buffer,0));
	if(!m_background_vertex_buffer){ app_throw("vertex buffer"); }

	_vec3 * v = 0;
	app_throw_hr(m_background_vertex_buffer->Lock(0, 0, (void**)&v, 0));

	v[0].x = m_width + m_x;
	v[0].y = 0.0f + m_y;

	v[1].x = 0.0f + m_x;
	v[1].y = m_height + m_y;

	v[2].x = 0.0f + m_x;
	v[2].y = 0.0f + m_y;

	v[3].x = m_width + m_x;
	v[3].y = 0.0f + m_y;

	v[4].x = m_width + m_x;
	v[4].y = m_height + m_y;

	v[5].x = 0.0f + m_x;
	v[5].y = m_height + m_y;

	app_throw_hr(m_background_vertex_buffer->Unlock());
	return true;
}
bool ui_button::genforegroundbuffer(){

	if(m_string.m_count == 0 ){ return true;}
	app_releasecom(m_foreground_vertex_buffer);

	uint32_t vertex_count = 6*m_string.m_count;

	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(vertex_count * sizeof(ui_vertex),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_foreground_vertex_buffer,0) );
	if(!m_foreground_vertex_buffer){ app_throw("vertex buffer"); }

	ui_vertex * v_ = 0;
	app_throw_hr(m_foreground_vertex_buffer->Lock(0, 0, (void**)&v_, 0));

	float font_width  = 8;
	float font_height = 16;
	float text_width  = font_width*m_string.m_count;

	bool  larger_text_width = m_width<text_width;
	float x = larger_text_width? m_x: ((m_width-text_width)/2)+m_x;

	bool  larger_text_height   = m_height<font_height;
	float y = larger_text_height? m_y: ((m_height-font_height)/2)+m_y;

	float count = larger_text_width?m_width/font_width:m_string.m_count;
	for(uint32_t i=0;i<count;i++){

		_vec2 character = ui::s_font_vectors[ m_string[i] ];

		_vec3 vertex_up_left    = _vec3( x+i*font_width           , y ,0);
		_vec3 vertex_up_right   = _vec3( x+i*font_width+font_width, y ,0);
		_vec3 vertex_down_right = _vec3( x+i*font_width+font_width, y+font_height ,0);
		_vec3 vertex_down_left  = _vec3( x+i*font_width           , y+font_height ,0);

		float font_with_part = (1.0f/16.0f)/16.0f;

		_vec2 uv_up_right   = _vec2( character.x +  font_with_part*font_width , character.y );
		_vec2 uv_up_left    = _vec2( character.x,character.y);
		_vec2 uv_down_right = _vec2( character.x +  font_with_part*font_width ,character.y+ (1.0f/16.0f) );
		_vec2 uv_down_left  = _vec2( character.x, character.y+ (1.0f/16.0f) );

		v_[(i*6)+0].m_vertex = vertex_up_right;
		v_[(i*6)+0].m_uv = uv_up_right;

		v_[(i*6)+1].m_vertex = vertex_down_left;
		v_[(i*6)+1].m_uv = uv_down_left;

		v_[(i*6)+2].m_vertex = vertex_up_left;
		v_[(i*6)+2].m_uv = uv_up_left;

		v_[(i*6)+3].m_vertex = vertex_up_right;
		v_[(i*6)+3].m_uv = uv_up_right;

		v_[(i*6)+4].m_vertex = vertex_down_right;
		v_[(i*6)+4].m_uv = uv_down_right;

		v_[(i*6)+5].m_vertex = vertex_down_left;
		v_[(i*6)+5].m_uv = uv_down_left;

	}
	app_throw_hr(m_foreground_vertex_buffer->Unlock());
	return true;
}
bool ui_button::init(){

	app_throw_hr( D3DXCreateTexture(_api_manager->m_d3ddevice,font_texture_size,font_texture_size,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&m_font_texture) );

	D3DLOCKED_RECT rect;
	app_throw_hr( m_font_texture->LockRect(0,&rect,0,D3DLOCK_DISCARD) );

	D3DLOCKED_RECT main_rect;
	app_throw_hr( app->m_ui->m_main_font_texture->LockRect(0,&main_rect,0,D3DLOCK_DISCARD) );

	D3DCOLOR * pixel = NULL;
	uint8_t  * pixel_pointer = (uint8_t*)rect.pBits;

	D3DCOLOR * main_pixel = NULL;
	uint8_t  * main_pixel_pointer = (uint8_t*)main_rect.pBits;

	int count =font_texture_size;
	for( uint32_t i = 0; i < font_texture_size; i++) {
		pixel_pointer += rect.Pitch;
		pixel = (D3DCOLOR*)pixel_pointer;

		main_pixel_pointer += main_rect.Pitch;
		main_pixel = (D3DCOLOR*)main_pixel_pointer;

		for( int32_t j = 0; j < font_texture_size; j++ ){
			if(   (  (uint8_t)0xFF & ( main_pixel[j] >>24) )     >0  ){
				pixel[j] = D3DCOLOR_ARGB(
					uint8_t(m_foreground_color.w*0xFF),
					uint8_t(m_foreground_color.x*0xFF),
					uint8_t(m_foreground_color.y*0xFF),
					uint8_t(m_foreground_color.z*0xFF) );
			}else{ pixel[j] = 0; }
		}
	}
	m_font_texture->UnlockRect(0);
	app->m_ui->m_main_font_texture->UnlockRect(0);

	genbackgroundbuffer();
	genforegroundbuffer();
	return true;
}

void ui_button::clear(){
	app_releasecom(m_foreground_vertex_buffer);
	app_releasecom(m_background_vertex_buffer);
}

bool ui_button::update(){

	app_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_background));
	app_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_background_vertex_declaration ));
	app_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,m_background_vertex_buffer, 0, sizeof(_vec3)));

	_vec4 background_color = testflags(ui_mouse_over)?m_alt_color:m_background_color;
	background_color = testflags(ui_left_button_down)?m_background_color:background_color;

	app_throw_hr(_fx->SetValue(_api_manager->m_hcolor, (D3DXCOLOR*)&background_color, sizeof(D3DXCOLOR)));
	app_throw_hr(_fx->Begin(NULL, 0));
	app_throw_hr(_fx->BeginPass(0));
	app_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,2) );
	app_throw_hr(_fx->EndPass());
	app_throw_hr(_fx->End());

	app_throw_hr(_fx->SetTexture(_api_manager->m_htex, m_font_texture ));
	app_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_foreground));
	app_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_foreground_vertex_declaration ));
	app_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,m_foreground_vertex_buffer, 0, sizeof(ui_vertex)));

	app_throw_hr(_fx->Begin(NULL, 0));
	app_throw_hr(_fx->BeginPass(0));
	app_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,m_string.m_count*2) );
	app_throw_hr(_fx->EndPass());
	app_throw_hr(_fx->End());

	return true;
}

void ui_button::msgproc(UINT msg, WPARAM wParam, LPARAM lParam){

	if( (msg != WM_MOUSEMOVE)&&(msg != WM_LBUTTONDOWN) && (app->m_ui->m_current_control != m_id) ){ return; }

	switch( msg )
	{
	case WM_LBUTTONDOWN :{
		if( testflags(ui_mouse_over) ){ 
			app->m_ui->m_current_control = m_id; 
			addflags(ui_left_button_down);

			if(m_id==app->m_animation_button_id){
				if(app->m_keyframe>=3){ app->m_keyframe=0; }
				else{ app->m_keyframe++; }
				if(app->m_keyframe ==0 ){ 
					app->m_grayman->setbindpose(); 
					app->m_animation_control->settext("setbindpose");
				}
				if(app->m_keyframe ==1 ){
					/*walk animation*/
					app->m_grayman->keyframe(1,19);
					/*seconds per frame*/
					app->m_grayman->m_animation_length = 0.04f;
					app->m_animation_control->settext("walk");
				}
				if(app->m_keyframe ==2 ){
					/*wave animation*/
					app->m_grayman->keyframe(21,59);
					/*seconds per frame*/
					app->m_grayman->m_animation_length = 0.04f;
					app->m_animation_control->settext("wave");
				}
				if(app->m_keyframe ==3 ){
					/*stretch animation*/
					app->m_grayman->keyframe(60,99);
					/*seconds per frame*/
					app->m_grayman->m_animation_length = 0.08f;
					app->m_animation_control->settext("strech");
				}
			}

		}
						 }break;
	case WM_LBUTTONUP :{
		if( testflags(ui_mouse_over) ){ 
			removeflags(ui_left_button_down);
		}
					   }break;

	case WM_MOUSEMOVE :{
		if(intersection_test()){ addflags(ui_mouse_over); }
		else{ removeflags(ui_mouse_over); }
					   }
	case WM_KEYDOWN:{


		if( wParam == VK_ESCAPE ){
			DestroyWindow(_window->m_hwnd);
		}else if( wParam == 'C' ){
			printf("c");

		}
					}
					break;
	}

}

void ui_button::settext(const char* text){
	if(text){
		m_string = text;
		genforegroundbuffer();
	}
}

