#pragma once

namespace Engine
{
	class App
	{
	public:
		App() = default;

		virtual void Init() = 0;
		virtual void Update() = 0;
	};

	extern App* GetApp();
}