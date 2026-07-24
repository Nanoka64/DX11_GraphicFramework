#pragma once

namespace BulletData
{
    //////////////////////////////////////////////////////////////////////////////////////////
    //
    //						データ定義
    // 
    //////////////////////////////////////////////////////////////////////////////////////////

    enum class BULLET_BEHAVIOUR_TYPE
    {
        NORMAL,
        EXPLOSION,

        HOMING,

        NUM,
    };

    /// <summary>
    /// 弾の移動タイプ
    /// </summary>
    enum class BULLET_MOVE_TYPE
    {
        LINEAR,
        HOMING,

    };

    /// <summary>
	/// 弾の見た目アーキタイプ（3Dモデルかビルボードか）を定義する
    /// </summary>
    enum class BULLET_VISUAL_ARCHETYPE
    {
        BILLBOARD,          // ビルボード
        MODEL,              // 3Dモデル
        //LASER,            // レーザー

        NUM,
    };

    /// <summary>
    /// 弾の種類（弾の種類でステートを変える）
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
    /// 弾の状態
    /// </summary>
    enum class BULLET_STATE
    {
        FLYING,
        HOMING,
        ATTACHED
    };

    /// <summary>
    /// どういった内容で生存が終了したか
    /// </summary>
    enum class BULLET_END_REASON
    {
        HIT,
        OUT_OF_RANGE,
        CANCELLED
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

    /// <summary>
    /// 弾の生成に必要な情報
    /// </summary>
    struct BulletSpawnContext
    {
        BulletTransformData _transform;
        std::weak_ptr<GameObject> _target;
        std::weak_ptr<GameObject> _shooter;
    };

    /// <summary>
    /// 見た目データ
    /// </summary>
    struct BulletVisualData
    {
        BULLET_VISUAL_ARCHETYPE _visualArchetype = BULLET_VISUAL_ARCHETYPE::BILLBOARD;
        std::string _bulletMaterialTag;                  // 使用するマテリアルのタグ（TODO:ビルボードと3Dモデルで分ける必要あるかも）
        VECTOR3::VEC3 _scale = VECTOR3::VEC3();          // 弾の大きさ

        int _trailDrawTime = 0;                          // 軌跡の表示時間（0なら非表示）
        float _trailWidth = 0;                           // 軌跡の幅
        VECTOR3::VEC3 _trailColor = VECTOR3::VEC3(1.0f); // トレイルカラー
        float _smokeInterval = 0.05f;                    // 煙の間隔
        std::string _smokeEffectTag;                     // 煙エフェクトのタグ
        bool _enableFlightSmoke = false;                 // 飛行中、煙を発生させるか
    };


    /// <summary>
    /// 弾のこうしん返却値
    /// </summary>
    struct BulletMoveResult
    {
        VECTOR3::VEC3 _velocity;
        DirectX::XMVECTOR _rotation;
    };

    /// <summary>
    /// 弾がヒットした際の返却値
    /// </summary>
    struct BulletHitResult
    {
        bool _deactivate = false;
        bool _penetration = false;
    };


    /// <summary>
    /// 弾の共通データ
    /// </summary>
    struct CommonBulletData
    {
        float _damage = 0.0f;                       // 弾のダメージ量
        float _range = 0.0f;                        // 弾の射程距離
        float _speed = 0.0f;                        // 弾の速度
        float _acceleration = 0.0f;                 // 弾の加速度
        float _gravityScale = 0.0f;                 // 重力の影響を受けるかどうか（0.0fなら受けない）
        float _knockbackForce = 1.0f;               // ノックバック力
        int _penetrationsCount = 0;                 // 貫通可能回数
        unsigned int _collisionMask = 0;            // どのオブジェクトと衝突するか（COLLISION_CATEGORY）
        float _collisionSize = 0.0f;                // 衝突判定の半径
    };

    /// <summary>
    /// 弾の移動データ
    /// </summary>
    struct LinearMoveData
    {

    };
    /// <summary>
    /// 追尾弾の移動データ
    /// </summary>
    struct HomingMoveData
    {
        float _turnSpeed = 0.0f;
        float _targetingDuration = 0.0f;
        float _targetingStartDelay = 0.0f;
    };

    /// <summary>
    /// 衝突データ
    /// </summary>
    struct DirectHitData
    {
        // 必要なら貫通や命中エフェクト固有値を持たせる
        std::string _decalMaterialTag;              // 衝突時に作成するデカールのタグ
        std::string _hitEffectTag;                  // 衝突時に発生させるエフェクト
    };

