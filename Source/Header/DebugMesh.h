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

    VECTOR3::VEC3 m_Pos;
    VECTOR3::VEC3 m_Scale;

public:
    DebugMesh();
    ~DebugMesh();

    bool Setup(RendererEngine& renderer, DEBUG_MESHS_TYPE _type);
    void Draw(RendererEngine& renderer, const DirectX::XMMATRIX &world);

private:
    bool CreateTransformCBuffer(RendererEngine& renderer);
    bool CreateVertexIndexBuffer(RendererEngine& renderer, const VERTEX::VERTEX_Static* pVertices, UINT vNum, WORD* pIndices, UINT iNum);
};

