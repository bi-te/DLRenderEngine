#pragma once

#include "includes/win_def.h"

#include <wrl.h>
#include <cassert>
#include <d3d11_4.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler")

#include "includes/win_undef.h"

template <class T>
using comptr = Microsoft::WRL::ComPtr<T>;
