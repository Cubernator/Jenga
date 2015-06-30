#pragma once

#include "DirectX.h"
#include "VertexBufferBase.h"
#include <string>
#include <unordered_map>

class Shader
{
private:
	ID3D11VertexShader *m_vertexShader;
	ID3D11PixelShader *m_pixelShader;
	ID3D10Blob *m_vByteCode, *m_pByteCode;

	std::unordered_map<const D3D11_INPUT_ELEMENT_DESC*, ID3D11InputLayout*> m_inputLayouts;

public:
	Shader(const std::wstring& vShaderPath, const std::wstring& pShaderPath);
	Shader(const std::wstring& shaderName);
	~Shader();

	void apply() const;

	ID3D11InputLayout * generateInputLayout(VertexBufferBase * vb);

	ID3D11VertexShader * getVertexShader();
	ID3D11PixelShader * getPixelShader();

	ID3D10Blob * getVertexByteCode();
	ID3D10Blob * getPixelByteCode();
};

