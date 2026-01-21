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
    std::unique_ptr<ID3D11Buffer> m_pVertexBuffer;
    std::unique_ptr<ID3D11Buffer> m_pIndexBuffer;
    UINT m_VertexNum;
    UINT m_IndexNum;

public:






};

