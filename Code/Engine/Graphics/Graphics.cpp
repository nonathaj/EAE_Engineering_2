// Header Files
//=============

#include "Graphics.h"

#include <cassert>

#include "../../Engine/System/UserOutput.h"
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
	if (!(context = Lame::Context::Create(i_renderingWindow)) ||
		!(squareMesh = Lame::Mesh::Create(context, "data/square.mesh")) ||
		!(triangleMesh = Lame::Mesh::Create(context, "data/triangle.mesh")) ||
		!(effect = Lame::Effect::Create(context, "data/vertex.shader", "data/fragment.shader"))
		)
	{
		ShutDown();
		return false;
	}
	else
		return true;
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
