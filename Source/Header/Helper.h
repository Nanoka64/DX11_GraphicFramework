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
    inline float RandRange(float min, float max)
    {
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        return min + r * (max - min);
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
        inline float InQuad(float t) {
            return t * t;
        }

        // 強い加速
        inline float InCubic(float t) {
            return t * t * t;
        }

        // 自然な加速
        inline float InSine(float t) {
            return 1.0f - cosf((t * DirectX::XM_PI) / 2.0f);
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


    // stringからwstringへの変換
    inline std::wstring StringToWstring(const std::string& str)
    {
        std::wstring ret;
        //一度目の呼び出しは文字列数を知るため
        auto result = MultiByteToWideChar(CP_UTF8,
            0,
            str.c_str(),//入力文字列
            static_cast<int>(str.length()),
            nullptr,
            0);
        assert(result >= 0);
        ret.resize(result);//確保する
        //二度目の呼び出しは変換
        result = MultiByteToWideChar(CP_UTF8,
            0,
            str.c_str(),//入力文字列
            static_cast<int>(str.length()),
            ret.data(),
            static_cast<int>(ret.size()));
        return ret;
    }
}











