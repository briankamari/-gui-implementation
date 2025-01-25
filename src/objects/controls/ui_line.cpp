#include "ui_line.h"

#include "app.h"
#include "d3d_window.h"
#include "d3d_manager.h"

ui_line::ui_line(void) {

	m_x = m_y = 0.0f;
	m_width  = 10.0f;
	m_height = 10.0f;

	m_scroll = 0;
	m_highlight = 0;
	m_text_position = 0;
	m_count = 0;
	m_coursor_buffer = NULL;
	m_highlight_buffer = NULL;
}

bool ui_line::gencoursorbuffer(){

	app_releasecom(m_coursor_buffer);

	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(
		6 * sizeof(_vec3),
		D3DUSAGE_WRITEONLY,0,
		D3DPOOL_MANAGED,&m_coursor_buffer,
		0)  );
	if(!m_coursor_buffer){ app_throw("vertex buffer"); }

	_vec3 * v = 0;

	app_throw_hr(m_coursor_buffer->Lock(0, 0, (void**)&v, 0));

	float coursor_width = 2.0f;
	float x = ((m_text_position-m_scroll)*m_font_width)+m_x;

	float y =  ((m_height-m_font_height)/2.0f)+m_y;
	v[0].x = coursor_width + x;
	v[0].y = 0.0f + y;

	v[1].x = 0.0f +x;
	v[1].y = m_font_height + y;

	v[2].x = 0.0f + x;
	v[2].y = 0.0f + y;

	v[3].x = coursor_width + x;
	v[3].y = 0.0f + y;

	v[4].x = coursor_width + x;
	v[4].y = m_font_height + y;

	v[5].x = 0.0f + x;
	v[5].y = m_font_height + y;


	app_throw_hr(m_coursor_buffer->Unlock());
	return true;
}
bool ui_line::genhighlightbuffer(){

	app_releasecom(m_highlight_buffer);

	if(  (m_string.m_count==0) || (m_highlight == m_text_position) ) { return true; }

	uint32_t start  = m_highlight>m_text_position?m_text_position:m_highlight;
	start  = m_scroll>start?m_scroll:start;
	uint32_t end    = m_highlight>m_text_position?m_highlight:m_text_position;
	uint32_t length = end-start;

	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(6 * sizeof(_vec3),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_highlight_buffer,0));
	if(!m_highlight_buffer){ app_throw("vertex buffer"); }
	_vec3 * v = 0;
	app_throw_hr(m_highlight_buffer->Lock(0, 0, (void**)&v, 0));

	float available = m_width/m_font_width;

	float width  = m_font_width*length;

	float x_ =   (start-m_scroll)*m_font_width;
	float x = m_x + x_;
	float y =  ((m_height-m_font_height)/2.0f)+m_y;

	width = (x_+width)>m_width?m_width-x_:width;

	v[0].x = width + x;
	v[0].y = 0.0f + y;

	v[1].x = 0.0f +x;
	v[1].y = m_font_height + y;

	v[2].x = 0.0f + x;
	v[2].y = 0.0f + y;

	v[3].x = width + x;
	v[3].y = 0.0f + y;

	v[4].x = width + x;
	v[4].y = m_font_height + y;

	v[5].x = 0.0f + x;
	v[5].y = m_font_height + y;


	app_throw_hr(m_highlight_buffer->Unlock());
	return true;
}

