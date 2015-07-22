#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <array>

typedef VertexPosTexNormTan MeshVertex;
typedef VertexBuffer<MeshVertex> MeshVertexBuffer;

struct OBJFaceVertex
{
	UINT16 pos, uv, normal;
};

struct OBJFace
{
	std::array<OBJFaceVertex, 3> vertices;
};

bool loadMeshFromObjFile(const std::wstring& fileName, MeshVertexBuffer **vb, IndexBuffer **ib);