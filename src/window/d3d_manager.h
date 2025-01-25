#pragma once

#include "appdefs.h"

#include <d3d9.h>
#include <d3dx9.h>

struct d3d_manager{

	d3d_manager();
	bool init();
	void clear();

	bool reset();
	bool buildfx();

	ID3DXEffect* m_fx;

	D3DPRESENT_PARAMETERS m_d3dpp;

	IDirect3D9*           m_d3dobject;
	IDirect3DDevice9*     m_d3ddevice;

	IDirect3DVertexDeclaration9* m_object_vertex_declaration;
	IDirect3DVertexDeclaration9* m_ui_foreground_vertex_declaration;
	IDirect3DVertexDeclaration9* m_ui_background_vertex_declaration;

	IDirect3DVertexDeclaration9* m_ui_bone_vertex_declaration;

	D3DXHANDLE   m_hmvp;
	D3DXHANDLE   m_htex;
	D3DXHANDLE   m_hworld;
    D3DXHANDLE   m_hcolor;
	D3DXHANDLE   m_htech_object;
	D3DXHANDLE   m_htech_ui_foreground;
	D3DXHANDLE   m_htech_ui_background;

    D3DXHANDLE   m_htech_bone;
    D3DXHANDLE   m_hbones;


	static d3d_manager* _manager;

};

