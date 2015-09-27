return
{
	{
		tool = "GenericBuilder.exe",
		files = 
		{
			{ source = "vertex.shader", target = "vertex.shader" },
			{ source = "fragment.shader", target = "fragment.shader" },
		}
	},
	{
		tool = "MeshBuilder.exe",
		files = 
		{
			{ source = "square.mesh", target = "square.mesh" },
			{ source = "triangle.mesh", target = "triangle.mesh" },
		}
	},
}