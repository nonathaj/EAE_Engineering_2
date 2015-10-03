// Header Files
//=============

#include "Graphics.h"

#include <cassert>

#include "Context.h"
#include "Vertex.h"
#include "Effect.h"
#include "Mesh.h"

namespace
{
	Lame::Context *context = nullptr;
	Lame::Effect *effect = nullptr;
	Lame::Mesh *squareMesh = nullptr;
	Lame::Mesh *triangleMesh = nullptr;
}

bool eae6320::Graphics::Initialize( const HWND i_renderingWindow )
{
	context = Lame::Context::Create(i_renderingWindow);
	if (!context)
		goto OnError;

	if (!(squareMesh = Lame::Mesh::Create(context, "data/square.mesh")))
	{
		MessageBox(i_renderingWindow, "Failed to load the square.mesh file.", "Mesh loading error", 0);
		goto OnError;
	}
	if (!(triangleMesh = Lame::Mesh::Create(context, "data/triangle.mesh")))
	{
		MessageBox(i_renderingWindow, "Failed to load the triangle.mesh file.", "Mesh loading error", 0);
		goto OnError;
	}

	if (!(effect = Lame::Effect::Create(context, "data/vertex.shader", "data/fragment.shader")))
	{
		MessageBox(i_renderingWindow, "Failed to load the shader files.", "Shader loading error", 0);
		goto OnError;
	}

	return true;

OnError:

	ShutDown();
	return false;
}

void eae6320::Graphics::Render()
{
	bool success = context->BeginFrame();
	assert(success);
	{
		//bind the effect
		success = effect->Bind(context);
		assert(success);

		//Draw the meshes
		success = squareMesh->Draw(context);
		assert(success);
		success = triangleMesh->Draw(context);
		assert(success);
	}
	success = context->EndFrame();
	assert(success);
}

bool eae6320::Graphics::ShutDown()
{
	if (context)
	{
		if (effect)
		{
			delete effect;
			effect = nullptr;
		}

		if (squareMesh)
		{
			Lame::Mesh::Destroy(squareMesh, context);
			squareMesh = nullptr;
		}

		if (triangleMesh)
		{
			Lame::Mesh::Destroy(triangleMesh, context);
			triangleMesh = nullptr;
		}
		delete context;
		context = nullptr;
	}

	return true;
}
