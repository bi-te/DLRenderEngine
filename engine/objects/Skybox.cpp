#include "Skybox.h"

#include "render/ShaderManager.h"
#include "render/TextureManager.h"

void Skybox::load_reflection(LPCWSTR file)
{
	reflection.map = TextureManager::instance().add_cubemap(file);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	reflection.map->GetDesc(&srvdesc);
	reflection.mip_levels = srvdesc.TextureCube.MipLevels;
}

void Skybox::render()
{
	Direct3D& globals = Direct3D::instance();

	skyshader->bind();
	globals.context4->PSSetShaderResources(0, 1, texture.GetAddressOf());
	globals.context4->Draw(3, 0);
}
