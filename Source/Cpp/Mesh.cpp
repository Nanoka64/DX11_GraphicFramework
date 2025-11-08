#include "pch.h"
#include "Mesh.h"

using namespace BASE_VERTEX;
using namespace Tool::UV;

MeshFactory::MeshFactory()
{
}

MeshFactory::~MeshFactory()
{
}

// ----------------------------------------------------------------------------------------------------------------------
//       * MeshFactory Class - Quadの作成- *
// 引数 1.マテリアル
// 		2.マテリアル数
// ----------------------------------------------------------------------------------------------------------------------
MeshInfo *MeshFactory::CreateQuadInfo(MATERIAL* materials, UINT matNum )
{
	MeshInfo *meshInfo = new MeshInfo();

	// 頂点数
	meshInfo->NumVertex = 4;

	// 頂点情報
	meshInfo->pVertices = new BASE_VERTEX::VERTEX[meshInfo->NumVertex]{
		// 座標                       // 法線                  // カラー                      // uv
		{ VEC3(-1.0f, 0.0f,  1.0f),  VEC3(0.0f, 1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f, 0.0f)}, // 8 左上
		{ VEC3( 1.0f, 0.0f,  1.0f),  VEC3(0.0f, 1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f, 0.0f)}, // 9 右上
		{ VEC3(-1.0f, 0.0f, -1.0f),  VEC3(0.0f, 1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f, 1.0f)}, // 10左下
		{ VEC3( 1.0f, 0.0f, -1.0f),  VEC3(0.0f, 1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f, 1.0f)}, // 11右下
	};

	// インデックス数
	meshInfo->NumIndex = 6;

	// インデックス情報
	meshInfo->pIndices = new WORD[meshInfo->NumIndex]{
		0,1,2,
		1,3,2
	};

	// マテリアル情報設定
	meshInfo->pMaterials = materials;
	meshInfo->NumMaterial = matNum;

	return meshInfo;
}


