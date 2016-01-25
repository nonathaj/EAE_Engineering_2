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
			{ source = "EAE 6330/ceiling_mesh.mesh", target = "ceiling_mesh.mesh.bin" },
			{ source = "EAE 6330/cement_mesh.mesh", target = "cement_mesh.mesh.bin" },
			{ source = "EAE 6330/floor_mesh.mesh", target = "floor_mesh.mesh.bin" },
			{ source = "EAE 6330/lambert_objects_mesh.mesh", target = "lambert_objects_mesh.mesh.bin" },
			{ source = "EAE 6330/metal_mesh.mesh", target = "metal_mesh.mesh.bin" },
			{ source = "EAE 6330/railing_mesh.mesh", target = "railing_mesh.mesh.bin" },
			{ source = "EAE 6330/walls_mesh.mesh", target = "walls_mesh.mesh.bin" },
		}
	},
    {
        tool = "EffectBuilder.exe",
        files = 
        {
            { source = "debug.effect", target = "debug.effect.bin" },
            { source = "opaque.effect", target = "opaque.effect.bin" },
            { source = "transparent.effect", target = "transparent.effect.bin" },
        }
    },
	{
		tool = "ShaderBuilder.exe",
        dependencies = { "shaders.inc" },
		files = 
		{
			{ source = "debug_vertex.shader", target = "debug_vertex.shader.bin", arguments = "vertex" },
			{ source = "debug_fragment.shader", target = "debug_fragment.shader.bin", arguments = "fragment" },
            { source = "vertex.shader", target = "vertex.shader.bin", arguments = "vertex" },
			{ source = "opaque_fragment.shader", target = "opaque_fragment.shader.bin", arguments = "fragment" },
			{ source = "transparent_fragment.shader", target = "transparent_fragment.shader.bin", arguments = "fragment" },
		}
	},
    {
        tool = "MaterialBuilder.exe",
        files = 
        {
            { source = "EAE 6330/cement_wall.material", target = "cement_wall.material.bin" },
            { source = "EAE 6330/floor.material", target = "floor.material.bin" },
            { source = "EAE 6330/metal_brace.material", target = "metal_brace.material.bin" },
            { source = "EAE 6330/railing.material", target = "railing.material.bin" },
            { source = "EAE 6330/wall.material", target = "wall.material.bin" },
            { source = "EAE 6330/white.material", target = "white.material.bin" },
        }
    },
    {
        tool = "TextureBuilder.exe",
        files = 
        {
            { source = "EAE 6330/cement_wall_D.png", target = "cement_wall_D.DDS" },
            { source = "EAE 6330/floor_D.png", target = "floor_D.DDS" },
            { source = "EAE 6330/metal_brace_D.png", target = "metal_brace_D.DDS" },
            { source = "EAE 6330/railing_D.png", target = "railing_D.DDS" },
            { source = "EAE 6330/wall_D.png", target = "wall_D.DDS" },
            { source = "EAE 6330/white.png", target = "white.DDS" },
        }
    },
}
