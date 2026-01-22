#pragma once
#include "Object.h"

enum class DEBUG_MESHS_TYPE
{
    CUBE,
    SPHERE,
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:DebugMesh Class --- */
//
// 【?】デバッグ用のメッシュ
//      当たり判定の可視化など、ほんとにゲームとは関係のない切り離されたもの
//      今のところ特にまとめてメッシュを表示するなどをしていないので、
//      このメッシュを描画したいときは、各コンポーネントか何かの
//      Drawの場所で呼んでね
//      内部で座標等を持たないので、Drawメソッドを呼ぶ際はワールド行列もセットすること
//
// ***************************************************************************************
class DebugMesh : public Object
{
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
    UINT m_VertexNum;
    UINT m_IndexNum;
    std::unique_ptr<CB_TRANSFORM_SET> m_pCBTransformSet;		// 定数バッファ(ワールド行列用)
    DEBUG_MESHS_TYPE m_MeshType;

public:
    DebugMesh();
    ~DebugMesh();

    bool Setup(RendererEngine& renderer, DEBUG_MESHS_TYPE _type);
    void Draw(RendererEngine& renderer, const DirectX::XMMATRIX &world);

private:
    bool CreateTransformCBuffer(RendererEngine& renderer);
    bool CreateVertexIndexBuffer(RendererEngine& renderer, const VERTEX::VERTEX_Static* pVertices, UINT vNum, const WORD* pIndices, UINT iNum);
};

