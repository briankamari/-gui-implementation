#include "ui_text.h"

#include "app.h"
#include "d3d_window.h"
#include "d3d_manager.h"

ui_text::ui_text(void) {
	m_x = m_y = 0.0f;
	m_width  = 10.0f;
	m_height = 10.0f;

	m_charcount = 0;

	m_vscroll = 0;
	m_hscroll = 0;
	m_highlight = 0;
	m_text_position = 0;
	m_line_position = -1;

	m_highlight_vertex_count =0;

	m_coursor_buffer = NULL;
	m_highlight_buffer = NULL;

}

void ui_text::stringgeneration(){
	if(m_string.m_count==0){ 
		m_strings.clear();
		return; 
	}
	m_strings = _stringsplit_(m_string,'\n');
}

void  ui_text::currentposition(uint32_t *x,uint32_t * y){

	uint32_t i =0, count = 0;
	while( i<m_strings.m_count ) {
		count += m_strings[i++].m_count; 
		if(count >= m_text_position ){ break; }
		count++;
	}
	if(m_string.m_count ) {
		(*y) = i-1;
		if( (m_strings[ (*y) ].m_count==0) ){ (*x)=0; }
		else { (*x) = m_strings[(*y)].m_count - ((count) - m_text_position ); }
	}
}

void  ui_text::currentposition(uint32_t *x,uint32_t * y,uint32_t position){

	uint32_t i =0, count = 0;
	while( i<m_strings.m_count ) {
		count += m_strings[i++].m_count; 
		if(count >= position ){ break; }
		count++;
	}
	if(m_string.m_count ) {
		(*y) = i-1;
		if( (m_strings[ (*y) ].m_count==0) ){ (*x)=0; }
		else { (*x) = m_strings[(*y)].m_count - ((count) - position ); }
	}
}

uint32_t ui_text::getlineposition(bool isup,uint32_t x,uint32_t y){

	uint32_t line = (isup)?(y-1):(y+1);
	uint32_t extra =0;
	if( m_strings[line].m_count>x) { extra = (m_strings[line].m_count-x); }
	extra++;

	uint32_t current_line = x>m_strings[y].m_count?m_strings[y].m_count:x;
	uint32_t text_position;

	if(isup) { 
		text_position = m_text_position - ( current_line + extra ); 
	}
	else { 
		uint32_t x_ = m_strings[line].m_count<x?m_strings[line].m_count:x;
		uint32_t x__ = m_strings[y].m_count<x?m_strings[y].m_count:x;
		text_position = m_text_position + (m_strings[y].m_count-x__)+x_+1; 
	}
	return text_position;
}

bool ui_text::gencoursorbuffer(){

	app_releasecom(m_coursor_buffer);
	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(6*sizeof(_vec3),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_coursor_buffer,0)  );
	if(!m_coursor_buffer){ app_throw("vertex buffer"); }

	uint32_t x_=0,y_=0;
	currentposition(&x_,&y_);

	if( (m_string.m_count)&&(m_text_position==m_string.m_count)&&(m_string[m_text_position-1] == '\n') ) { y_++; x_=0; }

	_vec3 * v = 0;
	app_throw_hr(m_coursor_buffer->Lock(0, 0, (void**)&v, 0));

	float coursor_width = 2.0f;
	float x = m_x + ( (x_-m_hscroll)*m_font_width  );
	float y = m_y + ( (y_-m_vscroll)*m_font_height );

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

