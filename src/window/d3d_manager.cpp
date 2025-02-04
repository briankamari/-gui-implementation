#include "d3d_manager.h"

#include "app.h"
#include "d3d_window.h"

d3d_manager* d3d_manager::_manager = NULL;

d3d_manager::d3d_manager(){

	m_fx     = NULL;

	app_zero(&m_d3dpp,sizeof(m_d3dpp));

	m_d3dobject = NULL;
	m_d3ddevice = NULL;

	m_object_vertex_declaration = NULL;
	m_ui_foreground_vertex_declaration = NULL;
	m_ui_background_vertex_declaration = NULL;

	m_hmvp                = (D3DXHANDLE)NULL;
	m_htex                = (D3DXHANDLE)NULL;
	m_hworld              = (D3DXHANDLE)NULL;
	m_htech_object        = (D3DXHANDLE)NULL;
	m_htech_ui_foreground = (D3DXHANDLE)NULL;
	m_htech_ui_background = (D3DXHANDLE)NULL;

	m_hbones              = (D3DXHANDLE)NULL;
	m_htech_bone          = (D3DXHANDLE)NULL;


}

bool d3d_manager::init(){

	m_d3dobject = Direct3DCreate9(D3D_SDK_VERSION);
	if( !m_d3dobject ) { app_throw("d3dobject"); }

	// Step 2: Verify hardware support for specified formats in windowed and full screen modes.
	D3DDISPLAYMODE mode;
	HRESULT HR = m_d3dobject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
	if(HR != D3D_OK){ app_throw("hr"); }

	app_throw_hr(m_d3dobject->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mode.Format, mode.Format, true));
	app_throw_hr(m_d3dobject->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false));

	// Step 3: Check for requested vertex processing and pure device.
	D3DCAPS9 caps;
	app_throw_hr(m_d3dobject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps));

	DWORD devbehaviorflags = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) { devbehaviorflags |= D3DCREATE_HARDWARE_VERTEXPROCESSING; }
	else { devbehaviorflags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING; }

	// If pure device and HW T&L supported
	if( caps.DevCaps & D3DDEVCAPS_PUREDEVICE && devbehaviorflags & D3DCREATE_HARDWARE_VERTEXPROCESSING) { devbehaviorflags |= D3DCREATE_PUREDEVICE; }

	// Step 4: Fill out the D3DPRESENT_PARAMETERS structure.
	m_d3dpp.BackBufferWidth            = 0;
	m_d3dpp.BackBufferHeight           = 0;
	m_d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;
	m_d3dpp.BackBufferCount            = 1;
	m_d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	m_d3dpp.MultiSampleQuality         = 0;
	m_d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.hDeviceWindow              = _window->m_hwnd;
	m_d3dpp.Windowed                   = true;
	m_d3dpp.EnableAutoDepthStencil     = true;
	m_d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	m_d3dpp.Flags                      = 0;
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	//    m_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_ONE;

	// Step 5: Create the device.
	app_throw_hr(m_d3dobject->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
		_window->m_hwnd,devbehaviorflags,
		&m_d3dpp,&m_d3ddevice));

	// checking shader version 2.1 or greater required
	app_zero( &caps , sizeof(D3DCAPS9) );
	app_throw_hr(m_d3ddevice->GetDeviceCaps(&caps));
	if( caps.VertexShaderVersion < D3DVS_VERSION(2, 0) ) { app_throw("dev caps"); }
	if( caps.PixelShaderVersion  < D3DPS_VERSION(2, 0) ) { app_throw("dev caps"); }


	D3DVERTEXELEMENT9 vertexelements_object[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexDeclaration(vertexelements_object, &m_object_vertex_declaration));

	D3DVERTEXELEMENT9 vertexelements_ui_foreground[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexDeclaration(vertexelements_ui_foreground, &m_ui_foreground_vertex_declaration));

	D3DVERTEXELEMENT9 vertexelements_ui_background[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};
	app_throw_hr(_api_manager->m_d3ddevice->CreateVertexDeclaration(vertexelements_ui_background, &m_ui_background_vertex_declaration));


	m_ui_bone_vertex_declaration = NULL;
    D3DVERTEXELEMENT9 vertexelements[] = {
        {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
        {0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
        D3DDECL_END()
    };
    app_throw_hr(_api_manager->m_d3ddevice->CreateVertexDeclaration(vertexelements, &m_ui_bone_vertex_declaration));


	return buildfx();
}

void d3d_manager::clear(){

	app_releasecom(m_d3dobject);
	app_releasecom(m_d3ddevice);
	app_releasecom(m_fx);
}

bool d3d_manager::reset(){
	app->onlostdevice();
	app_throw_hr(m_d3ddevice->Reset( &m_d3dpp) );
	app->onresetdevice();
	return true;
}

bool d3d_manager::buildfx() {

	ID3DXBuffer* errors = 0;

	HRESULT fx_result =D3DXCreateEffectFromFile(_api_manager->m_d3ddevice,"shader.fx",
		0, 0, D3DXSHADER_DEBUG, 0, &m_fx, &errors);
	if( errors )    { app_throw((char*)errors->GetBufferPointer()); }
	if( fx_result ) { app_throw("effect file"); }

	m_htech_object        = m_fx->GetTechniqueByName("object_tech");
	m_htech_ui_foreground = m_fx->GetTechniqueByName("ui_foreground_tech");
	m_htech_ui_background = m_fx->GetTechniqueByName("ui_background_tech");
	m_htech_bone      = m_fx->GetTechniqueByName("bone_tech");
	m_hmvp       = m_fx->GetParameterByName(0, "g_mvp");
	m_htex       = m_fx->GetParameterByName(0, "g_tex");
	m_hcolor     = m_fx->GetParameterByName(0, "g_color");
	m_hworld     = m_fx->GetParameterByName(0, "g_world");
    m_hbones     = m_fx->GetParameterByName(0, "g_bones");
	if( m_htech_object        ==(D3DXHANDLE)NULL ){ app_throw("technique handle"); }
	if( m_htech_ui_foreground ==(D3DXHANDLE)NULL ){ app_throw("technique handle"); }
	if( m_htech_ui_background ==(D3DXHANDLE)NULL ){ app_throw("technique handle"); }
	if( m_htech_bone          ==(D3DXHANDLE)NULL ){ app_throw("technique handle"); }

	if( m_hmvp  ==(D3DXHANDLE)NULL ){ app_throw("mvp handle"); }
	if( m_htex  ==(D3DXHANDLE)NULL ){ app_throw("texture handle"); }
	if( m_hcolor==(D3DXHANDLE)NULL ){ app_throw("color handle"); }
	if( m_hworld==(D3DXHANDLE)NULL ){ app_throw("world handle"); }
	if( m_hbones==(D3DXHANDLE)NULL ){ app_throw("bone handle"); }

	return true;
}
