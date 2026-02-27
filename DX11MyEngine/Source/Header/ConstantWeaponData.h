#pragma once

/// <summary>
/// 武器データをまとめた名前空間
/// </summary>
namespace WeaponData
{
    /// <summary>
    /// レンジャーの武器データ
    /// </summary>
    namespace Ranger
    {
        /// <summary>
        /// 装備する武器のカテゴリ
        /// </summary>
        enum class WEAPON_CATEGORY
        {
            PRIMARY01,  // メイン武器1
            PRIMARY02,  // メイン武器2
            SUB,        // サブ武器
            SPECIAL,    // 特殊（補助装備）
        };

        /// <summary>
        /// メイン武器の種類
        /// </summary>
        enum class PRIMARY_WEAPON_TYPE
        {
            ASSULT_RIFLE,       // アサルトライフル
            SHOTGUN,            // ショットガン
            SNIPER_RIFLE,       // スナイパーライフル
            ROCKET_LAUNCHER,    // ロケットランチャー
            MISSILE_LAUNCHER,   // ミサイルランチャー
            GRANADE_LAUNCHER,   // グレネードランチャー
            SUPER_WEAPON,       // スーパーウェポン
            NUM,
        };

        /// <summary>
        /// アサルトライフルの種類
        /// </summary>
        enum class ASSULT_RIFLE_ID
        {
            Hector22,
            Hector24PT,
            Hector24,

            RasterGCR1,
            RasterGCR2,
            RasterGCR3,
            RasterGCR4,
            RasterGCR5,

            NUM,
        };

    };
};