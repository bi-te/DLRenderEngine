#pragma once
#include <cstdint>

#include "Direct3D.h"

template<D3D11_BIND_FLAG Type>
class DynamicBuffer
{
	comptr<ID3D11Buffer> buffer;
	uint32_t byte_size;
public:
	DynamicBuffer(){}
	DynamicBuffer(uint32_t bytesize);
	DynamicBuffer(const void* data, uint32_t bytesize);

	void allocate(uint32_t bytesize);
	void write(const void* data);
	void write(const void* data, uint32_t bytesize);

	ID3D11Buffer* get() { return buffer.Get(); }
	ID3D11Buffer* const* address() { return buffer.GetAddressOf(); }

	void free() { buffer.Reset(); }
};

template <D3D11_BIND_FLAG Type>
DynamicBuffer<Type>::DynamicBuffer(uint32_t bytesize)
{
	byte_size = bytesize;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = Type;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = bytesize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Direct3D::globals().device5->CreateBuffer(&desc, nullptr, &buffer);
}

template <D3D11_BIND_FLAG Type>
DynamicBuffer<Type>::DynamicBuffer(const void* data, uint32_t bytesize)
{
	byte_size = bytesize;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = Type;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = bytesize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subdata{};
	subdata.pSysMem = data;
	Direct3D::globals().device5->CreateBuffer(&desc, &subdata, &buffer);
}

template <D3D11_BIND_FLAG Type>
void DynamicBuffer<Type>::allocate(uint32_t bytesize)
{
	byte_size = bytesize;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = Type;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = bytesize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Direct3D::globals().device5->CreateBuffer(&desc, nullptr, &buffer);
}

template <D3D11_BIND_FLAG Type>
void DynamicBuffer<Type>::write(const void* data)
{
	if (!buffer.Get())
		return;

	D3D11_MAPPED_SUBRESOURCE subresource{};

	Direct3D::globals().context4->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	memcpy(subresource.pData, data, byte_size);
	Direct3D::globals().context4->Unmap(buffer.Get(), 0);
}

template <D3D11_BIND_FLAG Type>
void DynamicBuffer<Type>::write(const void* data, uint32_t bytesize)
{
	if (bytesize != byte_size)
		allocate(bytesize);

	D3D11_MAPPED_SUBRESOURCE subresource{};

	Direct3D::globals().context4->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	memcpy(subresource.pData, data, byte_size);
	Direct3D::globals().context4->Unmap(buffer.Get(), 0);
}
