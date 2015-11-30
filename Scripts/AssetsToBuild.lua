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
			{ source = "white_floor.mesh", target = "white_floor.mesh.bin" },
			{ source = "white_triangle_prism.mesh", target = "white_triangle_prism.mesh.bin" },
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
    {
        tool = "MaterialBuilder.exe",
        files = 
        {
            { source = "red_opaque.material", target = "red_opaque.material.bin" },
            { source = "green_opaque.material", target = "green_opaque.material.bin" },
            { source = "green_transparent.material", target = "green_transparent.material.bin" },
            { source = "blue_transparent.material", target = "blue_transparent.material.bin" },
        }
    },
    {
        tool = "TextureBuilder.exe",
        files = 
        {
            { source = "rock.JPG", target = "rock.DDS" },
            { source = "leather.JPG", target = "leather.DDS" },
        }
    },
}
