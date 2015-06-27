#include <WICTextureLoader.h>
#include "Texture.h"

#pragma comment(lib,"DirectXTK.lib")

Texture2D::Texture2D(const std::wstring& fileName, bool generateMipMaps)
{
	CreateWICTextureFromFile(dev, devcon, fileName.c_str(), (ID3D11Resource**)&m_texture, &m_resourceView);
	if (generateMipMaps) {
		devcon->GenerateMips(m_resourceView);
	}
}

Texture2D::~Texture2D()
{
	m_resourceView->Release();
	m_texture->Release();
}

ID3D11Texture2D * Texture2D::getTexture()
{
	return m_texture;
}

ID3D11ShaderResourceView* Texture2D::getResourceView()
{
	return m_resourceView;
}