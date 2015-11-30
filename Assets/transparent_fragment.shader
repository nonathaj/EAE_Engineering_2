
#include "shaders.inc"

uniform float3 color_value;
uniform float alpha_value;
uniform sampler2D base_texture;

////////////////////////////////////////////////////////////////////////////////////////
#if defined( EAE6320_PLATFORM_D3D )
////////////////////////////////////////////////////////////////////////////////////////

// Entry Point
//============

void main(

	// Input
	//======

	// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
	// will be interpolated across the triangle and given as input to the fragment shader

	in float4 i_color : COLOR0,
	in float2 i_texcoords : TEXCOORD0,

	// Output
	//=======

	// Whatever color value is output from the fragment shader
	// will determine the color of the corresponding pixel on the screen
	out float4 o_color : COLOR0

	)

////////////////////////////////////////////////////////////////////////////////////////
#elif defined( EAE6320_PLATFORM_GL )
////////////////////////////////////////////////////////////////////////////////////////

// Input
//======

// Whatever arbitrary data (i.e. everything excluding position) was output from the vertex shader
// will be interpolated across the triangle and given as input to the fragment shader

layout( location = 0 ) in float4 i_color;
layout( location = 1 ) in float2 i_texcoords;

// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out float4 o_color;

// Entry Point
//============

void main()

////////////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////////////
{
	o_color = SampleFromTexture(base_texture, i_texcoords);
	o_color *= i_color;
	o_color.rgb *= color_value;
	o_color.a *= alpha_value;
}
