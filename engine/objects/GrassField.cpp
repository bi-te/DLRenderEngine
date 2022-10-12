#include "GrassField.h"

#include "render/LightSystem.h"

void GrassField::init_field(vec3f world_offset, float field_width, float field_height, float object_radius)
{
	width = field_width;
	height = field_height;
	world_position = world_offset;
	points = thinks::PoissonDiskSampling<float, 2>(
		object_radius, 
		{-field_width / 2.f, -field_height / 2.f},
		{field_width / 2.f, field_height / 2.f}
	);
}

void GrassField::update_instance_buffer()
{
	instanceBuffer.allocate(points.size() * sizeof(GrassInstanceBuffer));
	GrassInstanceBuffer* buffer = static_cast<GrassInstanceBuffer*>(instanceBuffer.map().pData);

	uint32_t num_copied = 0;

	for (auto point : points)
	{
		buffer[num_copied].rel_pos = {point[0] / width + 0.5f, point[1] / height + 0.5f};
		buffer[num_copied++].position = world_position + vec3f{point[0], 0.f, point[1]};
	}

	instanceBuffer.unmap();
}

void GrassField::render()
{
	Direct3D& direct = Direct3D::instance();

	update_instance_buffer();
	direct.context4->RSSetState(direct.two_face_rasterizer_state.Get());

	grassShader->bind();
	constantGrassBuffer.write(&grassBuffer, sizeof(GrassBuffer));
	uint32_t instance_stride = sizeof(GrassInstanceBuffer), ioffset = 0;
	direct.context4->IASetVertexBuffers(0, 1, &NULL_BUFFER, &instance_stride, &ioffset);
	direct.context4->IASetVertexBuffers(1, 1, instanceBuffer.address(), &instance_stride, &ioffset);
	direct.context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		
	direct.context4->VSSetConstantBuffers(1, 1, constantGrassBuffer.address());
	direct.context4->PSSetShaderResources(5, 1, baseColor.GetAddressOf());
	direct.context4->PSSetShaderResources(6, 1, normal.GetAddressOf());
	direct.context4->PSSetShaderResources(7, 1, roughness.GetAddressOf());
	direct.context4->PSSetShaderResources(8, 1, opacity.GetAddressOf());
	direct.context4->PSSetShaderResources(9, 1, translucency.GetAddressOf());
	direct.context4->PSSetShaderResources(10, 1, ambient_occlusion.GetAddressOf());

	direct.context4->DrawInstanced(6u * grassBuffer.planes * grassBuffer.sectors, points.size(), 0u, 0u);

	direct.context4->RSSetState(direct.rasterizer_state.Get());
}

void GrassField::shadow_render(uint32_t light_count)
{
	LightSystem& light_system = LightSystem::instance();
	Direct3D& direct = Direct3D::instance();

	update_instance_buffer();
	direct.context4->RSSetState(direct.two_face_rasterizer_state.Get());

	constantGrassBuffer.write(&grassBuffer, sizeof(GrassBuffer));
	uint32_t instance_stride = sizeof(GrassInstanceBuffer), ioffset = 0;
	direct.context4->IASetVertexBuffers(0, 1, &NULL_BUFFER, &instance_stride, &ioffset);
	direct.context4->IASetVertexBuffers(1, 1, instanceBuffer.address(), &instance_stride, &ioffset);
	direct.context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		
	direct.context4->VSSetConstantBuffers(1, 1, constantGrassBuffer.address());
	direct.context4->PSSetShaderResources(5, 1, opacity.GetAddressOf());

	for(uint32_t light = 0; light < light_count; light++)
	{
		light_system.bind_light_shadow_buffer(light);
		direct.context4->DrawInstanced(6u * grassBuffer.planes * grassBuffer.sectors, points.size(), 0u, 0u);
	}

	direct.context4->RSSetState(direct.rasterizer_state.Get());
}
