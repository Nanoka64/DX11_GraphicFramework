#pragma once
#include "VEC.h"

//
/* 基本となる頂点情報 */
//
namespace BASE_VERTEX
{
    using namespace VECTOR2;
    using namespace VECTOR3;
    using namespace VECTOR4;

    struct VERTEX
    {
        VEC3 pos;      // 座標  
        VEC3 normal;   // 法線
        VEC4 color;    // 頂点カラー
        VEC2 uv;       // uv値

        /* コンストラクタ */
        VERTEX() :pos(VEC3()), normal(VEC3()), color(VEC4()), uv(VEC2()) {};
        VERTEX(const VEC3& _pos) : pos(_pos), normal(VEC3()), color(VEC4()), uv(VEC2()) {};
        VERTEX(const VEC3& _pos, const VEC3& _normal) : pos(_pos), normal(_normal), color(VEC4()), uv(VEC2()) {};
        VERTEX(const VEC3& _pos, const VEC3& _normal, const VEC4& _color) : pos(_pos), normal(_normal), color(_color), uv(VEC2()) {};
        VERTEX(const VEC3& _pos, const VEC3& _normal, const VEC4& _color, const VEC2& _uv) : pos(_pos), normal(_normal), color(_color), uv(_uv) {};

        // 正規化
        void Normalize() {
            this->normal = this->normal.Normalize();
        };

        // 頂点同士の距離を取得(自分自身と)
        float DistanceTo(const VERTEX& target) const {
            return this->pos.Distance(this->pos, target.pos);
        }
    };

    struct MODEL_VERTEX : public VERTEX
    {
        int boneIDs[4] = { -1, -1, -1, -1 }; //影響を与えるボーンのインデックス
        float boneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // ボーンウェイト
    };

    namespace Utils
    {
        // 頂点同士の距離を取得
        static float Distance(const VERTEX& v0, const VERTEX& v1) {
            return VEC3::Distance(v0.pos, v1.pos);
        }
    }
}

