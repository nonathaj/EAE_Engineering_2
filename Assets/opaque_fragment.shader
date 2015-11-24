
#include "shaders.inc"

uniform float3 color_value;

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

layout( location = 0 ) in vec4 i_color;

// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

// Entry Point
//============

void main()

////////////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////////////
{
	o_color = i_color;
	o_color.rgb *= color_value;
}
