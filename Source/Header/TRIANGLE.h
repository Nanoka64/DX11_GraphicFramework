#pragma once

//
/* ポリゴン表示用の頂点情報 */
//
struct VERTEX3
{
    static constexpr size_t VERTEX_NUM = 3; // 頂点数(三角形なので３)

    BASE_VERTEX::VERTEX vertex[VERTEX_NUM];              // 頂点情報配列
    ID3D11Buffer* vBuf;     // 頂点バッファ
    UINT CPUAccessFlags;    // CPUアクセスの方法
    D3D11_USAGE Usage;      // 使用方法

    float translate_X;
    float translate_Y;
    float angle_Z;
    float scale;

    /* コンストラクタ */
    VERTEX3(BASE_VERTEX::VEC3 centerPos, float size) :
        vBuf(nullptr), 
        vertex() , 
        CPUAccessFlags(D3D11_CPU_ACCESS_WRITE), 
        Usage(D3D11_USAGE_DYNAMIC),
        translate_X(0.f),
        translate_Y(0.f),
        scale(1.f),
        angle_Z(0.f)
    {
        BASE_VERTEX::VEC3 a, b, c;
        a = centerPos + (BASE_VERTEX::VEC3(0.0f, size, 0.0f));   // 上
        b = centerPos + (BASE_VERTEX::VEC3(size, -size, 0.0f));  // 右
        c = centerPos + (BASE_VERTEX::VEC3(-size, -size, 0.0f)); // 左

        // √3
        float sqrt3 = static_cast<float>(sqrt(3));
        a = { 0.0f,  sqrt3 / 3.0f, 0.0f };
        b = { 0.5f, -sqrt3 / 6.0f, 0.0f };
        c = { -0.5f,-sqrt3 / 6.0f, 0.0f };

        /* pos */
        vertex[0].pos = a;
        vertex[1].pos = b;
        vertex[2].pos = c;      

        /* normal */
        vertex[0].normal = a;
        vertex[1].normal = b;
        vertex[2].normal = c;
        
        /* uv */
        vertex[0].uv = BASE_VERTEX::VEC2(0.5f, 0.0f); // 上
        vertex[1].uv = BASE_VERTEX::VEC2(1.0f, 1.0f); // 右
        vertex[2].uv = BASE_VERTEX::VEC2(0.0f, 1.0f); // 左

        /* color */
        vertex[0].color = BASE_VERTEX::VEC4(1, 0, 0, 1); // 上
        vertex[1].color = BASE_VERTEX::VEC4(0, 1, 0, 1); // 右
        vertex[2].color = BASE_VERTEX::VEC4(0, 0, 1, 1); // 左
    };

    ~VERTEX3() {
        SAFE_RELEASE(vBuf)
    };

    void Draw(RendererManager& render);                 // 表示
    bool CreateVertexBuffer(RendererManager& render);   // 頂点バッファの作成
    void SetupTransform(RendererManager& render);       // 定数バッファに変換行列を書き込む
    void UpdateTransform();
};