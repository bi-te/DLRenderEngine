#include "Skybox.h"

#include "render/ShaderManager.h"

void Skybox::render()
{
	Direct3D& globals = Direct3D::instance();

	skyshader->bind();
	globals.context4->PSSetShaderResources(0, 1, texture.GetAddressOf());
	globals.context4->Draw(3, 0);
}
