#include <iostream>

#include <engine/engine.h>

using namespace Engine;

class NaturaForge : public App
{
public:
	void Init() override
	{
		frameManager = new Vk::FrameManager();

		glm::vec2 viewport_size = { Vk::Global::swapChain->GetExtent().width, Vk::Global::swapChain->GetExtent().height };
	}

	void Update() override
	{
		
	}

	void Shutdown() override
	{
		delete frameManager;
	}

	Vk::FrameManager* frameManager;
};

App* ::Engine::GetApp()
{
	return new NaturaForge();
}

ENTRY()