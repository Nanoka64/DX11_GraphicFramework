#pragma once

namespace BulletData
{
    //////////////////////////////////////////////////////////////////////////////////////////
    //
    //						データ定義
    // 
    //////////////////////////////////////////////////////////////////////////////////////////

    /// <summary>
	/// 弾の見た目アーキタイプ（3Dモデルかビルボードか）を定義する
    /// </summary>
    enum class BULLET_VISUAL_ARCHETYPE
    {
        BILLBOARD,          // ビルボード
        MODEL,              // 3Dモデル
        //LASER,              // レーザー

        NUM,
    };

    /// <summary>
    /// 弾の種類
    /// </summary>
    enum class BULLET_TYPE
    {
        NORMAL,         // 通常弾（アサルトとか）
        EXPLOSION,      // 爆発系（ロケットランチャーとか）
        EXPLOSION_DELAY,// 遅延爆発系（グレネードランチャーとか）
        HORMING,        // 誘導系（ミサイルランチャーとか）
        LASER,          // レーザー系（直線）
        FLAME,          // 火炎系
        ACID,           // 酸

        NUM,
    };


    /// <summary>
    /// 弾のトランスフォーム情報
    /// </summary>
    struct BulletTransformData {
        VECTOR3::VEC3 _pos;
        //VECTOR3::VEC3 _rotRad;
        DirectX::XMVECTOR _rotQ;
        VECTOR3::VEC3 _scale;       // 弾のデータに持たせてしまったので必要ないかも？（Shot関数内で取り出し）
    };

    struct BulletDataBase
    {
        float _damage = 0.0f;                       // 弾のダメージ量
        float _speed = 0.0f;                        // 弾の速度
        float _acceleration = 0.0f;                 // 弾の加速度
        float _range = 0.0f;                        // 弾の射程距離
        float _gravityScale = 0.0f;                 // 重力の影響を受けるかどうか（0.0fなら受けない）
        float _knockbackForce = 1.0f;               // ノックバック力
        int _penetrationsCount = 0;                 // 貫通可能回数
        float _collisionSize = 0.0f;                // 衝突判定の半径
        unsigned int _collisionMask = 0;            // どのオブジェクトと衝突するか（COLLISION_CATEGORY）
        std::string _bulletMaterialTag;             // 使用するマテリアルのタグ（TODO:ビルボードと3Dモデルで分ける必要あるかも）
        std::string _decalMaterialTag;              // 衝突時に作成するデカールのタグ
        std::string _hitEffectTag;                  // 衝突時に発生させるエフェクト
        VECTOR3::VEC3 _scale = VECTOR3::VEC3();     // 弾の大きさ

        /// <summary>
        /// リセット
        /// </summary>
        void Reset()
        {
            *this = BulletDataBase();
        }
    };

    /// <summary>
    /// 通常弾のデータ（全ての武器で必須項目のため、他のデータはこれを継承する）
    /// </summary>
    struct NormalBulletData
    {
        float _damage = 0.0f;                       // 弾のダメージ量
        float _damageDistAttenuationRate = 0.0f;    // ダメージ減衰率（距離に応じて）
        float _speed = 0.0f;                        // 弾の速度
        float _acceleration = 0.0f;                 // 弾の加速度
        float _range = 0.0f;                        // 弾の射程距離
        int _penetrationsCount = 0;                 // 貫通可能回数
        float _collisionSize = 0.0f;                // 衝突判定の半径
        float _gravityScale = 0.0f;                 // 重力の影響を受けるかどうか（0.0fなら受けない）
        float _knockbackForce = 1.0f;               // ノックバック力

        unsigned int _collisionMask = 0;            // どのオブジェクトと衝突するか（COLLISION_CATEGORY）
        std::string _bulletMaterialTag;             // 使用するマテリアルのタグ（TODO:ビルボードと3Dモデルで分ける必要あるかも）
        std::string _decalMaterialTag;              // 衝突時に作成するデカールのタグ
        std::string _hitEffectTag;                  // 衝突時に発生させるエフェクト
        VECTOR3::VEC3 _scale = VECTOR3::VEC3();     // 弾の大きさ

        /// <summary>
        /// リセット
        /// </summary>
        void Reset()
        {
            *this = NormalBulletData();
        }

    };


    /// <summary>
    /// 爆発弾のデータ
    /// </summary>
    struct ExplosionBulletData : NormalBulletData
    {
        float _explosionRadius = 0.0f;             // 爆発の半径
        std::string _explosionEffectHandleTag;     // 爆発エフェクトのハンドル
        float _explosionEffectAliveTime = 1.0f;    // 爆発エフェクトの生存時間（1.0ならそのまま）
        bool _isSmoke = true;                      // 煙を出すかどうか

        /// <summary>
        /// リセット
        /// </summary>
        void Reset()
        {
            *this = ExplosionBulletData();
        }
    };


    /// <summary>
    /// 誘導弾のデータ（爆発アリ）
    /// </summary>
    struct HormingExplosionBulletData : ExplosionBulletData
    {
        float _turningSpeed = 0.0f;                 // ホーミングの回転速度（誘導性能依存）
        std::weak_ptr<class GameObject> _targetObj; // ホーミングのターゲット
        float _targetingDuration = 0.0f;            // ホーミングの誘導時間（0.0fならずっと誘導）
        float _targetingStartDelay = 0.0f;          // ホーミングの誘導開始までの遅延時間


        /// <summary>
        /// リセット
        /// </summary>
        void Reset()
        {
            *this = HormingExplosionBulletData();
        }
    };
};