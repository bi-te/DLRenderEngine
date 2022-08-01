#pragma once
#include <cstdint>

#include "d3d.h"

template<D3D11_BIND_FLAG Type>
class DynamicBuffer
{
	comptr<ID3D11Buffer> buffer;
	uint32_t byte_size;
public:


	DynamicBuffer(){}
	DynamicBuffer(uint32_t bytesize, const comptr<ID3D11Device5>& device5);
	DynamicBuffer(uint32_t bytesize, const void* data, const comptr<ID3D11Device5>& device5);

	void allocate(uint32_t bytesize, const comptr<ID3D11Device5>& device5);
	void write(const void* data, const comptr<ID3D11DeviceContext4>& context4);
	void write(const void* data, uint32_t bytesize, const comptr<ID3D11Device5>& device5);

	ID3D11Buffer* get() { return buffer.Get(); }
	ID3D11Buffer* const* address() { return buffer.GetAddressOf(); }

	void free() { buffer.Reset(); }
};

template <D3D11_BIND_FLAG Type>
DynamicBuffer<Type>::DynamicBuffer(uint32_t bytesize, const comptr<ID3D11Device5>& device5)
{
	allocate(bytesize, device5);
}

template <D3D11_BIND_FLAG Type>
DynamicBuffer<Type>::DynamicBuffer(uint32_t bytesize, const void* data, const comptr<ID3D11Device5>& device5)
{
	write(data, bytesize, device5);
}

template <D3D11_BIND_FLAG Type>
void DynamicBuffer<Type>::allocate(uint32_t bytesize, const comptr<ID3D11Device5>& device5)
{
	byte_size = bytesize;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = Type;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = bytesize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	device5->CreateBuffer(&desc, nullptr, &buffer);
}

template <D3D11_BIND_FLAG Type>
void DynamicBuffer<Type>::write(const void* data, const comptr<ID3D11DeviceContext4>& context4)
{
	if (!buffer.Get())
		return;

	D3D11_MAPPED_SUBRESOURCE subresource{};

	context4->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	memcpy(subresource.pData, data, byte_size);
	context4->Unmap(buffer.Get(), 0);
}

template <D3D11_BIND_FLAG Type>
void DynamicBuffer<Type>::write(const void* data, uint32_t bytesize, const comptr<ID3D11Device5>& device5)
{
	byte_size = bytesize;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = Type;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = bytesize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subdata{};
	subdata.pSysMem = data;
	device5->CreateBuffer(&desc, &subdata, &buffer);
}