// ----------------------------------------------------------------------------------------------------------------------
//       * MeshFactory Class - Cubuの作成- *
// 引数 1.マテリアル
// 		2.マテリアル数
// ----------------------------------------------------------------------------------------------------------------------
MeshInfo* MeshFactory::CreateCubeInfo(MATERIAL* materials, UINT matNum)
{
	MeshInfo* meshInfo = new MeshInfo();

	// 頂点数
	meshInfo->NumVertex = 24;

	// 頂点情報
	meshInfo->pVertices = new BASE_VERTEX::VERTEX[meshInfo->NumVertex]{
		// 座標                       // 法線                  // カラー                      // uv
		// 頂点フォーマット
		// 正面 1
		{ VEC3(-1.0f,  1.0f, -1.0f),   VEC3(0.0f, 0.0f, -1.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f, 0.0f)}, // 0 左上
		{ VEC3( 1.0f,  1.0f, -1.0f),   VEC3(0.0f, 0.0f, -1.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f, 0.0f)},  // 1 右上
		{ VEC3(-1.0f, -1.0f, -1.0f),   VEC3(0.0f, 0.0f, -1.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f, 1.0f)}, // 2 左下
		{ VEC3( 1.0f, -1.0f, -1.0f),   VEC3(0.0f, 0.0f, -1.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f, 1.0f)},  // 3 右下

		// 天井ポリゴン  3                   
		{ VEC3(-1.0f,  1.0f,  1.0f),   VEC3(0.0f, 1.0f, 0.0f),  VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f, 0.0f) }, // 4 左上
		{ VEC3( 1.0f,  1.0f,  1.0f),   VEC3(0.0f, 1.0f, 0.0f),  VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f, 0.0f) }, // 5 右上
		{ VEC3(-1.0f,  1.0f, -1.0f),   VEC3(0.0f, 1.0f, 0.0f),  VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f, 1.0f) }, // 6 左下
		{ VEC3( 1.0f,  1.0f, -1.0f),   VEC3(0.0f, 1.0f, 0.0f),  VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f, 1.0f) }, // 7 右下

		// 地面ポリゴン   4                                                                      
		{ VEC3( 1.0f,  -1.0f,  1.0f),  VEC3(0.0f, -1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f) , VEC2(0.0f, 0.0f)}, // 8 左上
		{ VEC3(-1.0f,  -1.0f,  1.0f),  VEC3(0.0f, -1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f) , VEC2(1.0f, 0.0f)}, // 9 右上
		{ VEC3( 1.0f,  -1.0f, -1.0f),  VEC3(0.0f, -1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f) , VEC2(0.0f, 1.0f)}, // 10左下
		{ VEC3(-1.0f,  -1.0f, -1.0f),  VEC3(0.0f, -1.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f) , VEC2(1.0f, 1.0f)}, // 11右下

		// 裏ポリゴン 6
		{ VEC3( 1.0f,  1.0f,  1.0f),   VEC3(0.0f, 0.0f, 1.0f),   VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(0.0f, 0.0f)  }, // 12 左上
		{ VEC3(-1.0f,  1.0f,  1.0f),   VEC3(0.0f, 0.0f, 1.0f),   VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(1.0f, 0.0f)  }, // 13 右上
		{ VEC3( 1.0f, -1.0f,  1.0f),   VEC3(0.0f, 0.0f, 1.0f),   VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(0.0f, 1.0f)  }, // 14 左下
		{ VEC3(-1.0f, -1.0f,  1.0f),   VEC3(0.0f, 0.0f, 1.0f),   VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(1.0f, 1.0f)  }, // 15 右下

		// 右ポリゴン  2                                                                         
		{ VEC3(1.0f,   1.0f, -1.0f),  VEC3(1.0f, 0.0f, 0.0f),   VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(0.0f, 0.0f)  }, // 16 左上
		{ VEC3(1.0f,   1.0f,  1.0f),  VEC3(1.0f, 0.0f, 0.0f),   VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(1.0f, 0.0f)  }, // 17 右上
		{ VEC3(1.0f,  -1.0f, -1.0f),  VEC3(1.0f, 0.0f, 0.0f),   VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(0.0f, 1.0f)  }, // 18 左下
		{ VEC3(1.0f,  -1.0f,  1.0f),  VEC3(1.0f, 0.0f, 0.0f),   VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(1.0f, 1.0f)  }, // 19 右下

		// 左ポリゴン 5
		{ VEC3(-1.0f,   1.0f,  1.0f), VEC3(-1.0f, 0.0f, 0.0f),  VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(0.0f, 0.0f)  },  // 20 左上
		{ VEC3(-1.0f,   1.0f, -1.0f), VEC3(-1.0f, 0.0f, 0.0f),  VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(1.0f, 0.0f)  },  // 21 右上
		{ VEC3(-1.0f,  -1.0f,  1.0f), VEC3(-1.0f, 0.0f, 0.0f),  VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(0.0f, 1.0f)  },  // 22 左下
		{ VEC3(-1.0f,  -1.0f, -1.0f), VEC3(-1.0f, 0.0f, 0.0f),  VEC4(1.0f, 1.0f, 1.0f, 1.0f), VEC2(1.0f, 1.0f)  },  // 23 右下
	};

	// インデックス数
	meshInfo->NumIndex = 36;

	// インデックス情報
	meshInfo->pIndices = new WORD[meshInfo->NumIndex]{
		// 正面
		0,1,2,
		1,3,2,  // 時計回りなら順番は何でもいい

		// 天井
		4,5,6,
		5,7,6,

		// 地面
		8,9,10,
		9,11,10,

		// 後ろ
		12,13,14,
		13,15,14,

		// 右
		16,17,18,
		17,19,18,

		// 左
		20,21,22,
		21,23,22,
	};

	// マテリアル情報設定
	meshInfo->pMaterials = materials;
	meshInfo->NumMaterial = matNum;

	return meshInfo;
}