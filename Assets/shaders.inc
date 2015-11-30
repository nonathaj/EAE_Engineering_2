/*
	This file should be #included in every shader
	to set up platform-specific preprocessor directives
	so that the shader itself can be mostly platform-independent
*/

////////////////////////////////////////////////////////////////////////////////////////
#if defined( EAE6320_PLATFORM_D3D )
////////////////////////////////////////////////////////////////////////////////////////

#define Transform(i_vector, i_matrix) mul(i_vector, i_matrix)
#define SampleFromTexture(i_texture, i_texture_coordinates) tex2D(i_texture, i_texture_coordinates)

////////////////////////////////////////////////////////////////////////////////////////
#elif defined( EAE6320_PLATFORM_GL )
////////////////////////////////////////////////////////////////////////////////////////

// The version of GLSL to use must come first
#version 330

// This extension is required in order to specify explicit locations for shader inputs and outputs
#extension GL_ARB_separate_shader_objects : require

// Translate GLSL variable types to HLSL
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define float2x2 mat2
#define float3x3 mat3
#define float4x4 mat4

#define Transform(i_vector, i_matrix) i_vector * i_matrix
#define SampleFromTexture(i_texture, i_texture_coordinates) texture2D(i_texture, i_texture_coordinates)

////////////////////////////////////////////////////////////////////////////////////////
#else
////////////////////////////////////////////////////////////////////////////////////////

#error No platform defined for shader file

#error Missing define for Transform(i_vector, i_matrix)
#error Missing define for SampleFromTexture(i_texture, i_texture_coordinates)

////////////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////////////