bool ui_line::genbackgroundbuffer(){


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
bool ui_line::genforegroundbuffer(){

	if(m_string.m_count == 0 ){ return true;}
	app_releasecom(m_foreground_vertex_buffer);

	uint32_t available = uint32_t(m_width/m_font_width);

	uint32_t position = uint32_t(m_text_position);

	if( (position-m_scroll) > available){ m_scroll = int32_t(position-available); }
	if( (m_scroll>0)&&(position <= m_scroll) ){ m_scroll--; }


	m_count = m_string.m_count - m_scroll;
	m_count = available<m_count?available:m_count;

	uint32_t vertex_count = 6*m_count;

	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(vertex_count * sizeof(ui_vertex),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_foreground_vertex_buffer,0)  );
	if(!m_foreground_vertex_buffer){ app_throw("vertex buffer"); }

	ui_vertex * v_ = 0;
	app_throw_hr(m_foreground_vertex_buffer->Lock(0, 0, (void**)&v_, 0));

	float text_width  = m_font_width*m_string.m_count;

	float x = m_x;

	float y =  ((m_height-m_font_height)/2.0f)+m_y;

	for(uint32_t i=0;i<m_count;i++){

		_vec2 character = ui::s_font_vectors[ m_string[i+m_scroll] ];

		_vec3 vertex_up_left    = _vec3( x+i*m_font_width              , y               ,0);
		_vec3 vertex_up_right   = _vec3( x+i*m_font_width+m_font_width , y               ,0);
		_vec3 vertex_down_right = _vec3( x+i*m_font_width+m_font_width , y+m_font_height ,0);
		_vec3 vertex_down_left  = _vec3( x+i*m_font_width              , y+m_font_height ,0);

		float font_with_part = (1.0f/16.0f)/16.0f;

		_vec2 uv_up_right    = _vec2( character.x+font_with_part*m_font_width , character.y );
		_vec2 uv_up_left     = _vec2( character.x                             , character.y );
		_vec2 uv_down_right  = _vec2( character.x+font_with_part*m_font_width , character.y+(1.0f/16.0f) );
		_vec2 uv_down_left   = _vec2( character.x                             , character.y+(1.0f/16.0f) );

		v_[(i*6)+0].m_vertex = vertex_up_right;
		v_[(i*6)+0].m_uv     = uv_up_right;

		v_[(i*6)+1].m_vertex = vertex_down_left;
		v_[(i*6)+1].m_uv     = uv_down_left;

		v_[(i*6)+2].m_vertex = vertex_up_left;
		v_[(i*6)+2].m_uv     = uv_up_left;

		v_[(i*6)+3].m_vertex = vertex_up_right;
		v_[(i*6)+3].m_uv     = uv_up_right;

		v_[(i*6)+4].m_vertex = vertex_down_right;
		v_[(i*6)+4].m_uv     = uv_down_right;

		v_[(i*6)+5].m_vertex = vertex_down_left;
		v_[(i*6)+5].m_uv     = uv_down_left;

	}
	app_throw_hr(m_foreground_vertex_buffer->Unlock());
	return true;
}
bool ui_line::init(){


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
	gencoursorbuffer();
	genhighlightbuffer();
	return true;
}

void ui_line::clear(){
	app_releasecom(m_foreground_vertex_buffer);
	app_releasecom(m_background_vertex_buffer);
}

bool ui_line::update(){

	app_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_background));
	app_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_background_vertex_declaration ));
	app_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,m_background_vertex_buffer, 0, sizeof(_vec3)));

	app_throw_hr(_fx->SetValue(_api_manager->m_hcolor, (D3DXCOLOR*)&m_background_color, sizeof(D3DXCOLOR)));
	app_throw_hr(_fx->Begin(NULL, 0));
	app_throw_hr(_fx->BeginPass(0));
	app_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,2) );
	app_throw_hr(_fx->EndPass());
	app_throw_hr(_fx->End());

	if(m_string.m_count-m_scroll>0){
		app_throw_hr(_fx->SetTexture(_api_manager->m_htex, m_font_texture ));
		app_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_foreground));
		app_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_foreground_vertex_declaration ));
		app_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,m_foreground_vertex_buffer, 0, sizeof(ui_vertex)));

		app_throw_hr(_fx->Begin(NULL, 0));
		app_throw_hr(_fx->BeginPass(0));
		app_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,m_count*2) );
		app_throw_hr(_fx->EndPass());
		app_throw_hr(_fx->End());
	}

	if( (app->testflags(app_coursor_on)&&(app->m_ui->m_current_control== m_id)) || testflags(ui_highlight) ){
		app_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_background));
		app_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_background_vertex_declaration ));

		IDirect3DVertexBuffer9 * buffer = testflags(ui_highlight)?m_highlight_buffer: m_coursor_buffer;
		app_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,buffer, 0, sizeof(_vec3)));

		_vec4 coursor_color = testflags(ui_highlight)?_vec4(1.0f,0.5f,0.3f,0.8f):_vec4(1.0f,0.5f,0.3f,1.0f);
		app_throw_hr(_fx->SetValue(_api_manager->m_hcolor, (D3DXCOLOR*)&coursor_color, sizeof(D3DXCOLOR)));

		app_throw_hr(_fx->Begin(NULL, 0));
		app_throw_hr(_fx->BeginPass(0));
		app_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,2) );
		app_throw_hr(_fx->EndPass());
		app_throw_hr(_fx->End());
	}

	return true;
}