bool ui_text::genhighlightbuffer(){

	if( !testflags(ui_highlight) || (m_text_position == m_highlight) ){ return true; }

	uint32_t start = m_highlight>m_text_position? m_text_position : m_highlight;
	uint32_t end   = m_highlight>m_text_position? m_highlight     : m_text_position;

	uint32_t s_x , s_y;
	currentposition(&s_x,&s_y,start);

	uint32_t e_x , e_y;
	currentposition(&e_x,&e_y,end);
	app_releasecom(m_highlight_buffer);

	uint32_t vertex_count = 6*m_strings.m_count;
	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(
		vertex_count * sizeof(_vec3),
		D3DUSAGE_WRITEONLY,0,
		D3DPOOL_MANAGED,&m_highlight_buffer,
		0)  );
	if(!m_highlight_buffer){ app_throw("vertex buffer"); }

	_vec3 * v = 0;
	app_throw_hr(m_highlight_buffer->Lock(0, 0, (void**)&v, 0));

	uint32_t _x , _y;
	currentposition(&_x,&_y);

	m_highlight_vertex_count = 0;
	for(uint32_t i=s_y;i<=e_y;i++){

		float x(0),y(0),width(0);
		if(s_y == e_y){
			x = m_x + (s_x*m_font_width  );
			y = m_y + (s_y*m_font_height );        
			width = (e_x - s_x) * m_font_width;    
		}else { 
			x = m_x;
			y = m_y+( i *m_font_height); 
			width = (i==s_x)? (m_strings[i].m_count-s_x)*m_font_width:m_font_width;        

			if(i==s_y) { 
				x += s_x*m_font_width;
				width = (m_strings[i].m_count-s_x)*m_font_width;
			}else if(i==e_y){ 
				width = e_x*m_font_width; 
			}else if(m_strings[i].m_count==0){
				width = m_font_width/2;
			}else { width = m_strings[i].m_count*m_font_width; }
		}
		x-=(m_hscroll*m_font_width);
		float test_x = (x-m_x);
		if( (width+test_x)>m_width){ 
			width -= ((width+test_x)-m_width);
		}
		if(x<m_x){
			if((width+test_x)<m_x){ continue; }
			width -=(m_x-x);
			x=m_x;
		}
		y-=(m_vscroll*m_font_height);
		if( y>=(m_y+m_height) ){ continue; }
		if( y < m_y ){ continue; }

		v[m_highlight_vertex_count].x = width + x;
		v[m_highlight_vertex_count].y = 0.0f + y;
		m_highlight_vertex_count++;
		v[m_highlight_vertex_count].x = 0.0f +x;
		v[m_highlight_vertex_count].y = m_font_height + y;
		m_highlight_vertex_count++;
		v[m_highlight_vertex_count].x = 0.0f + x;
		v[m_highlight_vertex_count].y = 0.0f + y;
		m_highlight_vertex_count++;
		v[m_highlight_vertex_count].x = width + x;
		v[m_highlight_vertex_count].y = 0.0f + y;
		m_highlight_vertex_count++;
		v[m_highlight_vertex_count].x = width + x;
		v[m_highlight_vertex_count].y = m_font_height + y;
		m_highlight_vertex_count++;
		v[m_highlight_vertex_count].x = 0.0f + x;
		v[m_highlight_vertex_count].y = m_font_height + y;
		m_highlight_vertex_count++;
	}
	app_throw_hr(m_highlight_buffer->Unlock());
	return true;
}

bool ui_text::genbackgroundbuffer(){

	app_releasecom(m_foreground_vertex_buffer);
	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(6 * sizeof(_vec3),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_background_vertex_buffer,0)  );
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

