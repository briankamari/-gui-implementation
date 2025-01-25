#include "ui_image.h"

#include "app.h"
#include "d3d_window.h"
#include "d3d_manager.h"

ui_image::ui_image(){
	m_foreground_vertex_buffer = NULL;
	m_background_vertex_buffer = NULL;
	m_x = m_y = 0.0f;
	m_width  = 10.0f;
	m_height = 10.0f;

	m_image_texture = NULL;
}

bool ui_image::genbackgroundbuffer(){


	app_releasecom(m_foreground_vertex_buffer);

	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(6*sizeof(ui_vertex),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_background_vertex_buffer,0));
	if(!m_background_vertex_buffer){ app_throw("vertex buffer"); }

	ui_vertex * v = 0;

	app_throw_hr(m_background_vertex_buffer->Lock(0, 0, (void**)&v, 0));

	_vec2 uv_up_right   = _vec2( 1.0f , 0.0f );
	_vec2 uv_up_left    = _vec2( 0.0f , 0.0f );
	_vec2 uv_down_right = _vec2( 1.0f , 1.0f );
	_vec2 uv_down_left  = _vec2( 0.0f , 1.0f );

    v[0].m_vertex.x  = m_width + m_x;
	v[0].m_vertex.y  = 0.0f + m_y;
	v[0].m_uv        = uv_up_right;

	v[1].m_vertex.x  = 0.0f + m_x;
	v[1].m_vertex.y  = m_height + m_y;
	v[1].m_uv        = uv_down_left;

	v[2].m_vertex.x  = 0.0f + m_x;
	v[2].m_vertex.y  = 0.0f + m_y;
	v[2].m_uv        = uv_up_left;

	v[3].m_vertex.x  = m_width + m_x;
	v[3].m_vertex.y  = 0.0f + m_y;
	v[3].m_uv        = uv_up_right;

	v[4].m_vertex.x  = m_width + m_x;
	v[4].m_vertex.y  = m_height + m_y;
	v[4].m_uv        = uv_down_right;

	v[5].m_vertex.x  = 0.0f + m_x;
	v[5].m_vertex.y  = m_height + m_y;
    v[5].m_uv        = uv_down_left;

	app_throw_hr(m_background_vertex_buffer->Unlock());


	return true;
}

bool ui_image::init(){

	if(!app->m_ui->readbitmap("waterball.bmp") ){ return false; }

	uint32_t max = app->m_ui->m_image_width > app->m_ui->m_image_height?app->m_ui->m_image_width:app->m_ui->m_image_height;

	app_throw_hr(D3DXCreateTextureFromFile(_api_manager->m_d3ddevice, "waterball.bmp", &m_image_texture));

	genbackgroundbuffer();
	return true;
}

void ui_image::clear(){
	app_releasecom(m_background_vertex_buffer);
}

bool ui_image::update(){

	app_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_foreground));
	app_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_foreground_vertex_declaration ));
	app_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,m_background_vertex_buffer, 0, sizeof(ui_vertex)));


	app_throw_hr(_fx->SetTexture(_api_manager->m_htex, m_image_texture ));

	app_throw_hr(_fx->Begin(NULL, 0));
	app_throw_hr(_fx->BeginPass(0));
	app_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,2) );
	app_throw_hr(_fx->EndPass());
	app_throw_hr(_fx->End());

	return true;
}

void ui_image::msgproc(UINT msg, WPARAM wParam, LPARAM lParam){

	if( (msg != WM_LBUTTONDOWN) && (app->m_ui->m_current_control != m_id) ){ return; }

	switch( msg )
	{
	case WM_LBUTTONDOWN : { if(intersection_test()){ app->m_ui->m_current_control = m_id; } }break;
	}

}

