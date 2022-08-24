#include "Skybox.h"

#include "render/TextureManager.h"
#include "render/ShaderManager.h"

void Skybox::render()
{
	TextureManager& texture_manager = TextureManager::instance();
	ShaderManager& shader_manager = ShaderManager::instance();
	Direct3D& globals = Direct3D::instance();

	const Shader& shader = shader_manager(skyshader.c_str());

	globals.context4->VSSetShader(shader.vertexShader.Get(), nullptr, NULL);
	globals.context4->PSSetShader(shader.pixelShader.Get(), nullptr, NULL);
	globals.context4->IASetInputLayout(shader.inputLayout.ptr.Get());
	globals.context4->PSSetShaderResources(0, 1, texture_manager.get_cubemap(texture.c_str()).GetAddressOf());
	globals.context4->Draw(3, 0);
}
