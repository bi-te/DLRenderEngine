#include "PostProcess.h"

#include "render/ShaderManager.h"
#include "render/Direct11/Direct3D.h"

void PostProcess::update_buffer(uint32_t msaa)
{
    PostProcessBuffer* pp_buffer = static_cast<PostProcessBuffer*>(postProcessBuffer.map().pData);
    pp_buffer->ev100 = ev100;
    pp_buffer->msaa = msaa;
    postProcessBuffer.unmap();
}

void PostProcess::resolve(RenderBuffer& hdrInput,
                          RenderBuffer& ldrOutput)
{
    Direct3D& direct = Direct3D::instance();

    update_buffer(hdrInput.msaa);

    ldrOutput.bind_rtv();
    if (hdrInput.msaa == 1) post_process_shader->bind();
    else post_process_shader_ms->bind();

    //post_process_shader->bind();

    direct.context4->PSSetConstantBuffers(1, 1, postProcessBuffer.address());
    direct.context4->PSSetShaderResources(0, 1, hdrInput.srv.GetAddressOf());
    direct.context4->Draw(3, 0);
}
