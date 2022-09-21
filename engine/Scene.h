#pragma once
#include <vector>


#include "ImageSettings.h"
#include "IntersectionQuery.h"
#include "render/Material.h"
#include "objects/Skybox.h"
#include "objects/SphereObject.h"
#include "render/OpaqueInstances.h"
#include "win32/Window.h"


class PostProcess;
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
	Skybox skybox;
	DepthStencil depth_stencil;
	RenderBuffer hdr_buffer;

	std::shared_ptr<Shader> pointShadowShader;
	std::shared_ptr<Shader> spotShadowShader;

	void init_depth_stencil_buffer(uint32_t width, uint32_t height);
	void init_depth_stencil_state();
	void init_hdr_and_depth_buffer(uint32_t width, uint32_t height, uint32_t msaa);
	
	void render(RenderBuffer& target_buffer, PostProcess& post_process, const Camera& camera);
	void shadow_pass();


	void render_reset()
	{
		depth_stencil.reset();
		hdr_buffer.reset();
		skybox.reset();
	}
};

