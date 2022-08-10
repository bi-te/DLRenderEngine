#include "ImmutableBuffer.h"
#include "Direct3D.h"

void ImmutableBuffer::write(void* data, uint32_t bytewidth)
{
	byte_width = bytewidth;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = type;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = byte_width;

	D3D11_SUBRESOURCE_DATA subdata{};
	subdata.pSysMem = data;
	Direct3D::instance().device5->CreateBuffer(&desc, &subdata, &buffer);
}