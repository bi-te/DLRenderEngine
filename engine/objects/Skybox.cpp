#include "Skybox.h"

void Skybox::draw()
{
	TextureManager& texture_manager = TextureManager::instance();
	ShaderManager& shader_manager = ShaderManager::instance();
	Direct3D& globals = Direct3D::globals();

	globals.context4->VSSetShader(shader_manager[shader].vertexShader.Get(), nullptr, NULL);
	globals.context4->VSSetConstantBuffers(1, 1, frustrumBuffer.address());
	globals.context4->PSSetShader(shader_manager[shader].pixelShader.Get(), nullptr, NULL);
	globals.context4->PSSetShaderResources(0, 1, texture_manager[texture].srvTexture.GetAddressOf());
	globals.context4->Draw(3, 0);
}
