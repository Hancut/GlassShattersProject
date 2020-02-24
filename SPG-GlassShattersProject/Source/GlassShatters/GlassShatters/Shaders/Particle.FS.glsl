#version 430

layout(location = 0) in vec2 texture_coord;
layout(location = 1) in vec3 world_position;
layout(location = 2) in vec3 world_normal;

uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;
uniform int mode;

uniform vec3 camera_position;

layout(location = 0) out vec4 out_color;

vec3 myReflect()
{
    // TODO - compute the reflection color value
	return reflect(world_position - camera_position , vec3(0, 0, 1));
}

vec3 myRefract(float refractive_index)
{
    // TODO - compute the refraction color value
	return refract(world_position - camera_position ,  world_normal, refractive_index);
}

void main()
{
	//vec3 color =  texture(texture_cubemap, myRefract(0.33));
	out_color = texture(texture_cubemap, myRefract(1.33));
	if (mode == 3)
		out_color = vec4(world_normal, 0.0f);
}