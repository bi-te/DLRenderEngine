#pragma once

#include "Screen.h"
#include "Scene.h"

class Engine
{
public:	
	Scene scene;
	Screen screen;

	static Engine& instance()
	{
		static std::unique_ptr<Engine> engine_(new Engine);
		return  *engine_;
	}
private:
	Engine() = default;	
};