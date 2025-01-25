#include "ui.h"

#include "app.h"
#include "d3d_window.h"
#include "d3d_manager.h"

_vec2 * ui::s_font_vectors = NULL;

#define bmp_throw(x) { fclose(file); app_throw(x); }

 bool ui_control::chartest(WPARAM wParam){
	if( app->testflags(app_controldown)||(wParam == VK_ESCAPE)||( wParam == VK_BACK )||( wParam == VK_RETURN ) ){ return false; }
	return true;
}

ui_control::ui_control(){

	m_x = m_y = 0;
	m_width = m_height =10.0f;
	m_font_width  = 8;
	m_font_height = 16;

	m_font_texture = NULL;
	m_foreground_vertex_buffer = NULL;
	m_background_vertex_buffer = NULL;
}
bool ui_control::intersection_test(){
	WINDOWINFO pwi;
	GetWindowInfo( _window->m_hwnd,&pwi);
	
	if( (app->m_x_cursor_pos) < float(pwi.rcClient.left+m_x) ){ return false; }
	if( (app->m_x_cursor_pos) > float(pwi.rcClient.left+m_x+m_width) ){ return false; }

	if( (app->m_y_cursor_pos) < float(pwi.rcClient.top+m_y) ){ return false; }
	if( (app->m_y_cursor_pos) > float(pwi.rcClient.top+m_y+m_height) ){ return false; }

	return true;
}

ui::ui() {

	if(!s_font_vectors) {
		s_font_vectors = new _vec2[256];
		for(uint16_t i=0;i<256;i++){
			s_font_vectors[i].x = (i%16)*0.0624f;
			s_font_vectors[i].y = floor(i/16.0f)*0.0624f;
		}
	}
	m_main_font_texture = NULL;
	m_current_control = -1;
}

bool ui::init(){
	float w = (float)_api_manager->m_d3dpp.BackBufferWidth;
	float h = (float)_api_manager->m_d3dpp.BackBufferHeight;

	m_projection = _ortho(0.0f,w,h,0.0f,-1000.0f,1000.0f);

	if(!readbitmap("courier.bmp") ){ return false; }
	app_throw_hr( D3DXCreateTexture(_api_manager->m_d3ddevice,m_image_width,m_image_height,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&m_main_font_texture) );

	D3DLOCKED_RECT rect;
	app_throw_hr( m_main_font_texture->LockRect(0,&rect,0,D3DLOCK_DISCARD) );

	D3DCOLOR * pixel = NULL;
	uint8_t  * pixel_pointer = (uint8_t*)rect.pBits;
	int count = m_image_size;
	for( uint32_t i = 0; i < m_image_height; i++) {
		pixel_pointer += rect.Pitch;
		pixel = (D3DCOLOR*)pixel_pointer;
		for( int32_t j = m_image_width-1; j >=0 ; j--, count-=4){
			if( (m_image_data[count+3])  < 254 ){
				pixel[j] = D3DCOLOR_ARGB(0,0,0,0);
			}else{
				pixel[j] = D3DCOLOR_ARGB(0xFF,0xFF,0xFF,0xFF);
			}
		}
	}
	m_main_font_texture->UnlockRect(0);

	for(uint32_t i=0;i<m_controls.m_count;i++){ m_controls[i]->init(); }

	return true;
}

void ui::clear(){

	for(uint32_t i=0;i<m_controls.m_count;i++){ m_controls[i]->clear(); }
	if(s_font_vectors) { delete [] s_font_vectors; }
	s_font_vectors = NULL;
	app_releasecom(m_main_font_texture);

}

bool ui::update(){


	app_throw_hr(_fx->SetMatrix(_api_manager->m_hmvp,(D3DXMATRIX*)&m_projection ));
	for(uint32_t i=0;i<m_controls.m_count;i++){ m_controls[i]->update(); }

	return true;
}

void ui::onlostdevice(){

}

void ui::onresetdevice(){
	float w = (float)_api_manager->m_d3dpp.BackBufferWidth;
	float h = (float)_api_manager->m_d3dpp.BackBufferHeight;
	m_projection = _ortho(0.0f,w,h,0.0f,-1000.0f,1000.0f);

}

bool ui::addcontrol(ui_control * control){
	if(!control){ app_throw("control pointer"); }

	control->m_id = m_controls.m_count; 
	m_controls.pushback(control);
	_app::object_array.pushback((_object*)control);

	return true;
}

