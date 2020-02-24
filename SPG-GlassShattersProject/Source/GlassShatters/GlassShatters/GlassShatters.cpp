#include "GlassShatters.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include <stb/stb_image.h>

using namespace std;

struct Particle
{
	glm::vec4 position;
	glm::vec4 speed;
	glm::vec4 initialPos;
	glm::vec4 initialSpeed;
	glm::vec4 type;
	Particle() {};

	Particle(const glm::vec4 &pos, const glm::vec4 &speed)
	{
		SetInitial(pos, speed, glm::vec4(0));
	}

	void SetInitial(const glm::vec4 &pos, const glm::vec4 &speed, const glm::vec4 type)
	{
		position = pos;
		initialPos = pos;

		this->speed = speed;
		initialSpeed = speed;

		this->type = type;
	}
};

ParticleEffect<Particle> *particleEffect;
int modeDraw = 1;
bool startAnimation = false;
float radians = 0;
float distanceCube = 0;
float speedup = 1;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/World.cpp

GlassShatters::GlassShatters()
{
}

GlassShatters::~GlassShatters()
{
}


void GlassShatters::Init()
{
	auto camera = GetSceneCamera();
	camera->SetPositionAndRotation(glm::vec3(0, 2, 4), glm::quat(glm::vec3(-40 * TO_RADIANS, 0, 0)));
	camera->Update();

	// upload texture for cubemap
	std::string texturePath = RESOURCE_PATH::TEXTURES + "Cube/";
	std::string shaderPath = "Source/GlassShatters/GlassShatters/Shaders/";

	{
		Mesh* mesh = new Mesh("cube");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("plane");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Load textures
	{
		TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES, "random.jpg");
	}

	// Create a shader program for rendering to texture
	{
		Shader *shader = new Shader("ShaderNormal");
		shader->AddShader(shaderPath + "Simple.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Simple.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	LoadShader("Normal", false);
	LoadShader("Glass", false);
	LoadShader("Particle");

	cubeMapTextureID = UploadCubeMapTexture(
		texturePath + "posx.png",
		texturePath + "posy.png",
		texturePath + "posz.png",
		texturePath + "negx.png",
		texturePath + "negy.png",
		texturePath + "negz.png"
	);

	unsigned int nrParticles = 400;

	particleEffect = new ParticleEffect<Particle>();
	particleEffect->Generate(nrParticles, true);

	auto particleSSBO = particleEffect->GetParticleBuffer();
	Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

	int cubeSize = 20;
	int hSize = cubeSize / 2;
	int length = 20;
	int num = 0;

	for (unsigned int i = 0; i < length; i++)
	{
		for (unsigned int j = 0; j < length; j++)
		{
			glm::vec4 pos(1);
			glm::vec4 type(0);

			pos.x = i * 0.1;
			pos.y = j * 0.2;
			pos.z = 0;
			if (i % 2 == 0) {
				type[0] = 1;
			}
			type[1] = (rand() % 10);
			glm::vec4 speed(0);
			speed.z = -(rand() % 20) / 10.0f;

			data[num].SetInitial(pos, speed, type);
			num++;
		}
		
	}
	particleSSBO->SetBufferData(data);
	
}


void GlassShatters::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}


void GlassShatters::Update(float deltaTimeSeconds)
{
	ClearScreen();

	//pe tot ecranul
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	distanceCube += deltaTimeSeconds * speedup * 3;
	if (distanceCube > 10 || !startAnimation) {
		auto particleSSBO = particleEffect->GetParticleBuffer();
		Particle* data = const_cast<Particle*>(particleSSBO->GetBuffer());

		int cubeSize = 20;
		int hSize = cubeSize / 2;
		int length = 20;
		int num = 0;

		for (unsigned int i = 0; i < length; i++)
		{
			for (unsigned int j = 0; j < length; j++)
			{
				glm::vec4 pos(1);
				glm::vec4 type(0);

				pos.x = i * 0.1;
				pos.y = j * 0.2;
				pos.z = 0;
				if (i % 2 == 0) {
					type[0] = 1;
				}
				type[1] = (rand() % 10);
				glm::vec4 speed(0);
				//speed.x = (rand() % 20 - 10) / 10.0f;
				speed.z = -(rand() % 20) / 10.0f;
				if (i > length / 4 && i < 3 * length / 4 && j > length / 4 && j < 3 * length / 4) {
					speed.z = -(rand() % 20) / 5.0f;
				}
				else if (i > length / 6 && i < length / 4 && j > length / 6 && j < 3 * length / 6) {
					speed.z = -(rand() % 20) / 8.0f;
				}
				//speed.y = rand() % 2 + 2.0f;

				data[num].SetInitial(pos, speed, type);
				num++;
			}

		}
		particleSSBO->SetBufferData(data);
		distanceCube = 0;
		speedup = 1;
		startAnimation = false;
	}
	
	// draw full glass
	/*{
		auto camera = GetSceneCamera();
		auto cameraPosition = camera->transform->GetWorldPosition();
		Shader *shader = shaders["Glass"];
		shader->Use();

		int loc_camera = shader->GetUniformLocation("camera_position");
		glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		int loc_texture = shader->GetUniformLocation("texture_cubemap");
		glUniform1i(loc_texture, 0);


		glm::mat4 model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
		model = glm::rotate(model, RADIANS(90), glm::vec3(1, 0, 0));
		//glm::translate(model, glm::vec3(0, 0, 0));
		TextureManager::GetTexture("random.jpg")->BindToTextureUnit(GL_TEXTURE0);
		RenderMesh(meshes["plane"], shaders["Glass"], model);
	}*/
	// draw the box
	
	{
		auto camera = GetSceneCamera();
		auto cameraPosition = camera->transform->GetWorldPosition();
		
		glm::mat4 model = glm::mat4(1);
		
		model = glm::translate(model, glm::vec3(1, 2, 5 - distanceCube));
		TextureManager::GetTexture("random.jpg")->BindToTextureUnit(GL_TEXTURE0);
		RenderMesh(meshes["sphere"], shaders["Normal"], model);
	}
	

	// draw the cubemap
	auto camera = GetSceneCamera();
	{
		Shader *shader = shaders["ShaderNormal"];
		shader->Use();

		glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(30));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		int loc_texture = shader->GetUniformLocation("texture_cubemap");
		glUniform1i(loc_texture, 0);

		meshes["cube"]->Render();
	}

	
	glLineWidth(3);

	if (modeDraw == 2) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
		
	{
		auto shader = shaders["Particle"];
		if (shader->GetProgramID())
		{
			shader->Use();
			
			auto cameraPosition = camera->transform->GetWorldPosition();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
			int loc_texture = shader->GetUniformLocation("texture_cubemap");
			glUniform1i(loc_texture, 0);

			int loc_camera = shader->GetUniformLocation("camera_position");
			glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);

			radians += deltaTimeSeconds * 2;
			int loc_rad = shader->GetUniformLocation("rad");
			glUniform1f(loc_rad, radians);
			

			int loc_d = shader->GetUniformLocation("hit");
			if (distanceCube + 0.5 >= 5) {
				glUniform1i(loc_d, 1);
			}
			else {
				glUniform1i(loc_d, 0);
			}

			int loc_speed = shader->GetUniformLocation("speedup");
			glUniform1f(loc_speed, speedup);

			int loc_mode = shader->GetUniformLocation("mode");
			glUniform1i(loc_mode, modeDraw);

			particleEffect->Render(GetSceneCamera(), shader);
		}
	}
	if (modeDraw == 2) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
}

