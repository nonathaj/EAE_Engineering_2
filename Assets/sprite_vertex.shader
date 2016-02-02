/*
	This is an example of a vertex shader
*/

#include "shaders.inc"

////////////////////////////////////////////////////////////////////////////////////////
#if defined( EAE6320_PLATFORM_D3D )
////////////////////////////////////////////////////////////////////////////////////////

void main(
	in const float3 i_position : POSITION,
	in const float4 i_color : COLOR,
	in const float2 i_texcoords : TEXCOORD0,

	out float4 o_position : POSITION,
	out float4 o_color : COLOR,
	out float2 o_texcoords : TEXCOORD0
	)
	
////////////////////////////////////////////////////////////////////////////////////////
#elif defined( EAE6320_PLATFORM_GL )
////////////////////////////////////////////////////////////////////////////////////////

#define o_position gl_Position

layout( location = 0 ) in float3 i_position;
layout( location = 1 ) in float2 i_texcoords;
layout( location = 2 ) in float4 i_color;

layout( location = 0 ) out float4 o_color;
layout( location = 1 ) out float2 o_texcoords;

void main()

////////////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////////////
{
	// Pass the input to the fragment shader unchanged:
	o_position = float4(i_position, 0);
	o_color = i_color;
	o_texcoords = i_texcoords;
}
