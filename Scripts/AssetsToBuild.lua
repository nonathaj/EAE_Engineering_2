return
{
	{
		tool = "GenericBuilder.exe",
		files = 
		{
		}
	},
	{
		tool = "MeshBuilder.exe",
		files = 
		{
			{ source = "box.mesh", target = "box.mesh.bin" },
			{ source = "floor.mesh", target = "floor.mesh.bin" },
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
