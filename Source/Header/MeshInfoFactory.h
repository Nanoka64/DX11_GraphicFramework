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

class MeshInfoFactory
{
public:
	MeshInfoFactory();
	~MeshInfoFactory();

	static MeshInfo *CreateTriangleInfo();
	static MeshInfo *CreateQuadInfo(MATERIAL *materials, UINT matNum);
	static MeshInfo *CreateCubeInfo(MATERIAL* materials, UINT matNum);
	static MeshInfo *CreateSphereInfo(MATERIAL* materials, UINT matNum);
	static MeshInfo *CreateCylinderInfo();
	static MeshInfo *CreateTorusInfo();

	static MeshInfo* CreateSpriteQuadInfo(float w, float h);
	static MeshInfo* CreateRTSpriteInfo(float w, float h);
};
