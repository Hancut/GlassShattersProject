#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform int hit;
uniform float speedup;

out int t;
out float radians;

layout(location = 0) out vec3 world_position;
layout(location = 1) out vec3 world_normal;

struct Particle
{
	vec4 position;
	vec4 speed;
	vec4 iposition;
	vec4 ispeed;
	vec4 type;
};

layout(std430, binding = 0) buffer particles {
	Particle data[];
};

float rand(vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	vec3 pos = data[gl_VertexID].position.xyz;
	vec3 spd = data[gl_VertexID].speed.xyz;


	float dt;

	if (hit == 1) {
		dt = 0.01 * speedup;
	}
	pos = pos + spd * dt + vec3(0, -0.9, 0) * dt * dt/2.0f ;
	spd = spd + vec3(0, -0.9, 0) * dt;

	if(pos.y < (-40 + rand(pos.xy) * 20))
	{
		pos = data[gl_VertexID].iposition.xyz;
		spd = data[gl_VertexID].ispeed.xyz;
	}

	data[gl_VertexID].position.xyz =  pos;
	data[gl_VertexID].speed.xyz =  spd;

	t = int(data[gl_VertexID].type.x);
	radians = float(data[gl_VertexID].type.y);

	world_position = (Model * vec4(v_position, 1)).xyz;
    world_normal = normalize(mat3(Model) * v_normal);

	gl_Position = Model * vec4(pos, 1);
}


