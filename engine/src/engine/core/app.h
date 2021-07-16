#pragma once

namespace Engine
{
	class App
	{
	public:
		App() = default;
		virtual ~App() {}

		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
	};

	extern App* GetApp();
}