#include "GameObject.h"
#include "DefaultTransform.h"
#include "MeshRenderer.h"
#include "VertexBuffer.h"

#include "utility.h"
#include "ModelLoader.h"

class Background : public GameObject
{
private:
	std::unique_ptr<DefaultTransform> m_transform;
	std::unique_ptr<MeshVertexBuffer> m_vbuffer;
	std::unique_ptr<IndexBuffer> m_ibuffer;
	std::unique_ptr<MeshRenderer> m_renderer;
	std::unique_ptr<Texture2D> m_texture;
	ComPtr<ID3D11SamplerState> m_samplerState;

	TexSpecular m_material;

public:
	Background(Shader * s, const std::wstring& objFileName, const std::wstring& texFileName);
};