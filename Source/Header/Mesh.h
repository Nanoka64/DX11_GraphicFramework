#pragma once


/*
*  メッシュ情報
*/
struct MeshInfo
{
	BASE_VERTEX::VERTEX*pVertices;		// 頂点情報
	UINT NumVertex;						// 頂点数
	WORD *pIndices;						// インデックス情報
	UINT NumIndex;						// インデックス数
	MATERIAL* pMaterials;				// マテリアル
	UINT NumMaterial;					// マテリアル数
};

class MeshFactory
{
public:
	MeshFactory();
	~MeshFactory();

	static MeshInfo *CreateTriangleInfo();
	static MeshInfo *CreateQuadInfo(MATERIAL *materials, UINT matNum);
	static MeshInfo *CreateCubeInfo(MATERIAL* materials, UINT matNum);
	static MeshInfo *CreateSphereInfo();
	static MeshInfo *CreateCylinderInfo();
	static MeshInfo *CreateTorusInfo();
};
