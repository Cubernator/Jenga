#include "ModelLoader.h"

#include <fstream>
#include <sstream>

#include <boost/tokenizer.hpp>

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

bool loadMeshFromObjFile(const std::wstring& fileName, MeshVertexBuffer **vb, IndexBuffer **ib)
{
	std::vector<XMFLOAT3> positions, normals;
	std::vector<XMFLOAT2> uvs;

	std::vector<OBJFace> faces;

	std::ifstream file(fileName);

	if (!file) return false;

	boost::char_separator<char> wsep(" "), fsep("/");

	// load data

	std::string line;
	while (std::getline(file, line)) {
		tokenizer tokens(line, wsep);
		tokenizer::iterator it = tokens.begin();
		std::string s = *(it++);
		if (s.front() == 'v') {
			if (s.size() < 2) {
				XMFLOAT3 p;
				for (int i = 0; i < 3; ++i, ++it) {
					reinterpret_cast<float*>(&p)[i] = std::stof(*it);
				}
				positions.push_back(p);
			} else if (s.at(1) == 't') {
				XMFLOAT2 t;
				for (int i = 0; i < 2; ++i, ++it) {
					reinterpret_cast<float*>(&t)[i] = std::stof(*it);
				}
				uvs.push_back(t);
			} else if (s.at(1) == 'n') {
				XMFLOAT3 n;
				for (int i = 0; i < 3; ++i, ++it) {
					reinterpret_cast<float*>(&n)[i] = std::stof(*it);
				}
				normals.push_back(n);
			}
		} else if (s.front() == 'f') {
			OBJFace face;
			int i = 0;
			for (; it != tokens.end(); ++it) {
				OBJFaceVertex fv;

				tokenizer indices(*it, fsep);
				tokenizer::iterator fit = indices.begin();

				fv.pos = std::stoi(*(fit++)) - 1;
				fv.uv = std::stoi(*(fit++)) - 1;
				fv.normal = std::stoi(*(fit++)) - 1;

				face.vertices[i] = fv;
				++i;
			}
			faces.push_back(face);
		}
	}

	std::vector<MeshVertex> vertices;
	std::vector<UINT16> indices;

	// assemble mesh from data

	for (OBJFace face : faces) {
		for (OBJFaceVertex fvert : face.vertices) {
			MeshVertex vertex;
			vertex.pos = positions[fvert.pos];
			vertex.tex = uvs[fvert.uv];
			vertex.tex.y = 1.0f - vertex.tex.y; // invert v coordinate (it's upside down in blender for some reason...)
			vertex.normal = normals[fvert.normal];
			XMStoreFloat3(&vertex.tangent, XMVectorZero());

			std::vector<MeshVertex>::iterator l = std::find(vertices.begin(), vertices.end(), vertex);
			UINT16 i = std::distance(vertices.begin(), l);
			if (l == vertices.end()) {
				vertices.push_back(vertex);
			}

			indices.push_back(i);
		}
	}

	// calculate tangents

	float tcU1, tcV1, tcU2, tcV2;
	XMVECTOR tangent = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < faces.size(); ++i) {
		edge1 = XMLoadFloat3(&vertices[indices[i * 3]].pos) - XMLoadFloat3(&vertices[indices[i * 3 + 2]].pos);
		edge2 = XMLoadFloat3(&vertices[indices[i * 3 + 2]].pos) - XMLoadFloat3(&vertices[indices[i * 3 + 1]].pos);

		tcU1 = vertices[indices[(i * 3)]].tex.x - vertices[indices[(i * 3) + 2]].tex.x;
		tcV1 = vertices[indices[(i * 3)]].tex.y - vertices[indices[(i * 3) + 2]].tex.y;

		tcU2 = vertices[indices[(i * 3) + 2]].tex.x - vertices[indices[(i * 3) + 1]].tex.x;
		tcV2 = vertices[indices[(i * 3) + 2]].tex.y - vertices[indices[(i * 3) + 1]].tex.y;

		tangent = (tcV1 * edge1 - tcV2 * edge2) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));

		for (unsigned int j = 0; j < 3; ++j) {
			XMStoreFloat3(&vertices[indices[i * 3 + j]].tangent, tangent);
		}
	}

	// create buffers

	*vb = new MeshVertexBuffer(vertices.data(), vertices.size());
	*ib = new IndexBuffer(indices.data(), indices.size());

	return true;
}