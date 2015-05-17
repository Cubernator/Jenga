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

	//HRESULT hrv = D3DCompileFromFile(vShaderPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0, &m_vByteCode, NULL);
	//HRESULT hrp = D3DCompileFromFile(pShaderPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", flags, 0, &m_pByteCode, NULL);

	HRESULT hrv = D3DReadFileToBlob(vShaderPath.c_str(), &m_vByteCode);
	HRESULT hrp = D3DReadFileToBlob(pShaderPath.c_str(), &m_pByteCode);

	// encapsulate both shaders into shader objects
	dev->CreateVertexShader(m_vByteCode->GetBufferPointer(), m_vByteCode->GetBufferSize(), NULL, &m_vertexShader);
	dev->CreatePixelShader(m_pByteCode->GetBufferPointer(), m_pByteCode->GetBufferSize(), NULL, &m_pixelShader);
}


Shader::~Shader()
{
	m_vertexShader->Release();
	m_pixelShader->Release();
}