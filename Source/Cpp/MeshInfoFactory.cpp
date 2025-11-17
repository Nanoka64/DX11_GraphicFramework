#include "pch.h"
#include "MeshInfoFactory.h"
#include "Window.h"

using namespace BASE_VERTEX;
using namespace Tool::UV;

MeshInfoFactory::MeshInfoFactory()
{
}

MeshInfoFactory::~MeshInfoFactory()
{
}

// ----------------------------------------------------------------------------------------------------------------------
//       * MeshFactory Class - 板ポリの作成- *
// 引数 1.マテリアル
// 		2.マテリアル数
// ----------------------------------------------------------------------------------------------------------------------
MeshInfo * MeshInfoFactory::CreateQuadInfo(MATERIAL* materials, UINT matNum )
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
//       * MeshFactory Class - キューブの作成- *
// 引数 1.マテリアル
// 		2.マテリアル数
// ----------------------------------------------------------------------------------------------------------------------
MeshInfo* MeshInfoFactory::CreateCubeInfo(MATERIAL* materials, UINT matNum)
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


// ----------------------------------------------------------------------------------------------------------------------
//       * MeshFactory Class - 球の作成- *
// 引数 1.マテリアル
// 		2.マテリアル数
// ----------------------------------------------------------------------------------------------------------------------
MeshInfo* MeshInfoFactory::CreateSphereInfo(MATERIAL* materials, UINT matNum)
{
	MeshInfo* meshInfo = new MeshInfo();

	// 分割数（増やすと滑らかになる）
	const UINT stackCount = 24; // 緯度分割
	const UINT sliceCount = 24; // 経度分割
	const float radius = 1.0f;

	// 頂点数
	meshInfo->NumVertex = (stackCount + 1) * (sliceCount + 1);
	meshInfo->pVertices = new BASE_VERTEX::VERTEX[meshInfo->NumVertex];

	// 頂点生成
	UINT vertexIndex = 0;
	for (UINT i = 0; i <= stackCount; ++i)
	{
		float phi = static_cast<float>(M_PI) * i / stackCount; // 緯度 [0, π]

		for (UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = (M_PI * 2) * j / sliceCount; // 経度 [0, 2π]

			float x = radius * sinf(phi) * cosf(theta);
			float y = radius * cosf(phi);
			float z = radius * sinf(phi) * sinf(theta);

			VEC3 pos = VEC3(x, y, z);
			VEC3 normal = VEC3::FromXMVECTOR(DirectX::XMVector3Normalize(pos)); // 単位化
			VEC4 color = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
			VEC2 uv = VEC2(
				(float)j / sliceCount,
				(float)i / stackCount
			);

			meshInfo->pVertices[vertexIndex++] = { pos, normal, color, uv };
		}
	}

	// インデックス数
	meshInfo->NumIndex = stackCount * sliceCount * 6;
	meshInfo->pIndices = new WORD[meshInfo->NumIndex];

	// インデックス生成
	UINT index = 0;
	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			UINT i0 = i * (sliceCount + 1) + j;
			UINT i1 = i0 + 1;
			UINT i2 = (i + 1) * (sliceCount + 1) + j;
			UINT i3 = i2 + 1;

			// 三角形2枚で四角形を構成
			meshInfo->pIndices[index++] = (WORD)i0;
			meshInfo->pIndices[index++] = (WORD)i2;
			meshInfo->pIndices[index++] = (WORD)i1;

			meshInfo->pIndices[index++] = (WORD)i1;
			meshInfo->pIndices[index++] = (WORD)i2;
			meshInfo->pIndices[index++] = (WORD)i3;
		}
	}

	// マテリアル設定
	meshInfo->pMaterials = materials;
	meshInfo->NumMaterial = matNum;

	return meshInfo;
}


