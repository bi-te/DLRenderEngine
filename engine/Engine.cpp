#include "Engine.h"

#include "EngineClock.h"
#include "moving/TransformSystem.h"
#include "render/LightSystem.h"
#include "render/MaterialManager.h"
#include "render/MeshSystem.h"
#include "render/ModelManager.h"
#include "render/ShaderManager.h"
#include "render/TextureManager.h"
#include "render/ParticleSystem.h"
#include <render/DecalSystem.h>

Engine* Engine::s_engine;

void Engine::init()
{
	if (s_engine) s_engine->reset();

	Direct3D::init();
	TextureManager::init();
	ShaderManager::init();
	MaterialManager::init();
	TransformSystem::init();
	ModelManager::init();
	MeshSystem::init();
	LightSystem::init();
	ParticleSystem::init();
	EngineClock::init();
	RandomGenerator::init();
	DecalSystem::init();

	s_engine = new Engine;
}

void Engine::reset()
{
	assert(s_engine && "Engine not initialized");

	delete s_engine;

	DecalSystem::reset();
	RandomGenerator::reset();
	EngineClock::reset();
	ParticleSystem::reset();
	LightSystem::reset();
	MeshSystem::reset();
	ModelManager::reset();
	TransformSystem::reset();
	MaterialManager::reset();
	ShaderManager::reset();
	TextureManager::reset();
	Direct3D::reset();
}
