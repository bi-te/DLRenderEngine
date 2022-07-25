#pragma once

#include "Scene.h"
#include "render/Renderer.h"
#include "render/TextureManager.h"

class Engine
{
	static Engine* s_engine;

public:
	Camera camera;
	Scene scene;
	Renderer renderer;

	static void init(HWND handle);

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
