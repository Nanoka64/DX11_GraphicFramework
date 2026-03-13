#pragma once
// =======================================================================================
//
// *---        Helper         ---*
//		便利関数・構造体群
// =======================================================================================
namespace Tool
{
#define _USE_MATH_DEFINES
#include <math.h>	

    /* デグリーからラジアンに変換 */
    inline float DegToRad(float deg) { return deg * (static_cast<float>(M_PI) / 180.0f); }

    /* ラジアンからデグリーに変換 */
    inline float RadToDeg(float rad) { return rad * (180.0f / static_cast<float>(M_PI)); }

    //-----------------------------------------
    /*      乱数    */
    //-----------------------------------------

    /* 指定範囲の乱数取得（rand()なので精度は...ね） */
    inline float RandRange(float _min, float _max)
    {
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        return _min + r * (_max - _min);
    }

    //-----------------------------------------
    /*      シェイカー構造体    */
    //-----------------------------------------
    struct VEC2_Shaker
    {
    private:
        int currentFrame = 0;       // 現在のフレーム
        int duration = 0;           // 継続時間
        float strength = 0.0f;      // 揺れの強さ
        bool isShaking = false;
        VECTOR2::VEC2 offset = { 0.0f, 0.0f };

    public:
        // シェイク開始
        void Start(int duration, float strength)
        {
            this->duration = duration;
            this->strength = strength;
            currentFrame = 0;
            isShaking = true;
        }

        // 更新
        void Update()
        {
            if (!isShaking) return;

            // 新しい揺れオフセットを生成
            offset.x = (rand() % 200 - 100) / 100.0f * strength;
            offset.y = (rand() % 200 - 100) / 100.0f * strength;

            currentFrame++;
            if (currentFrame >= duration)
            {
                isShaking = false;
                offset = { 0.0f, 0.0f };
            }
        }

        // シェイクしたベクトルと引数のベクトルを足して返す
        VECTOR2::VEC2 Apply(const VECTOR2::VEC2& originalPos)const
        {
            return originalPos + offset;
        }
    };

    //-----------------------------------------
    /*      線形補間関数    */
    //-----------------------------------------
    inline float Lerp(float start, float end, float t)
    {
        return (1 - t) * start + t * end;
    }

    inline VECTOR2::VEC2 VEC2_Lerp(VECTOR2::VEC2 start, VECTOR2::VEC2 end, float t)
    {
        return {
            ((1 - t) * start.x + t * end.x),
            ((1 - t) * start.y + t * end.y)
        };
    }


    //-----------------------------------------
    /*      イージング関数    */
    //-----------------------------------------
    namespace Easing
    {
        // 軽い加速
        inline float InQuad(float _t) {
            return _t * _t;
        }

        // 強い加速
        inline float InCubic(float _t) {
            return _t * _t * _t;
        }

        // 自然な加速
        inline float InSine(float _t) {
            return 1.0f - cosf((_t * DirectX::XM_PI) / 2.0f);
        }

        // ヒュッ...シュタッって感じ
        // 2乗
        inline float EaseOutQuad(float _t){
            float invT = 1.0f - _t;
            return 1.0f - (invT * invT);
        }

        // 3乗
        inline float EaseOutCubic(float _t) {
            float invT = 1.0f - _t;
            return 1.0f - (invT * invT * invT);
        }

        // 4乗
        inline float EaseOutQuart(float _t){
            float invT = 1.0f - _t;
            return 1.0f - (invT * invT * invT * invT);
        }

        // 5乗
        inline float EaseOutQuint(float _t){
            float invT = 1.0f - _t;
            return 1.0f - (invT * invT * invT * invT * invT);
        }
    }

    //-----------------------------------------
    /*      UV    */
    //-----------------------------------------
    namespace UV
    {
        /* UV変換用構造体 */
        struct SpriteUV
        {
            float u;   // 左上X（U）
            float v;   // 左上Y（V）
            float uw;  // 幅
            float vh;  // 高さ
        };

        /* 主にスプライトのUV変換を行う際に使う */
        /* 引数：1.左上 X座標  2.左上 Y座標  3.幅  4.高さ  5.画像の横幅  6.画像の縦幅*/
        inline SpriteUV MakeSpriteUV(float spriteX, float spriteY, float spriteW, float spriteH, float texW, float texH)
        {
            SpriteUV uv{};
            uv.u = spriteX / texW;
            uv.v = spriteY / texH;
            uv.uw = spriteW / texW;
            uv.vh = spriteH / texH;
            return uv;
        }

        // スプライトの表示に必要な情報 
        struct SPRITE_DATA {
            ID3D11ShaderResourceView* texture;
            VECTOR2::VEC2 pos;
            float width;
            float height;
            SpriteUV uv;
        };
    }

