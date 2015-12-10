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
			{ source = "asteroid.mesh", target = "asteroid.mesh.bin" },
			{ source = "bullet.mesh", target = "bullet.mesh.bin" },
			{ source = "box.mesh", target = "box.mesh.bin" },
			{ source = "bullet.mesh", target = "enemy1.mesh.bin" },
			{ source = "bullet.mesh", target = "enemy2.mesh.bin" },
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
            { source = "asteroid.material", target = "asteroid.material.bin" },
            { source = "bullet.material", target = "bullet.material.bin" },
            { source = "bullet.material", target = "enemy1.material.bin" },
            { source = "bullet.material", target = "enemy2.material.bin" },
        }
    },
    {
        tool = "TextureBuilder.exe",
        files = 
        {
            { source = "rock.JPG", target = "rock.DDS" },
            { source = "asteroid.jpg", target = "asteroid.DDS" },
            { source = "metal.jpg", target = "bullet.DDS" },
        }
    },
}
