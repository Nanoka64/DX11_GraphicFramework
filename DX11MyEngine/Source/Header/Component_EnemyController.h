#pragma once
#include "IComponent.h"
#include "ConstantUtilityData.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:EnemyController Class --- */
//
//  ★継承：IComponent ★
//
// 【?】敵の挙動を管理
//		実際の挙動はステートマシン内で行っている
//		それぞれのコンポーネントと値を行き来させる箱みたいな感じ
//		
// TODO:汎用性のない、ちぐはぐな設計になってしまっているので今後直す。
//		エネミーに必要な共通処理だけ持たせて、外から、ステートを設定するだけで、使えるようにするのがベスト
//		武器と同じように、jsonでパラメータを設定できるようにする
//		
// ***************************************************************************************
class EnemyController :  public IComponent
{
private:
	// TODO:一旦、ここにパラメータを直接置く
	const float SOUND_HIT_RADIUS = 300.0f;				// 被弾音が聞こえる範囲
	const float SOUND_DEAD_RADIUS = 600.0f;				// 死亡音が聞こえる範囲
	const int DROP_ITEM_MIN = 0;						// 落とすアイテムの最小数
	const int DROP_ITEM_MAX = 1;						// 落とすアイテムの最大数

	std::weak_ptr<class Health>m_pHealthComp;					// 体力管理コンポーネント
	std::weak_ptr<class SkinnedMeshAnimator> m_pAnimatorComp;	// アニメータコンポーネント
	std::weak_ptr<class Collider> m_pColliderComp;				// コライダーコンポーネント
	std::weak_ptr<class MoveLogic> m_pMoveLogicComp;			// 移動コンポーネント
	const GameObject* m_pTarget;								// 攻撃目標
	StateMachine<EnemyController> m_StateMachine;

	VECTOR3::VEC3 m_MoveVelocity;	// 移動
	VECTOR3::VEC3 m_StartPos;		// 開始位置
	const EnemyData::BaseEnemyData* m_pEnemyData;	// 読み取り専用データ

	int m_CrntAnimID;				// 現在のアニメーション番号
	float m_MoveSpeed;				// 移動速度
	float m_StateTimer;				// ステート時間
	float m_Gravity;				// 重力
	float m_GravityVelocity;		// 地面方向に掛かるベロシティ
	float m_AnimSpeed;				// アニメーション速度
	bool m_IsDead;
	bool m_IsAnim;					// アニメーション中かどうか
	bool m_IsGrounded;				// 接地しているか

public:
	EnemyController(std::weak_ptr<GameObject> pOwner, int updateRank);
	~EnemyController();

	void Start(RendererEngine& renderer) override;	// 初期化
	void LateUpdate(RendererEngine& renderer) override;	// 更新
	void Update(RendererEngine& renderer) override;	// 更新
	void Draw(RendererEngine& renderer) override;	// 描画
	void OnCollisionEnter(const class CollisionInfo &_other)override;
	void set_EnemyData(const EnemyData::BaseEnemyData* _enemyData) { m_pEnemyData = _enemyData; }	// エネミーデータの設定
	const EnemyData::BaseEnemyData* get_EnemyData()const;

	/// <summary>
	/// ステートの変更
	/// </summary>
	/// <param name="_state"></param>
	void ChangeState(const int _state);

	/// <summary>
	/// アニメーションの変更
	/// </summary>
	/// <param name="id"></param>
	void ChangeAnimation(const int _newId);

	/* アニメーション速度 */
	void set_AnimSpeed(float _time) { m_AnimSpeed = _time; }
	float get_AnimSpeed()const { return m_AnimSpeed; }

	/* アニメーションID */
	void set_AnimID(const int _id) { m_CrntAnimID = _id; }
	int get_AnimID()const { return m_CrntAnimID; }

	/* 移動速度 */
	float get_MoveSpeed() const { return m_MoveSpeed; }
	void set_MoveSpeed(float _spd) { m_MoveSpeed = _spd; }

	/* 移動ベクトル */
	const VECTOR3::VEC3 &get_MoveVelocity() { return m_MoveVelocity; }
	void set_MoveVelocity(const VECTOR3::VEC3 &_vel) { m_MoveVelocity = _vel; }

	/* アニメーションの再生させるかどうか */
	bool get_IsAnim() const { return m_IsAnim; }
	void set_IsAnim(bool _flag) { m_IsAnim = _flag; }

	/* 接地フラグ */
	bool get_IsGrounded() const { return m_IsGrounded; }

	/* 攻撃目標 */
	const GameObject* get_Target() const { return m_pTarget; }
	//void set_Target(std::shared_ptr<GameObject> _pObj) { m_pTarget = _pObj; }

	/* 攻撃目標のトランスフォームを取得 */
	std::shared_ptr<class MyTransform> get_TargetTransform() const;

	/* ステート時間 */
	float get_StateTimer()const { return m_StateTimer; }
	void clear_StateTimer() { m_StateTimer = 0.0f; }	


	/* 移動ロジックの切り替え */
	void set_MoveLogicState(UtilityData::MOVE_BEHAVIOUR_TYPE _moveType);

	/* 移動コンポーネントを取得する */
	std::weak_ptr<class MoveLogic> get_MoveLogicComponent() { return m_pMoveLogicComp; };

	/* 開始時の位置を取得する*/
	const VECTOR3::VEC3 &get_StartPos() { return m_StartPos; }

	/* 死亡フラグ */
	bool get_IsDead()const { return m_IsDead; }
};
