#pragma once

#include "Scene.h"
#include "render/Renderer.h"

class Engine
{
public:
	Camera camera;
	Scene scene;
	Renderer renderer;

	static Engine& instance()
	{
		static std::unique_ptr<Engine> engine_(new Engine);
		return  *engine_;
	}
private:
	Engine() = default;	
};