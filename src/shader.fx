
uniform float4 g_color;
uniform extern float4x4 g_mvp; 
uniform extern float4x4 g_world; 

uniform extern float4x4 g_bones[20];

uniform extern texture g_tex;
sampler tex_s = sampler_state {
     Texture = <g_tex>;

 };

struct OutputVS{  
     float4  pos : POSITION0; 
     float   s   : TEXCOORD0; 
     float2  tex : TEXCOORD1;  
}; 

OutputVS VertexShader_object( float3 pos : POSITION0, float3 normal    : NORMAL0, float2 tex : TEXCOORD0 ) {  
     OutputVS out_vs = (OutputVS)0; 
     out_vs.pos = mul(float4(pos, 1.0f), g_mvp); 
     out_vs.tex = tex; 
     out_vs.s   = max( dot(float3(1.0,-1.0f,0.0f), normalize( normal )  ), 0.0f); 
     return out_vs; 
}
float4 PixelShader_object(float s : TEXCOORD0, float2 tex : TEXCOORD1) : COLOR { return tex2D(tex_s, tex); } 
technique object_tech { 
     pass P0 
         { 
             vertexShader = compile vs_2_0 VertexShader_object(); 
             pixelShader  = compile ps_2_0 PixelShader_object(); 
			 AlphaBlendEnable = true;
             SrcBlend = SrcAlpha;
             DestBlend = InvSrcAlpha;
         }  
} 

OutputVS VertexShader_ui_foreground( float3 pos : POSITION0 , float2 tex : TEXCOORD0 ) {  
     OutputVS out_vs = (OutputVS)0; 
     out_vs.pos = mul(float4(pos, 1.0f), g_mvp); 
     out_vs.tex = tex; 
     return out_vs; 
}
float4 PixelShader_ui_foreground(float2 tex : TEXCOORD1) : COLOR { return tex2D(tex_s, tex); }
technique ui_foreground_tech { 
     pass P0 
         { 
             vertexShader = compile vs_2_0 VertexShader_ui_foreground(); 
             pixelShader  = compile ps_2_0 PixelShader_ui_foreground(); 
			 AlphaBlendEnable = true;
             SrcBlend = SrcAlpha;
             DestBlend = InvSrcAlpha;
         }  
}

OutputVS VertexShader_ui_background( float3 pos       : POSITION0 ) {  
     OutputVS out_vs = (OutputVS)0; 
     out_vs.pos = mul(float4(pos, 1.0f), g_mvp);
     return out_vs; 
}
float4 PixelShader_ui_background() : COLOR { return g_color; }
technique ui_background_tech { 
     pass P0 
         { 
             vertexShader = compile vs_2_0 VertexShader_ui_background(); 
             pixelShader  = compile ps_2_0 PixelShader_ui_background(); 
			 AlphaBlendEnable = true;
             SrcBlend = SrcAlpha;
             DestBlend = InvSrcAlpha;
         }
}

    OutputVS VertexShader_blend(
    float3 pos       : POSITION0,
    float3 normal    : NORMAL0,
    float2 tex       : TEXCOORD0,
    float2 boneindex : BLENDINDICES0,
    float2 weights   : BLENDWEIGHT0
    ) {
    OutputVS outVS = (OutputVS)0;
    float4 posL = weights.x * mul(float4(pos, 1.0f), g_bones[ int(boneindex.x) ]);
    posL       += weights.y * mul(float4(pos, 1.0f), g_bones[ int(boneindex.y) ]);
    posL.w = 1.0f;
    float4 normalL = weights.x * mul(float4(normal, 0.0f), g_bones[ int(boneindex.x) ]);
    normalL       += weights.y * mul(float4(normal, 0.0f), g_bones[ int(boneindex.y) ]);
    normalL.w = 0.0f;
    outVS.pos = mul(posL, g_mvp);
    outVS. s = max( dot(float3(1.0,-1.0f,0.0f), normalize( normalL.xyz )  ), 0.0f);
    return outVS;
	}
	float4 PixelShader_blend(float s : TEXCOORD0) : COLOR {
    return float4( s*float3(0.5f,0.5f,0.5f)  , 1.0f);
    } 



technique bone_tech { 
     pass P0 
         { 
             vertexShader = compile vs_2_0 VertexShader_blend(); 
             pixelShader  = compile ps_2_0 PixelShader_blend();
         }
}