bool ui_text::genforegroundbuffer(){

	app_releasecom(m_foreground_vertex_buffer);
	if(m_string.m_count == 0 ){ return true;}

	uint32_t vertical_available = uint32_t(m_height/m_font_height);
	vertical_available--;

	uint32_t horizontal_available = uint32_t(m_width/m_font_width);

	uint32_t x_=0,y_=0;
	currentposition(&x_,&y_);

	if(  x_<m_hscroll) { m_hscroll-= m_hscroll-x_; }
	if( (x_-m_hscroll) > horizontal_available){ m_hscroll = int32_t(x_-horizontal_available); }
	if( (m_hscroll>0)&&(x_ <= m_hscroll) ){ m_hscroll--; }
	x_=x_-m_hscroll;

	if( (m_string.m_count) && (m_text_position == m_string.m_count) && (m_string[m_text_position-1] == '\n' ) ) { 
		m_hscroll=0; y_++; x_=0; 
	}

	if(y_ < m_vscroll){ m_vscroll-= m_vscroll-y_; }
	if( (y_-m_vscroll) > vertical_available){ m_vscroll = int32_t(y_-vertical_available); }
	if( (m_vscroll>0)&&(y_ <= m_vscroll) ){ m_vscroll--; }
	y_=y_-m_vscroll;

	uint32_t vertex_count = 6*m_string.m_count;

	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(vertex_count*sizeof(ui_vertex),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_foreground_vertex_buffer,0)  );
	if(!m_foreground_vertex_buffer){ app_throw("vertex buffer"); }

	ui_vertex * v_ = 0;
	app_throw_hr(m_foreground_vertex_buffer->Lock(0, 0, (void**)&v_, 0));

	m_charcount =0;
	float x = m_x , y =  m_y;
	for(uint32_t l=0;l< (m_strings.m_count-m_vscroll) ;l++){

		uint32_t line = l+m_vscroll;
		if( (m_strings[line].m_count>0) && (int(m_strings[line].m_count)-int(m_hscroll))>0 ){
			
			uint32_t available = uint32_t(m_width/m_font_width);
			uint32_t char_count = m_strings[line].m_count>available?available:m_strings[line].m_count;
			for(uint32_t i=0;i<char_count;i++){

				_vec2 character = ui::s_font_vectors[ m_strings[line][i+m_hscroll] ];

				uint32_t x_scroll = uint32_t(m_x - (m_hscroll*m_font_width) );

				_vec3 vertex_up_left    = _vec3( x+i*m_font_width              , y+(m_font_height*l)               ,0);
				_vec3 vertex_up_right   = _vec3( x+i*m_font_width+m_font_width , y+(m_font_height*l)               ,0);
				_vec3 vertex_down_right = _vec3( x+i*m_font_width+m_font_width , y+m_font_height+(m_font_height*l) ,0);
				_vec3 vertex_down_left  = _vec3( x+i*m_font_width              , y+m_font_height+(m_font_height*l) ,0);

				float font_with_part = (1.0f/16.0f)/16.0f;

				_vec2 uv_up_right    = _vec2( character.x+font_with_part*m_font_width , character.y );
				_vec2 uv_up_left     = _vec2( character.x                             , character.y );
				_vec2 uv_down_right  = _vec2( character.x+font_with_part*m_font_width , character.y+(1.0f/16.0f) );
				_vec2 uv_down_left   = _vec2( character.x                             , character.y+(1.0f/16.0f) );

				v_[(i*6)+0+m_charcount].m_vertex = vertex_up_right;
				v_[(i*6)+0+m_charcount].m_uv     = uv_up_right;
				v_[(i*6)+1+m_charcount].m_vertex = vertex_down_left;
				v_[(i*6)+1+m_charcount].m_uv     = uv_down_left;
				v_[(i*6)+2+m_charcount].m_vertex = vertex_up_left;
				v_[(i*6)+2+m_charcount].m_uv     = uv_up_left;
				v_[(i*6)+3+m_charcount].m_vertex = vertex_up_right;
				v_[(i*6)+3+m_charcount].m_uv     = uv_up_right;
				v_[(i*6)+4+m_charcount].m_vertex = vertex_down_right;
				v_[(i*6)+4+m_charcount].m_uv     = uv_down_right;
				v_[(i*6)+5+m_charcount].m_vertex = vertex_down_left;
				v_[(i*6)+5+m_charcount].m_uv     = uv_down_left;
			}
			m_charcount+=(m_strings[line].m_count-m_hscroll)*6;
		}
	}
	app_throw_hr(m_foreground_vertex_buffer->Unlock());
	return true;
}
bool ui_text::init(){

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

void ui_text::clear(){
	app_releasecom(m_foreground_vertex_buffer);
	app_releasecom(m_background_vertex_buffer);
}

bool ui_text::update(){

	app_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_background));
	app_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_background_vertex_declaration ));
	app_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,m_background_vertex_buffer, 0, sizeof(_vec3)));

	app_throw_hr(_fx->SetValue(_api_manager->m_hcolor, (D3DXCOLOR*)&m_background_color, sizeof(D3DXCOLOR)));
	app_throw_hr(_fx->Begin(NULL, 0));
	app_throw_hr(_fx->BeginPass(0));
	app_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,2) );
	app_throw_hr(_fx->EndPass());
	app_throw_hr(_fx->End());

	if( (m_string.m_count>0) && (m_foreground_vertex_buffer) ){
		app_throw_hr(_fx->SetTexture(_api_manager->m_htex, m_font_texture ));
		app_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_foreground));
		app_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_foreground_vertex_declaration ));
		app_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,m_foreground_vertex_buffer, 0, sizeof(ui_vertex)));

		app_throw_hr(_fx->Begin(NULL, 0));
		app_throw_hr(_fx->BeginPass(0));

		app_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,(m_charcount/3)) );
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

		uint32_t count = testflags(ui_highlight)?m_highlight_vertex_count/3:2;
		app_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,count) );
		app_throw_hr(_fx->EndPass());
		app_throw_hr(_fx->End());
	}
	return true;
}

static bool chartest(WPARAM wParam){
	if( app->testflags(app_controldown)||(wParam == VK_ESCAPE)||( wParam == VK_BACK ) ){ return false; }
	return true;
}

void  ui_text::start_highlight(){
	if(app->testflags(app_shiftdown) && !testflags(ui_highlight) ){
		addflags(ui_redraw);
		addflags(ui_highlight);
		m_highlight=m_text_position;
	}
}

void  ui_text::end_highlight(){
	if( !app->testflags(app_shiftdown) || (m_text_position==m_highlight) ){ removeflags(ui_highlight); }
	m_line_position = -1;
}

void ui_text::position_proc(uint32_t * start,uint32_t * end){
	( *start) = m_highlight<m_text_position?m_highlight:m_text_position;
	( *end  ) = m_highlight<m_text_position?m_text_position:m_highlight;
	uint32_t c_x , c_y;
	currentposition(&c_x,&c_y, (*start) );
	if(c_x<m_hscroll){ m_hscroll = c_x; }
	if(c_y<m_vscroll){ m_vscroll = c_y; }
}

