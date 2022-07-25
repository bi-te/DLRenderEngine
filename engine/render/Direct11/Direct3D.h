#pragma once

#include "d3d.h"

class Direct3D
{
	static Direct3D* direct3d;
	Direct3D();

public:
	comptr<IDXGIFactory5> factory5;
	comptr<ID3D11Device5> device5;
	comptr<ID3D11DeviceContext4> context4;
	comptr<ID3D11Debug> devdebug;

	static void init()
	{
		if (direct3d) reset();

		direct3d = new Direct3D;
	}

    static Direct3D& globals()
    {
		assert(direct3d && "Direct3D not initialized");
        return *direct3d;
    }

	static void reset()
    {
		if (!direct3d) return;

		direct3d->context4->ClearState();
		direct3d->context4->Flush();

		direct3d->factory5.Reset();
		direct3d->device5.Reset();
		direct3d->context4.Reset();

		//direct3d->devdebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_SUMMARY | D3D11_RLDO_IGNORE_INTERNAL);
		direct3d->devdebug.Reset();

		delete direct3d;
    }
};