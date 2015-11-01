return
{
	{
		tool = "GenericBuilder.exe",
		files = 
		{
            { source = "shaders.inc", target = "shaders.inc" },
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
    {
        tool = "EffectBuilder.exe",
        files = 
        {
            { source = "effect.effect", target = "effect.effect.bin" },
        }
    },
	{
		tool = "ShaderBuilder.exe",
        dependencies = { "shaders.inc" },
		files = 
		{
			{ source = "vertex.shader", target = "vertex.shader.bin", arguments = "vertex" },
			{ source = "fragment.shader", target = "fragment.shader.bin", arguments = "fragment" },
		}
	},
}
