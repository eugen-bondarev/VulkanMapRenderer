#include <engine/engine.h>

#include "state/state.h"

using namespace Engine;

class NaturaForge : public App
{
public:
	void Init() override;
	void Update() override;
	void Shutdown() override;

private:
	std::shared_ptr<State> state;
	Vk::FrameManager* gameFrameManager;
	Vk::FrameManager* menuFrameManager;
};

App* ::Engine::GetApp()
{
	return new NaturaForge();
}