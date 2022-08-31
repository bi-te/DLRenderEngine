#pragma once

#include "win32/win_def.h"

#include <wrl.h>
#include <cstdint>
#include <cassert>
#include <d3d11_4.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler")

#include "win32/win_undef.h"

template <class T>
using comptr = Microsoft::WRL::ComPtr<T>;

const uint32_t DEFAULT_PADDING{ 16u };
