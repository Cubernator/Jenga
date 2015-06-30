#include "Shader.h"
#include "Engine.h"

#include <d3dcompiler.h>

#pragma comment (lib, "D3DCompiler.lib")

Shader::Shader(const std::wstring& vShaderPath, const std::wstring& pShaderPath)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT hrv = D3DReadFileToBlob(vShaderPath.c_str(), &m_vByteCode);
	HRESULT hrp = D3DReadFileToBlob(pShaderPath.c_str(), &m_pByteCode);

	// encapsulate both shaders into shader objects
	dev->CreateVertexShader(m_vByteCode->GetBufferPointer(), m_vByteCode->GetBufferSize(), NULL, &m_vertexShader);
	dev->CreatePixelShader(m_pByteCode->GetBufferPointer(), m_pByteCode->GetBufferSize(), NULL, &m_pixelShader);
}

Shader::Shader(const std::wstring& shaderName) : Shader(shaderName + L"_vs.cso", shaderName + L"_ps.cso") { }

Shader::~Shader()
{
	for (auto& p : m_inputLayouts) {
		p.second->Release();
	}

	m_vertexShader->Release();
	m_pixelShader->Release();
}

ID3D11InputLayout * Shader::generateInputLayout(VertexBufferBase * vb)
{
	const D3D11_INPUT_ELEMENT_DESC* ied = vb->getVertexDesc();
	if (m_inputLayouts.count(ied)) {
		return m_inputLayouts.at(ied);
	} else {
		ID3D11InputLayout * newLayout;
		dev->CreateInputLayout(ied, vb->getNumVertexElements(), m_vByteCode->GetBufferPointer(), m_vByteCode->GetBufferSize(), &newLayout);
		m_inputLayouts[ied] = newLayout;
		return newLayout;
	}
}

void Shader::apply() const
{
	devcon->VSSetShader(m_vertexShader, NULL, 0);
	devcon->PSSetShader(m_pixelShader, NULL, 0);
}

ID3D11VertexShader * Shader::getVertexShader() { return m_vertexShader; }
ID3D11PixelShader * Shader::getPixelShader() { return m_pixelShader; }
ID3D10Blob * Shader::getVertexByteCode() { return m_vByteCode; }
ID3D10Blob * Shader::getPixelByteCode() { return m_pByteCode; }
