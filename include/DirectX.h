#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <d2d1_1.h>
#include <dwrite_2.h>
#include <wincodec.h>

#include <wrl.h>

#include <memory>

using namespace DirectX;
using namespace Microsoft::WRL;

extern ID3D11Device *dev;                     // the pointer to our Direct3D device interface
extern ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
