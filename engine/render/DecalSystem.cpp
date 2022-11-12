#include "DecalSystem.h"

#include "Direct11/Direct3D.h"
#include "moving/TransformSystem.h"

DecalSystem* DecalSystem::s_system = nullptr;

void DecalSystem::updateInstanceBuffer()
{
	uint32_t numCopied = 0;

	if (!decals.size()) return;

	instanceBuffer.allocate(decals.size() * sizeof(Decal));

	Decal* decalBuffer = static_cast<Decal*>(instanceBuffer.map().pData);

	for (Decal& decal : decals) {
		decalBuffer[numCopied].objectId = decal.objectId;
		decalBuffer[numCopied].size = decal.size;
		decalBuffer[numCopied].position = decal.position *
			TransformSystem::instance().transforms[decal.objectId].matrix().topLeftCorner<3, 3>() +
			TransformSystem::instance().transforms[decal.objectId].matrix().row(3).head<3>();
		decalBuffer[numCopied].decalToWorld = decal.decalToWorld;
		decalBuffer[numCopied].color = decal.color;
		decalBuffer[numCopied].rotation_angle = decal.rotation_angle;
		decalBuffer[numCopied].roughness = decal.roughness;
		numCopied++;
	}

	instanceBuffer.unmap();
}

void DecalSystem::render(comptr<ID3D11ShaderResourceView> depth,
	comptr<ID3D11ShaderResourceView>normals,
	comptr<ID3D11ShaderResourceView> ids)
{
	Direct3D& direct = Direct3D::instance();

	updateInstanceBuffer();
	decalShader->bind();

	uint32_t istride = sizeof(Decal), ioffset = 0;
	direct.context4->IASetVertexBuffers(0, 1, &NULL_BUFFER, &istride, &ioffset);
	direct.context4->IASetVertexBuffers(1, 1, instanceBuffer.address(), &istride, &ioffset);
	direct.context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	direct.context4->PSSetShaderResources(5, 1, decalTexture->srv.GetAddressOf());
	direct.context4->PSSetShaderResources(6, 1, depth.GetAddressOf());
	direct.context4->PSSetShaderResources(7, 1, normals.GetAddressOf());
	direct.context4->PSSetShaderResources(8, 1, ids.GetAddressOf());

	direct.context4->DrawInstanced(36, decals.size(), 0u, 0u);
}
