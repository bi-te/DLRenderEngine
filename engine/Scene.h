#pragma once
#include <vector>

#include "Camera.h"
#include "ImageSettings.h"
#include "IntersectionQuery.h"
#include "render/Material.h"
#include "objects/Skybox.h"
#include "objects/SphereObject.h"
#include "render/OpaqueInstances.h"
#include "win32/Window.h"


const uint16_t MAX_REFLECTION_DEPTH = 2;	
const float MAX_REFLECTIVE_ROUGHNESS = 0.1f;
const float MAX_PROCESS_DISTANCE = 500.f;

const vec3f AMBIENT{ 0.18f, 0.f, 0.72f };

struct DepthStencil
{
	comptr<ID3D11Texture2D> buffer;
	comptr<ID3D11DepthStencilView> view;
	comptr<ID3D11DepthStencilState> state;

	void reset()
	{
		buffer.Reset();
		view.Reset();
		state.Reset();
	}
};

class Scene
{
	enum ObjType { SPHERE, MESH, POINTLIGHT, SPOTLIGHT, NONE };

	struct objectRef
	{
		void* ptr;
		ObjType type = NONE;
	};

public:
	DepthStencil depth_stencil;
	Camera camera;
	Skybox skybox;

	OpaqueInstances opaqueInstances;

	bool select_object(const Ray& ray, float t_min, float t_max, IntersectionQuery& record);

	void init_depth_and_stencil_buffer(uint32_t width, uint32_t height);
	void init_depth_stencil_state();

	void draw(Window& window);
};

