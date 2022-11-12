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
		comptr<ID3D11Texture2D> msaa_buffer;
		comptr<ID3D11Texture2D> buffer;
		comptr<ID3D11ShaderResourceView> msaa_srv;
		comptr<ID3D11DepthStencilView> msaa_view;
		comptr<ID3D11ShaderResourceView> srv;
		comptr<ID3D11DepthStencilView> view;

		comptr<ID3D11DepthStencilState> state;
		comptr<ID3D11DepthStencilState> read_only_state;
		comptr<ID3D11DepthStencilState> stencil_test_state;

		void reset()
		{
			msaa_buffer.Reset();
			msaa_srv.Reset();
			msaa_view.Reset();
			buffer.Reset();
			srv.Reset();
			view.Reset();
			state.Reset();
			read_only_state.Reset();
			stencil_test_state.Reset();
		}
	};

	struct GBuffer {
		comptr<ID3D11Texture2D> normalsTexture;
		comptr<ID3D11Texture2D> normalsTextureCopy;
		Texture normals;
		Texture normalsCopy;
		Texture albedo;
		Texture roughMetalTransmittance;
		Texture emission;
		Texture id;
		std::vector<ID3D11RenderTargetView*> rtvs;

		D3D11_VIEWPORT viewport;

		void clear()
		{
			Direct3D& direct = Direct3D::instance();
			float color[4]{ 0.f, 0.f, 0.f, 0.f };

			direct.context4->ClearRenderTargetView(emission.rtv.Get(), color);
			direct.context4->ClearRenderTargetView(normals.rtv.Get(), color);
			direct.context4->ClearRenderTargetView(albedo.rtv.Get(), color);
			direct.context4->ClearRenderTargetView(roughMetalTransmittance.rtv.Get(), color);
			direct.context4->ClearRenderTargetView(id.rtv.Get(), color);
		}

		void bind(comptr<ID3D11DepthStencilView> depth) {
			Direct3D& direct = Direct3D::instance();

			direct.context4->RSSetViewports(1, &viewport);
			direct.context4->OMSetRenderTargets(5, rtvs.data(), depth.Get());
		}

		void reset() {
			normals.reset();
			normalsCopy.reset();
			normalsTexture.Reset();
			normalsTextureCopy.Reset();
			albedo.reset();
			roughMetalTransmittance.reset();
			emission.reset();
			id.reset();
		}
	};

	struct DeferredBuffer {
		mat4f viewProjInv;
	};

public:

	Scene() {
		deferredBuffer.allocate(sizeof(DeferredBuffer));
	}

	Skybox skybox;
	GrassField grassfield;
	DepthStencil depth_stencil;
	RenderBuffer hdr_buffer;
	GBuffer g_buffer;

	std::shared_ptr<Shader> deferredResolveShader;
	std::shared_ptr<Shader> deferredResolvePointLightsShader;

	DynamicBuffer deferredBuffer{ D3D11_BIND_CONSTANT_BUFFER };

	void init_depth_stencil_buffer(uint32_t width, uint32_t height);
	void init_depth_stencil_states();
	void init_hdr_and_depth_buffer(uint32_t width, uint32_t height, uint32_t msaa);
	void init_gBuffer(uint32_t width, uint32_t height);

	void bind_srvs();
	void render(RenderBuffer& target_buffer, PostProcess& post_process, const Camera& camera);
	void shadow_pass();
	void resolve_gBuffer();

	void render_reset()
	{
		depth_stencil.reset();
		hdr_buffer.reset();
		skybox.reset();
		grassfield.reset();
		g_buffer.reset();
		deferredBuffer.free();
	}
};

