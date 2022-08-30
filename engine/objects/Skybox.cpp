#include "Skybox.h"

#include "render/ShaderManager.h"

void Skybox::render()
{
	Direct3D& globals = Direct3D::instance();
	
	globals.context4->VSSetShader(skyshader->vertexShader.Get(), nullptr, NULL);
	globals.context4->PSSetShader(skyshader->pixelShader.Get(), nullptr, NULL);
	globals.context4->IASetInputLayout(skyshader->inputLayout.ptr.Get());
	globals.context4->PSSetShaderResources(0, 1, texture.GetAddressOf());
	globals.context4->Draw(3, 0);
}
