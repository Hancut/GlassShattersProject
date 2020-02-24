#version 430
layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

layout(location = 0) in vec3 world_position[];
layout(location = 1) in vec3 world_normal[];

uniform mat4 View;
uniform mat4 Projection;
uniform vec3 eye_position;
uniform float rad;
uniform int hit;

in int t[];
in float radians[];

layout(location = 0) out vec2 texture_coord;
layout(location = 1) out vec3 w_position;
layout(location = 2) out vec3 w_normal;

vec3 vpos = gl_in[0].gl_Position.xyz;
vec3 forward = normalize(eye_position - vpos);
vec3 right = normalize(cross(forward, vec3(0, 1, 0)));
vec3 up = normalize(cross(forward, right));
vec3 normal = vec3(0, 0, 1);

float ds = 0.1 ;
float deep = 0.025;
float rad_init = radians[0];

mat4 Translate(float tx, float ty, float tz)
{
	return mat4( 
        	 1,  0, 0,  0,     
		 0,  1, 0,  0,     
		 0,  0, 1,  0,     
		tx, ty, tz, 1);     
 
}

mat4 RotateOx(float u)
{
	return mat4( 
        	 1,  0, 0,  0,     
		 0,  cos(u), -sin(u),  0,     
		 0,  sin(u), cos(u),  0,     
		 0, 0, 0, 1);     
 
}

mat4 RotateOy(float u)
{
	return mat4( 
        	 cos(u),  0, sin(u),  0,     
		 0,  1, 0,  0,     
		 -sin(u),  0, cos(u),  0,     
		 0, 0, 0, 1);     
 
}

void EmitPoint(vec3 offset)
{
	vec3 pos = vec3(1, 0, 0) * offset.x +  vec3(0, 1, 0) * offset.y + vec3(0, 0, 1) * offset.z + vpos;
	if (hit == 1) {
		gl_Position = Projection * View * Translate(vpos.x, vpos.y, vpos.z) * RotateOx(rad_init + rad) * Translate(-vpos.x, -vpos.y, -vpos.z) * vec4(pos, 1.0);
	} else {
		gl_Position = Projection * View * vec4(pos, 1.0);
	}
	w_position = pos;
	w_normal = normal;
	EmitVertex();
}

void main()
{
	vec3 p0 = vec3(-ds, -ds, deep);
	vec3 p1 = vec3(ds, -ds, deep);
	vec3 p2 = vec3(-ds, ds, deep);
	vec3 p3 = vec3(ds, -ds, -deep);
	vec3 p4 = vec3(-ds, ds, -deep);
	vec3 p5 = vec3(-ds, -ds, -deep);

	
	// Create a quad using triangle strip
	//
	//	3---------2
	//  | \       |
	//  |    \    |
	//  |       \ |		
	//  0---------1
	//
	// Triangles: (0, 1, 3), (1, 2, 3)
	int p = int(vpos.x * 10);

	if (int(t[0]) == 1) {
		texture_coord = vec2(0, 0);
		vec3 offset = p0;
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p1;
		EmitPoint(offset);

		texture_coord = vec2(0, 1);
		offset = p2;
		normal = normalize(cross(p1 - p0, p2 - p0));
		EmitPoint(offset);

		texture_coord = vec2(0, 0);
		offset = p4;
		normal = normalize(cross(p2 - p1, p4 - p1));		
		EmitPoint(offset);

		texture_coord = vec2(1, 1);
		offset = p5;
		normal = normalize(cross(p4 - p2, p5 - p2));		
		EmitPoint(offset);

		texture_coord = vec2(0, 1);
		offset = p3;
		normal = normalize(cross(p5 - p4, p3 - p4));
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p1;
		normal = normalize(cross(p3 - p5, p1 - p5));
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p4;
		normal = normalize(cross(p1 - p3, p4 - p3));
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p2;
		normal = normalize(cross(p4 - p1, p2 - p1));
		EmitPoint(offset);

		texture_coord = vec2(0, 1);
		offset = p5;
		normal = normalize(cross(p2 - p4, p5 - p4));
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p0;
		normal = normalize(cross(p5 - p2, p0 - p2));
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p1;
		normal = normalize(cross(p0 - p5, p1 - p5));
		EmitPoint(offset);
	} else {
		p0 = vec3(0, ds, deep);
		p1 = vec3(0, -ds, deep);
		p2 = vec3(-ds * 2, ds, deep);
		p3 = vec3(0, -ds, -deep);
		p4 = vec3(-ds * 2, ds, -deep);
		p5 = vec3(0, ds, -deep);

		texture_coord = vec2(0, 0);
		vec3 offset = p0;
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p1;
		EmitPoint(offset);

		texture_coord = vec2(0, 1);
		offset = p2;
		normal = normalize(cross(p1 - p0, p2 - p0));
		EmitPoint(offset);

		texture_coord = vec2(0, 0);
		offset = p4;
		normal = normalize(cross(p2 - p1, p4 - p1));
		EmitPoint(offset);

		texture_coord = vec2(1, 1);
		offset = p5;
		normal = normalize(cross(p4 - p2, p5 - p2));
		EmitPoint(offset);

		texture_coord = vec2(0, 1);
		offset = p3;
		normal = normalize(cross(p5 - p4, p3 - p4));
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p1;
		normal = normalize(cross(p3 - p5, p1 - p5));
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p4;
		normal = normalize(cross(p1 - p3, p4 - p3));
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p2;
		normal = normalize(cross(p4 - p1, p2 - p1));
		EmitPoint(offset);

		texture_coord = vec2(0, 1);
		offset = p5;
		normal = normalize(cross(p2 - p4, p5 - p4));
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p0;
		normal = normalize(cross(p5 - p2, p0 - p2));
		EmitPoint(offset);

		texture_coord = vec2(1, 0);
		offset = p1;
		normal = normalize(cross(p0 - p5, p1 - p5));
		EmitPoint(offset);
	}

	
}