// ----------------------------------------------------------------------------------------------------------------------
//       * MeshFactory Class - スプライト用クアッドの作成- *
// ----------------------------------------------------------------------------------------------------------------------
MeshInfo* MeshInfoFactory::CreateSpriteQuadInfo(float w, float h)
{
	MeshInfo* meshInfo = new MeshInfo();

	// 頂点数
	meshInfo->NumVertex = 4;

	// 中心位置
	VEC2 centerVec = VEC2(0.0f, 0.0f);
	float hw  = w * 0.5f;
	float hh  = h * 0.5f;

	// 頂点情報
	meshInfo->pVertices = new BASE_VERTEX::VERTEX[meshInfo->NumVertex]{
		// 座標												// 法線                  // カラー                      // uv
		{ VEC3(centerVec.x - hw, centerVec.y - hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f, 0.0f)}, // 8 左上
		{ VEC3(centerVec.x + hw, centerVec.y - hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f, 0.0f)}, // 9 右上
		{ VEC3(centerVec.x - hw, centerVec.y + hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f, 1.0f)}, // 10左下
		{ VEC3(centerVec.x + hw, centerVec.y + hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f, 1.0f)}, // 11右下
	};

	// インデックス数
	meshInfo->NumIndex = 6;

	// インデックス情報
	meshInfo->pIndices = new WORD[meshInfo->NumIndex]{
		0,1,2,
		1,3,2
	};

	// マテリアル情報設定
	meshInfo->pMaterials = nullptr;
	meshInfo->NumMaterial = 0;

	return meshInfo;
}


// ----------------------------------------------------------------------------------------------------------------------
//       * MeshFactory Class - レンダーターゲット用スプライトクアッドの作成- *
// ----------------------------------------------------------------------------------------------------------------------
MeshInfo* MeshInfoFactory::CreateRTSpriteInfo(float w, float h)
{
	MeshInfo* meshInfo = new MeshInfo();

	// 頂点数
	meshInfo->NumVertex = 4;

	// 中心位置
	VEC2 centerVec = VEC2(0.0f, 0.0f);
	float hw = w;
	float hh = h;


	/* テクスチャ座標では
	* 上：0.0
	* 下：1.0
	* だけど、 3D座標系では反対の為、レンダーターゲットにする場合はV座標を反転させる
	*/


	// 画面分割的なのができた
	
	//int bunkatu = 2;
	//// 頂点情報
	//meshInfo->pVertices = new BASE_VERTEX::VERTEX[meshInfo->NumVertex]{
	//	// 座標												// 法線                  // カラー                      // uv
	//	{ VEC3(centerVec.x - hw, centerVec.y - hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f - bunkatu + 1,	1.0f + bunkatu)}, // 8 左上
	//	{ VEC3(centerVec.x + hw, centerVec.y - hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f + bunkatu,		1.0f + bunkatu)}, // 9 右上
	//	{ VEC3(centerVec.x - hw, centerVec.y + hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f - bunkatu + 1,	0.0f - bunkatu + 1)}, // 10左下
	//	{ VEC3(centerVec.x + hw, centerVec.y + hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f + bunkatu,		0.0f - bunkatu + 1)}, // 11右下
	//};
		
	meshInfo->pVertices = new BASE_VERTEX::VERTEX[meshInfo->NumVertex]{
		// 座標												// 法線                  // カラー                      // uv
		{ VEC3(centerVec.x - hw, centerVec.y - hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f,1.0f)}, // 8 左上
		{ VEC3(centerVec.x + hw, centerVec.y - hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f,1.0f)}, // 9 右上
		{ VEC3(centerVec.x - hw, centerVec.y + hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(0.0f,0.0f)}, // 10左下
		{ VEC3(centerVec.x + hw, centerVec.y + hh,  0.0f), VEC3(0.0f, 0.0f, 0.0f), VEC4(1.0f, 1.0f, 1.0f, 1.0f),  VEC2(1.0f,0.0f)}, // 11右下
	};

	// インデックス数
	meshInfo->NumIndex = 6;

	// インデックス情報
	meshInfo->pIndices = new WORD[meshInfo->NumIndex]{
		0,2,1,
		1,2,3
	};

	// マテリアル情報設定
	meshInfo->pMaterials = nullptr;
	meshInfo->NumMaterial = 0;

	return meshInfo;
}
