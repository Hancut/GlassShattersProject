#version 430

layout(location = 0) in vec2 text_coord;
layout(location = 1) in vec3 world_position;
layout(location = 2) in vec3 world_normal;

uniform sampler2D u_texture_0;
uniform samplerCube texture_cubemap;
uniform vec3 camera_position;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = texture(texture_cubemap, refract(world_position - camera_position ,  world_normal , 1.33));
}
