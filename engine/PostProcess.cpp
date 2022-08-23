#include "PostProcess.h"

#include "render/ShaderManager.h"
#include "render/Direct11/Direct3D.h"

void PostProcess::update()
{
    PostProcessBuffer* pp_buffer = static_cast<PostProcessBuffer*>(postProcessBuffer.map().pData);
    pp_buffer->ev100 = ev100;
    postProcessBuffer.unmap();
}

void PostProcess::resolve(RenderBuffer& hdrInput,
                          RenderBuffer& ldrOutput) const
{
    Direct3D& direct = Direct3D::instance();

    const Shader& post_shader = ShaderManager::instance()(post_process_shader.c_str());

    ldrOutput.bind_rtv();
    direct.context4->VSSetShader(post_shader.vertexShader.Get(), nullptr, NULL);
    direct.context4->PSSetShader(post_shader.pixelShader.Get(), nullptr, NULL);
    direct.context4->PSSetConstantBuffers(1, 1, postProcessBuffer.address());
    direct.context4->PSSetShaderResources(0, 1, hdrInput.srv.GetAddressOf());
    direct.context4->Draw(3, 0);
}
