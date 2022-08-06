#pragma once

#include "math/Mesh.h"
#include "math/Ray.h"
#include "math/Transform.h"
#include "math/Intersection.h"

#include "render/TextureManager.h"
#include "render/ShaderManager.h"
#include "render/Direct11/DynamicBuffer.h"
#include "render/Direct11/ImmutableBuffer.h"

struct RenderData
{
	std::wstring texture, shader;
	uint32_t material;

	ImmutableBuffer<D3D11_BIND_VERTEX_BUFFER> vertices;
	ImmutableBuffer<D3D11_BIND_INDEX_BUFFER> indices;
	DynamicBuffer<D3D11_BIND_CONSTANT_BUFFER> transformation;
};

class MeshInstance
{
public:
	Mesh* mesh;
	Transform transform;
	RenderData render_data;

	void update_transform_buffer()
	{
		transform.update();
		render_data.transformation.write(transform.mtransform.data(), Direct3D::instance().context4);
	}

	void load_buffers()
	{
		render_data.vertices.write(mesh->vertices.data(), mesh->vertices.size() * sizeof(float), Direct3D::instance().device5);
		render_data.indices.write(mesh->indices.data(), mesh->indices.size() * sizeof(uint32_t), Direct3D::instance().device5);
	}

	void reset_buffers()
	{
		render_data.vertices.free();
		render_data.indices.free();
		render_data.transformation.free();
	}

	void draw()
	{
		Direct3D& globals = Direct3D::instance();
		const Shader& shdr = ShaderManager::instance()(render_data.shader.c_str());

		uint32_t offset = 0;
		globals.context4->IASetVertexBuffers(0, 1, render_data.vertices.address(), &mesh->stride, &offset);
		globals.context4->IASetIndexBuffer(render_data.indices.get(), DXGI_FORMAT_R32_UINT, 0);
		globals.context4->IASetInputLayout(shdr.inputLayout.ptr.Get());
		globals.context4->VSSetShader(shdr.vertexShader.Get(), nullptr, NULL);
		globals.context4->VSSetConstantBuffers(1, 1, render_data.transformation.address());
		globals.context4->PSSetShader(shdr.pixelShader.Get(), nullptr, NULL);
		globals.context4->PSSetShaderResources(0, 1, TextureManager::instance().get_texture(render_data.texture.c_str()).GetAddressOf());

		globals.context4->DrawIndexed(mesh->indices.size(), 0, 0);
	}

	bool intersection(const Ray& ray, float t_min, float t_max, Intersection& nearest, uint32_t& material_index) const;
	bool intersection(const Ray& ray, float t_min, float t_max, Intersection& record) const;

};
