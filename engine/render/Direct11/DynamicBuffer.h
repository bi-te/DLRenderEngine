#pragma once
#include <cstdint>

#include "d3d.h"

template<D3D11_BIND_FLAG Type>
class DynamicBuffer
{

	comptr<ID3D11Device> device;
	comptr<ID3D11DeviceContext> context;
	comptr<ID3D11Buffer> buffer;
	uint32_t byte_size;
public:

	DynamicBuffer() = default;

	DynamicBuffer(const comptr<ID3D11Device>& device5, const comptr<ID3D11DeviceContext>& context4): 
		device(device5), context(context4)
	{
	}

	void set_device(const comptr<ID3D11Device>& new_device) { device = new_device; }
	void set_context(const comptr<ID3D11DeviceContext>& new_context) { context = new_context; }

	void allocate(uint32_t bytesize);
	void write(const void* data);
	void write(const void* data, uint32_t bytesize);

	D3D11_MAPPED_SUBRESOURCE& map();
	void unmap();

	ID3D11Buffer* get() { return buffer.Get(); }
	ID3D11Buffer* const* address() { return buffer.GetAddressOf(); }

	void free() { buffer.Reset(); }
};

template <D3D11_BIND_FLAG Type>
void DynamicBuffer<Type>::allocate(uint32_t bytesize)
{
	byte_size = bytesize;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = Type;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = bytesize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	device->CreateBuffer(&desc, nullptr, &buffer);
}

template <D3D11_BIND_FLAG Type>
void DynamicBuffer<Type>::write(const void* data)
{
	if (!buffer.Get())
		return;

	D3D11_MAPPED_SUBRESOURCE subresource{};

	context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	memcpy(subresource.pData, data, byte_size);
	context->Unmap(buffer.Get(), 0);
}

template <D3D11_BIND_FLAG Type>
void DynamicBuffer<Type>::write(const void* data, uint32_t bytesize)
{
	byte_size = bytesize;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = Type;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = bytesize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subdata{};
	subdata.pSysMem = data;
	device->CreateBuffer(&desc, &subdata, &buffer);
}

template <D3D11_BIND_FLAG Type>
D3D11_MAPPED_SUBRESOURCE& DynamicBuffer<Type>::map()
{
	D3D11_MAPPED_SUBRESOURCE subresource{};

	context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	return subresource;
}

template <D3D11_BIND_FLAG Type>
void DynamicBuffer<Type>::unmap() 
{
	context->Unmap(buffer.Get(), 0);
}