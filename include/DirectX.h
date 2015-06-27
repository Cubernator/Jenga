#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>

#include <memory>

using namespace DirectX;

extern ID3D11Device *dev;                     // the pointer to our Direct3D device interface
extern ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
