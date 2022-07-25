#include "Engine.h"

Engine* Engine::s_engine;

void Engine::init(HWND handle)
{
	if (s_engine) s_engine->reset();

	Direct3D::init();
	TextureManager::init();
	ShaderManager::init();

	s_engine = new Engine;
	s_engine->renderer.init(handle);
}

void Engine::reset()
{
	assert(s_engine && "Engine not initialized");

	s_engine->renderer.reset();
	delete s_engine;

	ShaderManager::reset();
	TextureManager::reset();
	Direct3D::reset();
}