bool ui::readbitmap(const char * filename){

	uint8_t   header[54];
	uint32_t  channel_count;

	FILE * file = NULL;
	fopen_s(&file,filename,"rb");
	if (!file){ app_throw("file"); }

	app_zero(header,54);
	if ( fread(header, 1, 54, file)!=54 )   {  bmp_throw("header"); }

	if ( header[0]!='B' || header[1]!='M' ) {  bmp_throw("not bmp file"); }

	/* 40 = BITMAPINFOHEADER 52 = BITMAPV2INFOHEADER 108 =BITMAPV4HEADER 124 = BITMAPV5HEADER */
	uint32_t headertype = *(uint32_t*)&(header[0x0E]);
	if( (headertype != 40) && (headertype != 52) && (headertype != 108) && (headertype != 124)  ){ bmp_throw("compression"); }

	/*  0 = BI_RGB  3 = BI_BITFIELDS  6 = BI_ALPHABITFIELDS */ 
	uint32_t compression = *(int*)&(header[0x1E]);
	if( compression== 0 )     { channel_count = 3; }
	else if( compression==3 ) { channel_count = headertype<108?3:4; }
	else if( compression==6 ) { channel_count = 4; }
	else { bmp_throw("compression"); }

	uint16_t bitcount = *(uint16_t*)&(header[0x1C]);
	if ( bitcount!=24 && bitcount!=32 )       { bmp_throw("bitcount"); }

	m_image_width          = *(uint32_t*)&(header[0x12]);
	m_image_height         = *(uint32_t*)&(header[0x16]);
	if(m_image_width==0 || m_image_height==0)   { bmp_throw("image dimensions"); }

	uint32_t data_position = *(uint32_t*)&(header[0x0A]);
	if(data_position==0)   { bmp_throw("data position "); }

	/*the image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps.*/
	m_image_size      = *(uint32_t*)&(header[0x22]);
	if(m_image_size==0) { m_image_size=m_image_width*m_image_height*channel_count; }
	else{ 
		if( (m_image_size/channel_count) != (m_image_width*m_image_height) ){ app_throw("imagesize"); }
	}

	uint8_t * image_data = new uint8_t [m_image_size];

	/* read pixel array*/
	fseek(file,0,SEEK_SET);
	fseek(file,data_position,SEEK_SET);
	if( fread(image_data,1,m_image_size,file) != m_image_size) { bmp_throw("fread"); }
	fclose (file);

	m_image_size = m_image_width*m_image_height*4;
	m_image_data = new uint8_t[m_image_size];
	uint32_t data_index = 0;
	for(uint32_t i =0; i<m_image_height ;i++){
		for(uint32_t ii =0; ii<(m_image_width*4) ;ii+=4,data_index+=channel_count ){
			m_image_data[ (i*m_image_width*4)+ii   ] = image_data[ data_index    ];
			m_image_data[ (i*m_image_width*4)+ii+1 ] = image_data[ data_index+1  ];
			m_image_data[ (i*m_image_width*4)+ii+2 ] = image_data[ data_index+2  ];
			if(channel_count==4){
				m_image_data[ (i*m_image_width*4)+ii+3 ] = image_data[ data_index+3  ];
			}else{
				m_image_data[ (i*m_image_width*4)+ii+3 ] = 0xFF;
			}
		}
	}

	delete [] image_data;
	return true;
}

_string ui::getclipboard(){
	_string result; 

	if (!IsClipboardFormatAvailable(CF_TEXT)) { app_error("getclipboard"); return result; } 
	if (!OpenClipboard(_window->m_hwnd))  { app_error("getclipboard"); return result; }

	HGLOBAL global = GetClipboardData(CF_TEXT); 
	if (global != NULL) { 
		char* string = (char*)GlobalLock(global); 
		if (string != NULL) { result = string; } 
		else{ app_error("Globallock"); }

		if(!GlobalUnlock(global)){ app_error("GlobalUnlock"); }
	}else { app_error("GetClipboardData"); }

	if(!CloseClipboard()){ app_throw("CloseClipboard"); } 
	return result;
}

bool ui::setclipboard(const _string & text){

	if(text.m_count==0){ app_throw("empty string");}

	// Open the clipboard, and empty it. 
	if(!OpenClipboard(_window->m_hwnd)){ app_throw("hwnd"); } 
	if(!EmptyClipboard()) { app_throw("EmptyClipboard"); }

	// Allocate a global memory object for the text. 
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE,(text.m_count+1)* sizeof(char)); 
	if (hglbCopy == NULL)  { CloseClipboard(); app_throw("hglbCopy"); } 

	// Lock the handle and copy the text to the buffer. 
	char* buffer = (char*)GlobalLock(hglbCopy); 
	app_zero( buffer,  (text.m_count+1)* sizeof(char) );
	memcpy(buffer, text.m_data, text.m_count* sizeof(char) ); 

	if(GlobalUnlock(hglbCopy)!=NO_ERROR){ app_throw("GlobalUnlock"); }
	// Place the handle on the clipboard. 
	if(SetClipboardData(CF_TEXT, hglbCopy) == NULL) { app_throw("SetClipboardData"); } 
	// Close the clipboard. 
	if(!CloseClipboard()){ app_throw("CloseClipboard"); } 

	return true;
}