void ui_line::msgproc(UINT msg, WPARAM wParam, LPARAM lParam){

	bool render_foreground = false;
	
	if( (msg != WM_LBUTTONDOWN) && (app->m_ui->m_current_control != m_id) ){ return; }
	switch( msg )
	{
	case WM_LBUTTONDOWN :{
		if(intersection_test()){ app->m_ui->m_current_control = m_id; }
						 }

	case WM_KEYDOWN:{
		if( wParam == VK_BACK){

			if(testflags(ui_highlight)){
				uint32_t start = m_highlight<m_text_position?m_highlight:m_text_position;
				uint32_t end   = m_highlight<m_text_position?m_text_position:m_highlight;
				if(start<m_scroll){ m_scroll = start; }
				_string_insert(NULL,&m_string,start,end);
				m_text_position = start;
				render_foreground = true;
				removeflags(ui_highlight);
			}else{
				if(m_text_position>0){
					if(m_string.m_count==m_text_position){ m_string.pop(); }
					else{ _string_insert(NULL,&m_string,m_text_position-1,m_text_position); }
					m_text_position--;
					if(m_scroll>0){ m_scroll --; }
					render_foreground = true;
				}
			}
		}
		if(wParam == VK_LEFT ){
			if(app->testflags(app_shiftdown) && !testflags(ui_highlight) ){
				addflags(ui_highlight);
				render_foreground = true;
				m_highlight=m_text_position;
			}
			if(m_text_position>0){
				m_text_position--;
				render_foreground = true;
			}
			if( !app->testflags(app_shiftdown) || (m_text_position==m_highlight) ){ removeflags(ui_highlight); }
		}
		if(wParam == VK_RIGHT ){
			if(app->testflags(app_shiftdown) && !testflags(ui_highlight) ){
				addflags(ui_highlight);
				render_foreground = true;
				m_highlight=m_text_position;
			}
			if(m_text_position<m_string.m_count){
				m_text_position++;
				render_foreground = true;
			}
			if( !app->testflags(app_shiftdown) || (m_text_position==m_highlight) ){ removeflags(ui_highlight); }
		}

		if(app->testflags(app_controldown)&&(wParam == 0x43/*c*/) ){
			if( testflags(ui_highlight) ){
				uint32_t start = m_highlight<m_text_position?m_highlight:m_text_position;
				uint32_t end   = m_highlight<m_text_position?m_text_position:m_highlight;

				char* buffer = new char[(end-start)+1];
				app_zero(buffer, (end-start)+1 );
				for(uint32_t i=0,ii=start;i<end;i++,ii++) { buffer[i] = m_string.m_data[ii]; }
				app->m_ui->setclipboard(buffer);
			}
		}
		if(app->testflags(app_controldown)&&(wParam == 0x58/*X*/) ){
			if( testflags(ui_highlight) ){
				uint32_t start = m_highlight<m_text_position?m_highlight:m_text_position;
				uint32_t end   = m_highlight<m_text_position?m_text_position:m_highlight;
				if(start<m_scroll){ m_scroll = start; }

				uint32_t length = end-start;
				char* clipboard_buffer = new char[length+1];
				app_zero(clipboard_buffer, length+1 );
				for(uint32_t i=0;i<length;i++) { clipboard_buffer[i] = m_string.m_data[i+start]; }
				app->m_ui->setclipboard(clipboard_buffer);

				_string_insert(NULL,&m_string,start,end);
				render_foreground = true;
				removeflags(ui_highlight);
			}
		}
		if( app->testflags(app_controldown)&&(wParam == 0x56/*V*/) ){
			_string clipboard_string = app->m_ui->getclipboard();
			if(clipboard_string.m_count>0){
				if( testflags(ui_highlight) ){
					uint32_t start = m_highlight<m_text_position?m_highlight:m_text_position;
					uint32_t end   = m_highlight<m_text_position?m_text_position:m_highlight;
					if(start<m_scroll){ m_scroll = start; }
					_string_insert(clipboard_string.m_data,&m_string,start,end);
					render_foreground = true;
					removeflags(ui_highlight);
				}else{
					_string_insert(clipboard_string.m_data,&m_string,m_text_position,m_text_position);
					m_text_position+=clipboard_string.m_count;
					render_foreground = true;
				}
			}
		}
					}break;

	case WM_CHAR:{
		if( chartest(wParam) ){

			if(testflags(ui_highlight)){
				uint32_t start = m_highlight<m_text_position?m_highlight:m_text_position;
				uint32_t end   = m_highlight<m_text_position?m_text_position:m_highlight;
				if(start<m_scroll){ m_scroll = start; }
				_character_insert(wParam,&m_string,start,end);
				m_text_position++;
				render_foreground = true;
				removeflags(ui_highlight);
			}else{
				if(m_string.m_count==m_text_position){ 
					m_string.pushback(char(wParam)); 
					m_text_position++;
				}
				else{
					_character_insert(wParam,&m_string,m_text_position,m_text_position);
					m_text_position++;
				}
				render_foreground = true;
			}
		}
				 }break;
	}
	if( render_foreground ){
		genforegroundbuffer();
		gencoursorbuffer();
		genhighlightbuffer();
	}
}

void ui_line::settext(const char* text){
	if(text){
		m_string = text;
		m_text_position = m_string.m_count;
		genforegroundbuffer();
		gencoursorbuffer();
		genhighlightbuffer();
	}
}