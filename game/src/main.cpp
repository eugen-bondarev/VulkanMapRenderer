#include <iostream>

#include <engine/core/window/window.h>
#include <engine/core/entry.h>

class NaturaForge : public Engine::App
{
public:
	void Init() override
	{
		
	}

	void Update() override
	{

	}
};

Engine::App* Engine::GetApp()
{
	return new NaturaForge();
}

ENTRY()