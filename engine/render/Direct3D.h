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

	void clear()
    {
		context4->ClearState();
		context4->Flush();

		factory5.Reset();
		device5.Reset();
		context4.Reset();
		devdebug.Reset();
    }
};