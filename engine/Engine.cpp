#include "Engine.h"
#include "render/TextureManager.h"
Engine* Engine::s_engine;

void Engine::init()
{
	if (s_engine) s_engine->reset();

	Direct3D::init();
	TextureManager::init();
	ShaderManager::init();

	s_engine = new Engine;
}

void Engine::render()
{
	PerFrame per_frame;
	per_frame.view_projection = scene.camera.view_proj;
	per_frame.frustum.bottom_left_point = scene.camera.blnear_fpoint - scene.camera.view_inv.row(3);
	per_frame.frustum.up_vector = scene.camera.frustrum_up;
	per_frame.frustum.right_vector = scene.camera.frustrum_right;
	Direct3D::instance().bind_globals(per_frame);

	scene.draw(*window);
}

void Engine::reset()
{
	assert(s_engine && "Engine not initialized");
	
	delete s_engine;

	ShaderManager::reset();
	TextureManager::reset();
	Direct3D::reset();
}
