#include "DynamicBuffer.h"
#include "Direct3D.h"

void DynamicBuffer::allocate(uint32_t bytesize)
{
	byte_size = bytesize;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = type;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = bytesize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Direct3D::instance().device5->CreateBuffer(&desc, nullptr, &buffer);
}

void DynamicBuffer::write(const void* data)
{
	if (!buffer.Get())
		return;

	D3D11_MAPPED_SUBRESOURCE subresource{};

	Direct3D::instance().context4->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	memcpy(subresource.pData, data, byte_size);
	Direct3D::instance().context4->Unmap(buffer.Get(), 0);
}

void DynamicBuffer::write(const void* data, uint32_t bytesize)
{
	byte_size = bytesize;
	D3D11_BUFFER_DESC desc{};
	desc.BindFlags = type;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = bytesize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subdata{};
	subdata.pSysMem = data;
	Direct3D::instance().device5->CreateBuffer(&desc, &subdata, &buffer);
}

D3D11_MAPPED_SUBRESOURCE DynamicBuffer::map()
{
	D3D11_MAPPED_SUBRESOURCE subresource{};

	Direct3D::instance().context4->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	return subresource;
}

void DynamicBuffer::unmap()
{
	Direct3D::instance().context4->Unmap(buffer.Get(), 0);
}
