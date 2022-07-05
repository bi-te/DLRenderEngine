#pragma once

#include "../includes/d3d.h"

class Direct3D
{	
	Direct3D();

public:

    static Direct3D& globals()
    {
        static Direct3D graphics;
        return graphics;
    }

	comptr<IDXGIFactory5> factory5;
	comptr<ID3D11Device5> device5;
	comptr<ID3D11DeviceContext4> context4;
	comptr<ID3D11Debug> devdebug;
};