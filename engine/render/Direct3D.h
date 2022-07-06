#pragma once

#include "../includes/d3d.h"

class Direct3D
{	
	Direct3D();

public:
	comptr<IDXGIFactory5> factory5;
	comptr<ID3D11Device5> device5;
	comptr<ID3D11DeviceContext4> context4;
	comptr<ID3D11Debug> devdebug;

    static Direct3D& globals()
    {
        static Direct3D graphics;
        return graphics;
    }

	~Direct3D()
	{
		devdebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);

		context4->ClearState();
		context4->Flush();
	}
};