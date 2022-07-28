#include "Skybox.h"

#include "render/TextureManager.h"
#include "render/ShaderManager.h"

void Skybox::draw()
{
	TextureManager& texture_manager = TextureManager::instance();
	ShaderManager& shader_manager = ShaderManager::instance();
	Direct3D& globals = Direct3D::instance();

	globals.context4->VSSetShader(shader_manager(shader.c_str()).vertexShader.Get(), nullptr, NULL);
	globals.context4->PSSetShader(shader_manager(shader.c_str()).pixelShader.Get(), nullptr, NULL);
	globals.context4->PSSetShaderResources(0, 1, texture_manager.get_cubemap(texture.c_str()).GetAddressOf());
	globals.context4->Draw(3, 0);
}
