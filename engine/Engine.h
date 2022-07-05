#pragma once

#include "Scene.h"

class Engine
{
public:
	Camera camera;
	Scene scene;

	static Engine& instance()
	{
		static std::unique_ptr<Engine> engine_(new Engine);
		return  *engine_;
	}
private:
	Engine() = default;	
};