#pragma once

#include "Scene.h"

class Engine
{
	static Engine* s_engine;

public:
	Scene scene;
	Window* window;

	static void init();

	void render();

	static Engine& instance()
	{
		assert(s_engine && "Engine not initialized");
		return *s_engine;
	}
	static void reset();

private:
	Engine() = default;
	
	Engine(const Engine& other) = delete;
	Engine(Engine&& other) noexcept = delete;
	Engine& operator=(const Engine& other) = delete;
	Engine& operator=(Engine&& other) noexcept = delete;
	
};
