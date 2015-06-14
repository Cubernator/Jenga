#pragma once

#include "DirectX.h"
#include <string>

class Shader
{
private:
	ID3D11VertexShader *m_vertexShader;
	ID3D11PixelShader *m_pixelShader;
	ID3D10Blob *m_vByteCode, *m_pByteCode;

public:
	Shader(const std::wstring& vShaderPath, const std::wstring& pShaderPath);
	~Shader();

	ID3D11VertexShader * getVertexShader();
	ID3D11PixelShader * getPixelShader();

	ID3D10Blob * getVertexByteCode();
	ID3D10Blob * getPixelByteCode();
};

