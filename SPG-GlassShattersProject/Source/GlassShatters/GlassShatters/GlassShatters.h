#pragma once

#include <Component/SimpleScene.h>

class GlassShatters : public SimpleScene
{
	public:
		GlassShatters();
		~GlassShatters();

		void Init() override;

		unsigned int UploadCubeMapTexture(const std::string & posx, const std::string & posy, const std::string & posz, const std::string & negx, const std::string & negy, const std::string & negz);

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void LoadShader(std::string name, bool hasGeomtery = true);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;
		int cubeMapTextureID;
	
	protected:
		glm::mat4 modelMatrix;
		float translateX, translateY, translateZ;
		float scaleX, scaleY, scaleZ;
		float angularStepOX, angularStepOY, angularStepOZ;
		GLenum polygonMode;
		FrameBuffer *frameBuffer;
};