unsigned int GlassShatters::UploadCubeMapTexture(const std::string &posx, const std::string &posy, const std::string &posz, const std::string& negx, const std::string& negy, const std::string& negz)
{
	int width, height, chn;

	unsigned char* data_posx = stbi_load(posx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posy = stbi_load(posy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posz = stbi_load(posz.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negx = stbi_load(negx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negy = stbi_load(negy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negz = stbi_load(negz.c_str(), &width, &height, &chn, 0);

	// TODO - create OpenGL texture
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	// TODO - bind the texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// TODO - load texture information for each face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posz);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negz);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// free memory
	SAFE_FREE(data_posx);
	SAFE_FREE(data_posy);
	SAFE_FREE(data_posz);
	SAFE_FREE(data_negx);
	SAFE_FREE(data_negy);
	SAFE_FREE(data_negz);

	return textureID;
}

void GlassShatters::FrameEnd()
{
	//DrawCoordinatSystem();
}

void GlassShatters::LoadShader(std::string name, bool hasGeomtery)
{
	static std::string shaderPath = "Source/GlassShatters/GlassShatters/Shaders/";

	// Create a shader program for particle system
	{
		Shader *shader = new Shader(name.c_str());
		shader->AddShader((shaderPath + name + ".VS.glsl").c_str(), GL_VERTEX_SHADER);
		shader->AddShader((shaderPath + name + ".FS.glsl").c_str(), GL_FRAGMENT_SHADER);
		if (hasGeomtery)
		{
			shader->AddShader((shaderPath + name + ".GS.glsl").c_str(), GL_GEOMETRY_SHADER);
		}

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/SPG-Framework/blob/master/Source/Core/Window/InputController.h

void GlassShatters::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
};

void GlassShatters::OnKeyPress(int key, int mods)
{
	if (GLFW_KEY_0 == key) {
		startAnimation = true;
	}

	if (GLFW_KEY_9 == key) {
		startAnimation = false;
	}
	
	if (GLFW_KEY_2 == key) {
		modeDraw = 2;
	}

	if (GLFW_KEY_1 == key) {
		modeDraw = 1;
	}

	if (GLFW_KEY_UP == key) {
		speedup += 0.1;
	}

	if (GLFW_KEY_DOWN == key && speedup > 0.1) {
		speedup -= 0.1;
	}

	if (GLFW_KEY_3 == key) {
		modeDraw = 3;
	}
};

void GlassShatters::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void GlassShatters::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void GlassShatters::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void GlassShatters::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void GlassShatters::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void GlassShatters::OnWindowResize(int width, int height)
{
	// treat window resize event
}