    //--------------------------------------------------------------------------------------
    /*      当たり判定    */
    //--------------------------------------------------------------------------------------
    namespace Collision
    {
        // 衝突した相手の情報
        struct COLLISION_HIT_TARGET_DATA
        {
            VECTOR3::VEC3 Pos;
            bool hitLeft = false;
            bool hitRight = false;
            bool hitTop = false;
            bool hitBottom = false;
            bool Hit = false;
        };











        // bounding circle : 境界円
        inline bool HitCheckBC(VECTOR2::VEC2 src_pos, float src_r, VECTOR2::VEC2 dst_pos, float dst_r)
        {
            VECTOR2::VEC2 result;
            result = dst_pos - src_pos;

            float length = result.LengthSq();
            float r = src_r + dst_r;

            if ((r * r) > length)
            {
                return true;
            }

            return false;
        }

        // bounding box : 境界箱 AABB -> axis aligned bounding box 軸平行境界箱
        inline bool HitCheckBB(VECTOR2::VEC2 src_pos, float src_w, float src_h, VECTOR2::VEC2 dst_pos, float dst_w, float dst_h)
        {
            // 左側の判定
            if ((src_pos.x + (src_w / 2)) < (dst_pos.x - (dst_w / 2))) return false;

            // 右側の判定
            if ((src_pos.x - (src_w / 2)) > (dst_pos.x + (dst_w / 2))) return false;

            // 上側の判定
            if ((src_pos.y + (src_h / 2)) < (dst_pos.y - (dst_h / 2))) return false;

            // 下側の判定
            if ((src_pos.y - (src_h / 2)) > (dst_pos.y + (dst_h / 2))) return false;

            // 当たっている可能性しか残っていない
            return true;
        }

        inline COLLISION_HIT_TARGET_DATA HitCheckBB_RESULT_HIT_DATA(VECTOR2::VEC2 src_pos, float src_w, float src_h, VECTOR2::VEC2 dst_pos, float dst_w, float dst_h)
        {
            COLLISION_HIT_TARGET_DATA hit;

            // それぞれの辺の座標を計算
            float src_left = src_pos.x + src_w / 2;
            float src_right = src_pos.x - src_w / 2;
            float src_top = src_pos.y + src_h / 2;
            float src_bottom = src_pos.y - src_h / 2;

            float dst_left = dst_pos.x - dst_w / 2;
            float dst_right = dst_pos.x + dst_w / 2;
            float dst_top = dst_pos.y - dst_h / 2;
            float dst_bottom = dst_pos.y + dst_h / 2;

            // AABB判定（重なっていない場合は早期リターン）
            if (src_right > dst_left && src_left < dst_right &&
                src_top < dst_bottom && src_bottom > dst_top)
            {
                return hit; // ヒットしていない（全部false）
            }

            hit.Hit = true;

            float dx = (std::min)(src_right, dst_right) - (std::max)(src_left, dst_left);
            float dy = (std::min)(src_top, dst_top) - (std::max)(src_bottom, dst_bottom);

            if (dx < dy) {
                // 左 or 右の衝突
                if (src_pos.x < dst_pos.x)
                    hit.hitRight = true; // srcがdstの左にいる→右に当たった
                else
                    hit.hitLeft = true;
            }
            else {
                // 上 or 下の衝突
                if (src_pos.y < dst_pos.y)
                    hit.hitTop = true;
                else
                    hit.hitBottom = true;
            }

            // 当たっている可能性しか残っていない
            return hit;
        }
    }

    //************************************************************************
    //
    /* 文字変換関連の参考サイト */
    // https://iifx.dev/ja/articles/1421553/std-string%E3%81%A8std-wstring-c-%E6%96%87%E5%AD%97%E3%82%B3%E3%83%BC%E3%83%89%E3%81%AE%E9%81%B8%E3%81%B3%E6%96%B9%E3%81%A8%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E8%A7%A3%E6%B1%BA
    //
    //************************************************************************

    // stringからwstringへの変換
    inline std::wstring StringToWstring(const std::string& str)
    {
        std::wstring ret;
        //一度目の呼び出しは文字列数を知るため
        int len = MultiByteToWideChar(CP_UTF8,
            0,
            str.c_str(),//入力文字列
            static_cast<int>(str.length()),
            nullptr,
            0
        );
        assert(len >= 0);
        ret.resize(len);    //文字数分確保する

        //二度目の呼び出しは変換
        MultiByteToWideChar(CP_UTF8,
            0,
            str.c_str(),//入力文字列
            static_cast<int>(str.length()),
            ret.data(),
            static_cast<int>(ret.size())
        );
        return ret;
    }

    // wstringからstringへの変換
    inline std::string WStringToString(const std::wstring& wstr)
    {
        std::string ret;
        //一度目の呼び出しは文字列数を知るため
        int len = WideCharToMultiByte(
            CP_UTF8,
            0,
            wstr.c_str(),//入力文字列
            static_cast<int>(wstr.length()),
            nullptr,
            0,
            nullptr,
            nullptr
        );
        assert(len >= 0);
        ret.resize(len);    //文字数分確保する

        //二度目の呼び出しは変換
        WideCharToMultiByte(
            CP_UTF8,
            0,
            wstr.c_str(),//入力文字列
            static_cast<int>(wstr.length()),
            ret.data(),
            static_cast<int>(ret.size()),
            nullptr,
            nullptr
        );
        return ret;
    }