void ui_text::msgproc(UINT msg, WPARAM wParam, LPARAM lParam){

	if( (msg != WM_LBUTTONDOWN) && (app->m_ui->m_current_control != m_id) ){ return; }

	switch( msg )
	{
	case WM_LBUTTONDOWN : { if(intersection_test()){ app->m_ui->m_current_control = m_id; } } break;
	case WM_KEYDOWN:{

		if( wParam == VK_RETURN ){

			if(testflags(ui_highlight)){
				uint32_t start,end;
				position_proc(&start,&end);
				_character_insert(char('\n'),&m_string,start,end);
				m_text_position = start+1;
				addflags(ui_redraw);
				removeflags(ui_highlight);
			}else{
				if(m_string.m_count==m_text_position){ 
					m_string.pushback(char('\n')); 
					m_text_position++;
				}
				else{
					_character_insert(char('\n'),&m_string,m_text_position,m_text_position);
					m_text_position++;
				}
			}
			addflags(ui_redraw);
			stringgeneration();
			end_highlight();
		}
		if( wParam == VK_BACK){

			if(testflags(ui_highlight)){
				uint32_t start,end;
				position_proc(&start,&end);
				_string_insert(NULL,&m_string,start,end);
				m_text_position = start;
				addflags(ui_redraw);
				removeflags(ui_highlight);
			}else{
				if(m_text_position>0){
					if(m_string.m_count==m_text_position){ m_string.pop(); }
					else{ _string_insert(NULL,&m_string,m_text_position-1,m_text_position); }
					m_text_position--;
					if(m_hscroll>0){ m_hscroll --; }
					addflags(ui_redraw);
				}
			}
			stringgeneration();
			end_highlight();
		}
		if(wParam == VK_LEFT ){
			start_highlight();
			if(m_text_position>0){
				m_text_position--;
				addflags(ui_redraw);
			}
			end_highlight();
		}
		if(wParam == VK_RIGHT ){
			start_highlight();
			if(m_text_position<m_string.m_count){
				m_text_position++;
				addflags(ui_redraw);
			}
			end_highlight();
		}
		if(wParam == VK_UP ){
			start_highlight();
			uint32_t x_=0,y_=0;
			currentposition(&x_,&y_);
			if( m_line_position == -1 ){ m_line_position=x_; }
			if(y_ >0){
				m_text_position = getlineposition(true,m_line_position,y_);
				addflags(ui_redraw);
			}
			end_highlight();
		}
		if(wParam == VK_DOWN ){
			start_highlight();
			uint32_t x_=0,y_=0;
			currentposition(&x_,&y_);
			if( m_line_position == -1 ){  m_line_position=x_; }
			if( (m_strings.m_count>1) && ( y_ <(m_strings.m_count-1) ) ){
				m_text_position = getlineposition(false,m_line_position,y_);
				addflags(ui_redraw);
			}
			end_highlight();
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
				uint32_t start,end;
				position_proc(&start,&end);

				uint32_t length = end-start;
				char* clipboard_buffer = new char[length+1];
				app_zero(clipboard_buffer, length+1 );
				for(uint32_t i=0;i<length;i++) { clipboard_buffer[i] = m_string.m_data[i+start]; }
				app->m_ui->setclipboard(clipboard_buffer);

				_string_insert(NULL,&m_string,start,end);
				m_text_position = start;

				stringgeneration();
				addflags(ui_redraw);
				removeflags(ui_highlight);
			}
		}
		if( app->testflags(app_controldown)&&(wParam == 0x56/*V*/) ){

			_string clipboard_string = app->m_ui->getclipboard();
			if(clipboard_string.m_count>0){
				if( testflags(ui_highlight) ){
					uint32_t start,end;
					position_proc(&start,&end);
					_string_insert(clipboard_string.m_data,&m_string,start,end);
					m_text_position = start+clipboard_string.m_count;
					stringgeneration();
					addflags(ui_redraw);
					removeflags(ui_highlight);
				}else{
					_string_insert(clipboard_string.m_data,&m_string,m_text_position,m_text_position);
					m_text_position+=clipboard_string.m_count;
					stringgeneration();
					addflags(ui_redraw);
				}
			}
		}
					}break;

	case WM_CHAR:{
		if( chartest(wParam) ){

			if(testflags(ui_highlight)){
				uint32_t start,end;
				position_proc(&start,&end);
				_character_insert(wParam,&m_string,start,end);
				m_text_position = start+1;
				addflags(ui_redraw);
				stringgeneration();
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
				addflags(ui_redraw);
				stringgeneration();
			}
			end_highlight();
		}
				 }break;
	}
	if( testflags(ui_redraw) ){
		genforegroundbuffer();
		gencoursorbuffer();
		genhighlightbuffer();
		removeflags(ui_redraw);
	}
}

void ui_text::settext(const char* text){
	if(text){
		m_string = text;
		m_text_position = m_string.m_count;
		stringgeneration();
		genforegroundbuffer();
		gencoursorbuffer();
		genhighlightbuffer();
	}
}