/*
	This is an example of a vertex shader
*/

#include "shaders.inc"

uniform float4x4 world_to_view;
uniform float4x4 view_to_screen;

////////////////////////////////////////////////////////////////////////////////////////
#if defined( EAE6320_PLATFORM_D3D )
////////////////////////////////////////////////////////////////////////////////////////

void main(

	// These values come from one of the sVertex that we filled the vertex buffer with in C code
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


// These values come from one of the sVertex that we filled the vertex buffer with in C code
layout( location = 0 ) in float3 i_position;
layout( location = 1 ) in float2 i_texcoords;
layout( location = 2 ) in float4 i_color;

layout( location = 0 ) out float4 o_color;
layout( location = 1 ) out float2 o_texcoords;

// Entry Point
//============

void main()

////////////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////////////

{
	// Calculate the position of this vertex on screen
	{
	    float4 position_view = Transform( float4( i_position, 1.0 ), world_to_view );
	    o_position = Transform( position_view, view_to_screen );
	}

	// Pass the input color and texture coordinates to the fragment shader unchanged:
	{
		o_color = i_color;
		o_texcoords = i_texcoords;
	}
}