    /// <summary>
    /// 爆発弾の衝突データ
    /// </summary>
    struct ExplosionHitData : DirectHitData
    {
        float _explosionRadius = 0.0f;              // 爆発の半径
        float _explosionEffectAliveTime = 1.0f;     // 爆発エフェクトの生存時間（1.0ならそのまま）
        std::string _explosionEffectHandleTag;      // 爆発エフェクトのハンドル
        bool _isSmoke = true;                       // 煙を出すかどうか

        /// <summary>
        /// リセット
        /// </summary>
        void Reset()
        {
            *this = ExplosionHitData();
        }
    };


    ///// <summary>
    ///// 誘導弾のデータ（爆発アリ）
    ///// </summary>
    //struct HormingExplosionBulletData
    //{
    //    float _turningSpeed = 0.0f;                 // ホーミングの回転速度（誘導性能依存）
    //    std::weak_ptr<class GameObject> _targetObj; // ホーミングのターゲット
    //    float _targetingDuration = 0.0f;            // ホーミングの誘導時間（0.0fならずっと誘導）
    //    float _targetingStartDelay = 0.0f;          // ホーミングの誘導開始までの遅延時間
    //    /// <summary>
    //    /// リセット
    //    /// </summary>
    //    void Reset()
    //    {
    //        *this = HormingExplosionBulletData();
    //    }
    //};


    /// <summary> 共用ヒットデータ </summary>
    using BulletHitData = std::variant<
        DirectHitData,
        ExplosionHitData
    >;
    /// <summary> 共用移動データ </summary>
    using BulletMovementData = std::variant<
        LinearMoveData,
        HomingMoveData
    >;

    /// <summary>
    /// 基本の弾データ
    /// </summary>
    struct BulletDataBase
    {
        CommonBulletData _commonData;                           // 共通データ
        BulletMovementData _moveData;                           // 移動データ
        BulletVisualData _visualData;                           // 見た目データ
        BulletHitData _hitData = DirectHitData{};               // ヒットデータ

        /// <summary>
        /// リセット
        /// </summary>
        void Reset()
        {
            *this = BulletDataBase();
        }
    };



    struct BulletRuntime
    {
        BULLET_STATE _state = BULLET_STATE::FLYING;

        VECTOR3::VEC3 _moveDirection;
        class MyTransform* _transform = nullptr; 
        float _currentSpeed = 0.0f;
        float _traveledDistance = 0.0f;
        float _stateTime = 0.0f;

        std::weak_ptr<GameObject> _target;
        std::weak_ptr<GameObject> _attachedObject;
        VECTOR3::VEC3 _attachedLocalPosition;
    };


    ///// <summary>
    ///// 通常弾のデータ（全ての武器で必須項目のため、他のデータはこれを継承する）
    ///// </summary>
    //struct NormalBulletData
    //{
    //    float _damage = 0.0f;                       // 弾のダメージ量
    //    float _damageDistAttenuationRate = 0.0f;    // ダメージ減衰率（距離に応じて）
    //    float _speed = 0.0f;                        // 弾の速度
    //    float _acceleration = 0.0f;                 // 弾の加速度
    //    float _range = 0.0f;                        // 弾の射程距離
    //    int _penetrationsCount = 0;                 // 貫通可能回数
    //    float _collisionSize = 0.0f;                // 衝突判定の半径
    //    float _gravityScale = 0.0f;                 // 重力の影響を受けるかどうか（0.0fなら受けない）
    //    float _knockbackForce = 1.0f;               // ノックバック力

    //    unsigned int _collisionMask = 0;            // どのオブジェクトと衝突するか（COLLISION_CATEGORY）
    //    std::string _bulletMaterialTag;             // 使用するマテリアルのタグ（TODO:ビルボードと3Dモデルで分ける必要あるかも）
    //    std::string _decalMaterialTag;              // 衝突時に作成するデカールのタグ
    //    std::string _hitEffectTag;                  // 衝突時に発生させるエフェクト
    //    VECTOR3::VEC3 _scale = VECTOR3::VEC3();     // 弾の大きさ

    //    /// <summary>
    //    /// リセット
    //    /// </summary>
    //    void Reset()
    //    {
    //        *this = NormalBulletData();
    //    }
    //};

};