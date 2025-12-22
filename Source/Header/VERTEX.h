#pragma once
#include "VEC.h"

//
/* 基本となる頂点情報 */
//
namespace VERTEX
{
    using namespace VECTOR2;
    using namespace VECTOR3;
    using namespace VECTOR4;

    /// <summary>
    /// 頂点座標とuvのみ
    /// </summary>
    struct VERTEX_Simple
    {
        VEC3 pos;      // 座標  
        VEC2 uv;       // uv値

        /* コンストラクタ */
        VERTEX_Simple() :pos(VEC3()), uv(VEC2()) {};
        VERTEX_Simple(const VEC3& _pos) : pos(_pos),  uv(VEC2()) {};
        VERTEX_Simple(const VEC3& _pos, const VEC2& _uv) : pos(_pos), uv(_uv) {};


        // 頂点同士の距離を取得(自分自身と)
        float DistanceTo(const VERTEX_Simple& target) const {
            return this->pos.Distance(this->pos, target.pos);
        }
    };

    /// <summary>
    /// 静的メッシュ
    /// 法線マップなし(tanなし)
    /// </summary>
    struct VERTEX_Static
    {
        VEC3 pos;      // 座標  
        VEC2 uv;       // uv値
        VEC4 color;    // 頂点カラー
        VEC3 normal;   // 法線

        /* コンストラクタ */
        VERTEX_Static() :pos(VEC3()), normal(VEC3()), color(VEC4()), uv(VEC2()) {};
        VERTEX_Static(const VEC3& _pos) : pos(_pos), normal(VEC3()), color(VEC4()), uv(VEC2()) {};
        VERTEX_Static(const VEC3& _pos, const VEC3& _normal) : pos(_pos), normal(_normal), color(VEC4()), uv(VEC2()) {};
        VERTEX_Static(const VEC3& _pos, const VEC3& _normal, const VEC4& _color) : pos(_pos), normal(_normal), color(_color), uv(VEC2()) {};
        VERTEX_Static(const VEC3& _pos,const VEC2& _uv, const VEC4& _color, const VEC3& _normal ) : pos(_pos), normal(_normal), color(_color), uv(_uv) {};

        // 正規化
        void Normalize() {
            this->normal = this->normal.Normalize();
        };
    };

    /// <summary>
    /// 静的メッシュ
    /// 法線マップを使う場合は基本これ
    /// </summary>
    struct VERTEX_Static_N
    {
        VEC3 pos;
        VEC2 uv;
        VEC4 color;
        VEC3 normal;
        VEC3 tangent;
        VEC3 bitangent;

        /* コンストラクタ */
        VERTEX_Static_N() : pos(VEC3()), uv(VEC2()), color(VEC4()), normal(VEC3()), tangent(VEC3()), bitangent(VEC3()) {};
        VERTEX_Static_N(const VEC3& _pos, const VEC2& _uv, const VEC4& _color, const VEC3& _normal, const VEC3& _tan, const VEC3& _biTan) : pos(_pos), normal(_normal), color(_color), uv(_uv), tangent(_tan), bitangent(_biTan) {};


        // 正規化
        void Normalize() {
            this->normal = this->normal.Normalize();
        };
    };


    /// <summary>
    /// スキニングモデル用
    /// </summary>
    struct VERTEX_Skneed
    {
        VEC3 pos;
        VEC2 uv;
        VEC4 color;
        VEC3 normal;
        VEC3 tangent;       // 接ベクトル
        VEC3 bitangent;     // 従ベクトル

        uint8_t boneIDs[4] = { 0,0,0,0 };            //影響を与えるボーンのインデックス
        float boneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };  // ボーンウェイト

        /* コンストラクタ */
        VERTEX_Skneed() : pos(VEC3()), uv(VEC2()), color(VEC4()), normal(VEC3()), tangent(VEC3()), bitangent(VEC3()){};


        // 正規化
        void Normalize() {
            this->normal = this->normal.Normalize();
        };
    };

    namespace Utils
    {
        // 頂点同士の距離を取得
        static float Distance(const VERTEX_Static& v0, const VERTEX_Static& v1) {
            return VEC3::Distance(v0.pos, v1.pos);
        }
    }
}

