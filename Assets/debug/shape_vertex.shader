/*
	This is an example of a vertex shader
*/

#include "../shaders.inc"

uniform float4x4 local_to_world;
uniform float4x4 world_to_view;
uniform float4x4 view_to_screen;

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

// The locations assigned are arbitrary
// but must match the C calls to glVertexAttribPointer()

// These values come from one of the sVertex that we filled the vertex buffer with in C code
layout( location = 0 ) in float3 i_position;
layout( location = 1 ) in float2 i_texcoords;
layout( location = 2 ) in float4 i_color;

// Output
//=======

// The vertex shader must always output a position value,
// but unlike HLSL where the value is explicit
// GLSL has an implicit required variable called "gl_Position"

// Any other data is optional; the GPU doesn't know or care what it is,
// and will merely interpolate it across the triangle
// and give us the resulting interpolated value in a fragment shader.
// It is then up to us to use it however we want to.
// The locations are used to match the vertex shader outputs
// with the fragment shader inputs
// (note that Direct3D uses arbitrarily assignable "semantics").
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
	    float4 position_world = Transform( float4( i_position, 1.0 ), local_to_world );
	    float4 position_view = Transform( position_world, world_to_view );
	    o_position = Transform( position_view, view_to_screen );
	}

	// Pass the input color and texture coordinates to the fragment shader unchanged:
	{
		o_color = i_color;
		o_texcoords = i_texcoords;
	}
}
