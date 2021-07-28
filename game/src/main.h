#include <engine/engine.h>

#include "game/game.h"

using namespace Engine;

class NaturaForge : public App
{
public:
	void Init() override;
	void Update() override;
	void Shutdown() override;

private:
	std::shared_ptr<Game> game;
	Vk::FrameManager* frameManager;
};

App* ::Engine::GetApp()
{
	return new NaturaForge();
}