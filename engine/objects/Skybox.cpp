#include "Skybox.h"

#include "render/ShaderManager.h"
#include "render/TextureManager.h"

void Skybox::load_reflection(LPCWSTR file)
{
	reflection.map = TextureManager::instance().add_cubemap(file);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	reflection.map->srv->GetDesc(&srvdesc);
	reflection.mip_levels = srvdesc.TextureCube.MipLevels;
}

void Skybox::render()
{
	Direct3D& direct = Direct3D::instance();

	skyshader->bind();
	direct.context4->PSSetShaderResources(5, 1, texture->srv.GetAddressOf());
	direct.context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	direct.context4->Draw(3, 0);
	direct.context4->PSSetShaderResources(5, 1, &NULL_SRV);
}
