#pragma once
#include "IComponent.h"
#include "RootSceneState.h"
#include "StateMachine.h"

/// <summary>
/// プレイヤーのアニメーション
/// 末尾にRMとついているものはアニメーション自体に移動成分が入っているもの
/// </summary>
enum class PLAYER_ANIMATION_ID
{
	NONE = -1,

	T_POSE = 0,
	CROUCH_FWD_LOOP,
	CROUCH_IDLE_LOOP,
	DANCE_LOOP,
	DEATH01,
	DRIVING_LOOP,
	FIXING_KNEELING,
	HIT_CHEST,
	HIT_HEAD,
	IDLE_LOOP,
	IDLE_TALKING_LOOP,
	IDLE_TORCH_LOOP,
	INTERACT,
	JOG_FWD_LOOP,
	JUMP_LAND,
	JUMP_LOOP,
	JUMP_START,
	PICKUP_TABLE,
	PISTOL_AIM_DOWN,
	PISTOL_AIM_NEUTRAL,
	PISTOL_AIM_UP,
	PISTOL_IDLE_LOOP,
	PISTOL_RELOAD,
	PISTOL_SHOOT,
	PANCH_CROSS,
	PUNCH_JAB,
	PUSH_LOOP,
	ROLL,
	ROLL_RM,
	SITTING_ENTER,
	SITTING_EXIT,
	SITTING_IDLE_LOOP,
	SITTING_TALKING_LOOP,
	SPELL_SIMPLE_ENTER,
	SPELL_SIMPLE_EXIT,
	SPELL_SIMPLE_IDLE_LOOP,
	SPRINT_LOOP,
	SWIM_FWD_LOOP,
	SWIM_IDLE_LOOP,
	SWORD_ATTACK,
	SWORD_ATTACK_RM,
	SWORD_IDLE,
	WALK_FORMAL_LOOP,
	WALK_LOOP,


	NUM,
};

const std::string g_PlayerAnimationNames[] = 
{
	"NONE",
	
	"T_POSE",
	"CROUCH_FWD_LOOP",
	"CROUCH_IDLE_LOOP",
	"DANCE_LOOP",
	"DEATH01",
	"DRIVING_LOOP",
	"FIXING_KNEELING",
	"HIT_CHEST",
	"HIT_HEAD",
	"IDLE_LOOP",
	"IDLE_TALKING_LOOP",
	"IDLE_TORCH_LOOP",
	"INTERACT",
	"JOG_FWD_LOOP",
	"JUMP_LAND",
	"JUMP_LOOP",
	"JUMP_START",
	"PICKUP_TABLE",
	"PISTOL_AIM_DOWN",
	"PISTOL_AIM_NEUTRAL",
	"PISTOL_AIM_UP",
	"PISTOL_IDLE_LOOP",
	"PISTOL_RELOAD",
	"PISTOL_SHOOT",
	"PANCH_CROSS",
	"PUNCH_JAB",
	"PUSH_LOOP",
	"ROLL",
	"ROLL_RM",
	"SITTING_ENTER",
	"SITTING_EXIT",
	"SITTING_IDLE_LOOP",
	"SITTING_TALKING_LOOP",
	"SPELL_SIMPLE_ENTER",
	"SPELL_SIMPLE_EXIT",
	"SPELL_SIMPLE_IDLE_LOOP",
	"SPRINT_LOOP",
	"SWIM_FWD_LOOP",
	"SWIM_IDLE_LOOP",
	"SWORD_ATTACK",
	"SWORD_ATTACK_RM",
	"SWORD_IDLE",
	"WALK_FORMAL_LOOP",
	"WALK_LOOP",
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:PlayerController Class --- */
//
//  ★継承：IComponent ★
//
// 【?】プレイヤー操作を行う
//		
// ***************************************************************************************
class PlayerController : public IComponent
{
private:
    StateMachine<PlayerController> m_StateMachine;
	std::shared_ptr<class Camera3D> m_pCameraComp;
    bool m_IsAnim;		// アニメーション中かどうか
    float m_MoveSpeed;	// 移動速度
	bool m_IsJump;
	VECTOR3::VEC3 m_MoveVelocity;	// 移動
	std::shared_ptr<class SkinnedMeshAnimator> m_AnimatorComp;
	PLAYER_ANIMATION_ID m_CrntAnimID;	// 現在のアニメーションID

	// リジッドボディコンポーネントを作って移す
	float m_JumpForce = 3.0f;	// ジャンプ力
	float m_Gravity = 0.18f;	// 重力
public:
	PlayerController(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	~PlayerController();

	void Start(RendererEngine& renderer) override;		// 初期化
	void Update(RendererEngine& renderer) override;	// 更新処理
	void Draw(RendererEngine& renderer)override;		// 描画処理

	void OnCollisionEnter(const class CollisionInfo &other)override;

	/* 移動速度 */
    float get_MoveSpeed() const { return m_MoveSpeed; }
    void set_MoveSpeed(float speed) { m_MoveSpeed = speed; }

	/* アニメーションID */
	void set_AnimID(PLAYER_ANIMATION_ID id) { m_CrntAnimID = id; };
	PLAYER_ANIMATION_ID get_AnimID()const { return m_CrntAnimID; };

	/* 移動速度ベクトル */
	void set_MoveVelocity(const VECTOR3::VEC3 &vec) { m_MoveVelocity = vec; }
	VECTOR3::VEC3 get_MoveVelocity()const { return m_MoveVelocity; }

	/* ジャンプフラグ */
	bool get_IsJump()const { return m_IsJump; };

	/* ジャンプ力 */
	void set_JumpFoce(float jf) { m_JumpForce = jf; }
	float get_JumpFoce()const { return m_JumpForce; }

	/* 重力 */
	void set_Gravity(float gv) { m_Gravity = gv; }
	float get_Gravity()const { return m_Gravity; }

private:
	/// <summary>
	/// アニメーションの変更
	/// </summary>
	/// <param name="id"></param>
	void ChangeAnimation(PLAYER_ANIMATION_ID id);
};

