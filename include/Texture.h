#include "DirectX.h"

#include <string>

class Texture2D
{
private:
	ID3D11Texture2D * m_texture;
	ID3D11ShaderResourceView * m_resourceView;

public:
	Texture2D(const std::wstring& fileName, bool generateMipMaps = true);
	~Texture2D();

	ID3D11Texture2D* getTexture();
	ID3D11ShaderResourceView* getResourceView();
};