    /// <summary>
    /// U8文字列から通常の文字列へ変換
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    inline const char* U8ToChar(const char8_t* str)
    {
        return reinterpret_cast<const char*>(str);
    }

    /// <summary>
    /// 接線・副接線を求める
    /// </summary>
    inline void CalcTangentAndBitangent(VERTEX::VERTEX_Static_N *vertices, UINT vtxNum, const WORD* indices, UINT idxNum)
    {
        using VECTOR2::VEC2;
        using VECTOR3::VEC3;

        for (UINT i = 0; i < vtxNum; i++) {
            vertices[i].tangent = VEC3(0, 0, 0);
            vertices[i].bitangent = VEC3(0, 0, 0);
        }

        // 三角形ごとに計算し、加算していく
        for (UINT i = 0; i < idxNum; i += 3)
        {
            WORD i0 = indices[i];
            WORD i1 = indices[i + 1];
            WORD i2 = indices[i + 2];

            VEC3 E1 = vertices[i1].pos - vertices[i0].pos;
            VEC3 E2 = vertices[i2].pos - vertices[i0].pos;
            VEC2 Delta_UV1 = vertices[i1].uv - vertices[i0].uv;
            VEC2 Delta_UV2 = vertices[i2].uv - vertices[i0].uv;
            float f = 1.0f / (Delta_UV1.x * Delta_UV2.y - Delta_UV2.x * Delta_UV1.y);

            VEC3 tan;
            VEC3 biTan;

            // 接線
            tan.x = f * (Delta_UV2.y * E1.x - Delta_UV1.y * E2.x);
            tan.y = f * (Delta_UV2.y * E1.y - Delta_UV1.y * E2.y);
            tan.z = f * (Delta_UV2.y * E1.z - Delta_UV1.y * E2.z);
            tan = tan.Normalize();

            // 副接線
            biTan.x = f * (-Delta_UV2.x * E1.x + Delta_UV1.x * E2.x);
            biTan.y = f * (-Delta_UV2.x * E1.y + Delta_UV1.x * E2.y);
            biTan.z = f * (-Delta_UV2.x * E1.z + Delta_UV1.x * E2.z);
            biTan = biTan.Normalize();

            vertices[i0].tangent += tan;
            vertices[i0].bitangent += biTan;            
            vertices[i1].tangent += tan;
            vertices[i1].bitangent += biTan;            
            vertices[i2].tangent += tan;
            vertices[i2].bitangent += biTan;
        }

        // 全頂点正規化
        for (UINT i = 0; i < vtxNum; i++)
        {
            vertices[i].tangent = vertices[i].tangent.Normalize();
            vertices[i].bitangent = vertices[i].bitangent.Normalize();
        }
    }
}



//--------------------------------------------------------------------------------------
// File: ReadData.h
//
// Helper for loading binary data files from disk
//
// For Windows desktop apps, it looks for files in the same folder as the running EXE if
// it can't find them in the CWD
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------
 
// マイクロソフトのやつ https://learn.microsoft.com/ja-jp/windows/win32/direct3dhlsl/dx-graphics-hlsl-part1
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <system_error>
namespace DX
{
    inline std::vector<uint8_t> ReadData(_In_z_ const wchar_t *name)
    {
        std::ifstream inFile(name, std::ios::in | std::ios::binary | std::ios::ate);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
        if (!inFile)
        {
            wchar_t moduleName[_MAX_PATH] = {};
            if (!GetModuleFileNameW(nullptr, moduleName, _MAX_PATH))
                throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "GetModuleFileNameW");

            wchar_t drive[_MAX_DRIVE];
            wchar_t path[_MAX_PATH];

            if (_wsplitpath_s(moduleName, drive, _MAX_DRIVE, path, _MAX_PATH, nullptr, 0, nullptr, 0))
                throw std::runtime_error("_wsplitpath_s");

            wchar_t filename[_MAX_PATH];
            if (_wmakepath_s(filename, _MAX_PATH, drive, path, name, nullptr))
                throw std::runtime_error("_wmakepath_s");

            inFile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
        }
#endif

        if (!inFile)
            throw std::runtime_error("ReadData");

        const std::streampos len = inFile.tellg();
        if (!inFile)
            throw std::runtime_error("ReadData");

        std::vector<uint8_t> blob;
        blob.resize(size_t(len));

        inFile.seekg(0, std::ios::beg);
        if (!inFile)
            throw std::runtime_error("ReadData");

        inFile.read(reinterpret_cast<char *>(blob.data()), len);
        if (!inFile)
            throw std::runtime_error("ReadData");

        inFile.close();

        return blob;
    }
}











