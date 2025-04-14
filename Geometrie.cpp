#include "Geometrie.h"

#include "d3dUtils.h"

void Geometrie::CreateCube(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, float width, float height, float depth)
{
	
	MeshData meshData;

	VERTEX v[24] = {
		// FRONT (Red)
		{{ -.5f, -.5f, -.5f }, { 1, 0, 0, 1 }},
		{{ -.5f,  .5f, -.5f }, { 1, 0, 0, 1 }},
		{{  .5f,  .5f, -.5f }, { 1, 0, 0, 1 }},
		{{  .5f, -.5f, -.5f }, { 1, 0, 0, 1 }},

		// BACK (Green)
		{{ -.5f, -.5f,  .5f }, { 0, 1, 0, 1 }},
		{{  .5f, -.5f,  .5f }, { 0, 1, 0, 1 }},
		{{  .5f,  .5f,  .5f }, { 0, 1, 0, 1 }},
		{{ -.5f,  .5f,  .5f }, { 0, 1, 0, 1 }},

		// LEFT (Blue)
		{{ -.5f, -.5f,  .5f }, { 0, 0, 1, 1 }},
		{{ -.5f,  .5f,  .5f }, { 0, 0, 1, 1 }},
		{{ -.5f,  .5f, -.5f }, { 0, 0, 1, 1 }},
		{{ -.5f, -.5f, -.5f }, { 0, 0, 1, 1 }},

		// RIGHT (Yellow)
		{{  .5f, -.5f, -.5f }, { 1, 1, 0, 1 }},
		{{  .5f,  .5f, -.5f }, { 1, 1, 0, 1 }},
		{{  .5f,  .5f,  .5f }, { 1, 1, 0, 1 }},
		{{  .5f, -.5f,  .5f }, { 1, 1, 0, 1 }},

		// TOP (Cyan)
		{{ -.5f,  .5f, -.5f }, { 0, 1, 1, 1 }},
		{{ -.5f,  .5f,  .5f }, { 0, 1, 1, 1 }},
		{{  .5f,  .5f,  .5f }, { 0, 1, 1, 1 }},
		{{  .5f,  .5f, -.5f }, { 0, 1, 1, 1 }},

		// BOTTOM (Magenta)
		{{ -.5f, -.5f,  .5f }, { 1, 0, 1, 1 }},
		{{ -.5f, -.5f, -.5f }, { 1, 0, 1, 1 }},
		{{  .5f, -.5f, -.5f }, { 1, 0, 1, 1 }},
		{{  .5f, -.5f,  .5f }, { 1, 0, 1, 1 }},
	};

	meshData.Vertices.assign(&v[0], &v[24]);
 
	//
	// Create the indices.
	//
	uint32_t i[36] = {
		// FRONT
		0, 1, 2,
		0, 2, 3,

		// BACK
		4, 5, 6,
		4, 6, 7,

		// LEFT
		8, 9, 10,
		8, 10, 11,

		// RIGHT
		12, 13, 14,
		12, 14, 15,

		// TOP 
		16, 17, 18,
		16, 18, 19,

		// BOTTOM
		20, 21, 22,
		20, 22, 23 
	};

	meshData.Indices32.assign(&i[0], &i[36]);

	std::vector<VERTEX> vertex = meshData.Vertices;
	std::vector<uint16> index = meshData.GetIndices16();

	const UINT vbByteSize = (UINT)vertex.size() * sizeof(VERTEX);
	const UINT ibByteSize = (UINT)index.size() * sizeof(uint16);

	// Emplacement memoire CPU
	D3DCreateBlob(vbByteSize, &VertexBuffer);
	CopyMemory(VertexBuffer->GetBufferPointer(), vertex.data(), vbByteSize);

	D3DCreateBlob(ibByteSize, &IndexBuffer);
	CopyMemory(IndexBuffer->GetBufferPointer(), index.data(), ibByteSize);

	// Copy the triangle data to the vertex buffer.
	VertexBufferGPU = d3dUtils::CreateBuffer(device, commandList, vertex.data(), vbByteSize, VertexBufferUploader);
	VertexBufferGPU->SetName(L"VERTEX_BUFFER");
	
	// Copy the triangle data to the indices buffer.
	IndexBufferGPU = d3dUtils::CreateBuffer(device, commandList, index.data(), ibByteSize, IndexBufferUploader);
	IndexBufferGPU->SetName(L"INDEX_BUFFER");

	// Initialize the vertex buffer view.
	VertexByteStride = sizeof(VERTEX);
	VertexBufferByteSize = vbByteSize;

	// Initialize the indices buffer view.
	IndexFormat = DXGI_FORMAT_R16_UINT;
	IndexBufferByteSize = ibByteSize;

	IndicesCount = static_cast<UINT>(index.size());

	delete VertexBufferUploader;
	delete IndexBufferUploader;
	
	VertexBufferUploader = nullptr;
	IndexBufferUploader = nullptr;
	
}
