#include "Scene.h"

#include <iostream>

#include "moving/SphereMover.h"
#include "moving/PointLightMover.h"
#include "moving/SpotlightMover.h"
#include "moving/TransformMover.h"
#include "render/light_render.h"
#include "RandomGenerator.h"

void Scene::select_object(const Ray& ray, float t_min, float t_max, IntersectionQuery& record)
{
	objectRef ref;
	record.intersection = Intersection::infinite();

	for(PointLightObject& plobject: point_lights)
		if (plobject.sphere.intersection(ray, t_min, t_max, record.intersection)){
			ref.type = POINTLIGHT;
			ref.ptr = &plobject;
		}

	for (SpotlightObject& slobject : spotlights)
		if (slobject.sphere.intersection(ray, t_min, t_max, record.intersection)){
			ref.type = SPOTLIGHT;
			ref.ptr = &slobject;
		}

	for (SphereObject& object : spheres)
		if (object.sphere.intersection(ray, t_min, t_max, record.intersection)){
			ref.type = SPHERE;
			ref.ptr = &object;
		}

	for (MeshInstance& mesh : meshes)
		if (mesh.intersection(ray, t_min, t_max, record.intersection)){
			ref.type = MESH;
			ref.ptr = &mesh;
		}

	switch (ref.type)
	{
	case SPHERE: record.mover = std::make_unique<SphereMover>(static_cast<SphereObject*>(ref.ptr)->sphere); break;
	case MESH: record.mover = std::make_unique<TransformMover>(static_cast<MeshInstance*>(ref.ptr)->transform); break;
	case POINTLIGHT: record.mover = std::make_unique<PointLightMover>(*static_cast<PointLightObject*>(ref.ptr)); break;
	case SPOTLIGHT: record.mover = std::make_unique<SpotlightMover>(*static_cast<SpotlightObject*>(ref.ptr)); break;
	case NONE: record.mover = nullptr;
	}
}

void Scene::init_buffers()
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.ByteWidth = meshes_data[0].vertices_data_.size() * sizeof(float);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subresdesc;
	ZeroMemory(&subresdesc, sizeof(subresdesc));
	subresdesc.pSysMem = meshes_data[0].vertices_data_.data();
	
	HRESULT result = Direct3D::globals().device5->CreateBuffer(&vertexBufferDesc, &subresdesc, &vertexBuffer);
	assert(SUCCEEDED(result) && "CreateBuffer");
}


