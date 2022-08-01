#pragma once

#include <cstdint>

#include "d3d.h"

template<D3D11_BIND_FLAG Type>
class ImmutableBuffer
{
	comptr<ID3D11Buffer> buffer;
	uint32_t byte_width;
public:
	ImmutableBuffer(){}
	ImmutableBuffer(void* data, uint32_t byte_width, const comptr<ID3D11Device5>& device5);

	void write(void* data, uint32_t byte_width, const comptr<ID3D11Device5>& device5);

	uint32_t bytewidth() { return byte_width; }
	ID3D11Buffer* get() { return buffer.Get(); }
	ID3D11Buffer* const* address() { return buffer.GetAddressOf(); }

	void free() { buffer.Reset(); }
};

template <D3D11_BIND_FLAG Type>
ImmutableBuffer<Type>::ImmutableBuffer(void* data, uint32_t bytewidth, const comptr<ID3D11Device5>& device5)
{
	write(data, byte_width, device5);
}

template <D3D11_BIND_FLAG Type>
void ImmutableBuffer<Type>::write(void* data, uint32_t bytewidth, const comptr<ID3D11Device5>& device5)
{
	byte_width = bytewidth;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = Type;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = byte_width;

	D3D11_SUBRESOURCE_DATA subdata{};
	subdata.pSysMem = data;
	device5->CreateBuffer(&desc, &subdata, &buffer);
}
