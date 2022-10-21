#pragma once
#include <vector>


#include "ImageSettings.h"
#include "IntersectionQuery.h"
#include "objects/GrassField.h"
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

class Scene
{
	enum ObjType { SPHERE, MESH, POINTLIGHT, SPOTLIGHT, NONE };

	struct objectRef
	{
		void* ptr;
		ObjType type = NONE;
	};

	struct DepthStencil
	{
		comptr<ID3D11ShaderResourceView> msaa_srv;
		comptr<ID3D11DepthStencilView> msaa_view;
		comptr<ID3D11ShaderResourceView> srv;
		comptr<ID3D11DepthStencilView> view;
		comptr<ID3D11DepthStencilState> state;
		comptr<ID3D11DepthStencilState> no_depth_state;

		void reset()
		{
			msaa_srv.Reset();
			msaa_view.Reset();
			srv.Reset();
			view.Reset();
			state.Reset();
			no_depth_state.Reset();
		}
	};

public:
	Skybox skybox;
	GrassField grassfield;
	DepthStencil depth_stencil;
	RenderBuffer hdr_buffer;

	void init_depth_stencil_buffer(uint32_t width, uint32_t height);
	void init_depth_stencil_state();
	void init_hdr_and_depth_buffer(uint32_t width, uint32_t height, uint32_t msaa);

	void bind_srvs();
	void render(RenderBuffer& target_buffer, PostProcess& post_process, const Camera& camera);
	void shadow_pass();


	void render_reset()
	{
		depth_stencil.reset();
		hdr_buffer.reset();
		skybox.reset();
		grassfield.reset();
	}
};

