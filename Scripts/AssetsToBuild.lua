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
			{ source = "color_floor.mesh", target = "color_floor.mesh.bin" },
			{ source = "triangle_prism.mesh", target = "triangle_prism.mesh.bin" },
		}
	},
    {
        tool = "EffectBuilder.exe",
        files = 
        {
            { source = "opaque.effect", target = "opaque.effect.bin" },
            { source = "transparent.effect", target = "transparent.effect.bin" },
        }
    },
	{
		tool = "ShaderBuilder.exe",
        dependencies = { "shaders.inc" },
		files = 
		{
			{ source = "vertex.shader", target = "vertex.shader.bin", arguments = "vertex" },
			{ source = "opaque_fragment.shader", target = "opaque_fragment.shader.bin", arguments = "fragment" },
			{ source = "transparent_fragment.shader", target = "transparent_fragment.shader.bin", arguments = "fragment" },
		}
	},
}
