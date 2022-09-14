#pragma once
#include <cstdint>

#include "d3d.h"

class DynamicBuffer
{
	comptr<ID3D11Buffer> buffer;
	D3D11_BIND_FLAG type;
	uint32_t byte_size;
public:

	DynamicBuffer(D3D11_BIND_FLAG type): type(type){}

	void allocate(uint32_t bytesize);
	void write(const void* data);
	void write(const void* data, uint32_t bytesize);

	D3D11_MAPPED_SUBRESOURCE map();
	void unmap();

	ID3D11Buffer* get() { return buffer.Get(); }
	ID3D11Buffer* const* address() const { return buffer.GetAddressOf(); }

	void free() { buffer.Reset(); }
};
