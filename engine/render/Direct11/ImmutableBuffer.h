#pragma once

#include <cstdint>

#include "d3d.h"

class ImmutableBuffer
{
	comptr<ID3D11Buffer> buffer;
	D3D11_BIND_FLAG type;
	uint32_t byte_width;
public:
	ImmutableBuffer(D3D11_BIND_FLAG type): type(type) {}

	void write(void* data, uint32_t byte_width);

	uint32_t bytewidth() { return byte_width; }
	ID3D11Buffer* get() { return buffer.Get(); }
	ID3D11Buffer* const* address() { return buffer.GetAddressOf(); }

	void free() { buffer.Reset(); }
};

