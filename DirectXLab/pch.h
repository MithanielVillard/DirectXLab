#pragma once

// == Win Includes ==
#include <Windows.h>
#include <wrl.h>
#include <commctrl.h>
#include <comdef.h>

// == D3D12 Includes ==
#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_4.h>
#include <dxgidebug.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <DirectXColors.h>

#include "d3dx12.h"

// == STD Includes ==
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>

// == Comments ==
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Comctl32.lib")

// == Typedefs ==
#undef byte

using byte = unsigned __int8;

using ushort = unsigned __int16;
using uint = unsigned __int32;
using ulong = unsigned __int64;

// == Macros ==
#define NOMINMAX

#define PRINT_DEBUG(DebugMsg) { std::wostringstream os_; os_ << (DebugMsg) << "\n"; OutputDebugStringW(os_.str().c_str());}
#define PRINT_COM_ERROR(ErrorMsg, result) { _com_error err(result); LPCTSTR errMsg = err.ErrorMessage(); PRINT_DEBUG(ErrorMsg); PRINT_DEBUG(errMsg);}