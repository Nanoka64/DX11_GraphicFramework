#pragma once



//* =========================================================================
//* - @:移動用のパラメータ 構造体 - */
//* 【?】Behaviourクラスに渡す移動のパラメータをまとめた構造体
//*      移動方法によっては使わないパラメータもあるが、
//*      複雑な設計にするよりはまとめて渡す方がシンプルでわかりやすいので一旦こんな感じに
//* =========================================================================
struct MoveParam
{
    VECTOR3::VEC3 _moveDirection;   // 移動方向（正規化済みを渡して）
    VECTOR3::VEC3 _targetPos;       // 目標位置
    float _moveSpeed;               // 移動速度
    float _acceleration;            // 加速度
	float _turnSpeed;               // 回転速度 

    MoveParam() : 
        _moveDirection(VECTOR3::VEC3()),
        _targetPos(VECTOR3::VEC3()),
        _acceleration(0.0f),
        _turnSpeed(0.0f),
        _moveSpeed(0.0f)
    {
    }
};

struct ResultMove
{
    VECTOR3::VEC3 _moveVelocity;    // 移動ベクトル
    VECTOR3::VEC3 _rotateVelocity;  // 回転ベクトル
    ResultMove() :
        _moveVelocity(VECTOR3::VEC3()),
        _rotateVelocity(VECTOR3::VEC3())
    {
    }
};

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:IMoveBehaviour Class --- */
//
//  ★★★抽象クラス★★★
//
// 【?】移動の挙動を定義するクラス
//      例えば、プレイヤーの移動、敵の移動、弾の移動など、移動の挙動は様々なパターンがあるため、
//      それぞれの移動の挙動を定義するクラスを作成し、IMoveBehaviourクラスを継承させることで、
//      移動の挙動を定義することができるようにする。
//      MoveComponentクラスにこれらのクラスをセットすることで、移動を実装することができるようにする。
//
// ***************************************************************************************
class IMoveBehaviour
{
private:

public:
    IMoveBehaviour() = default;
    virtual ~IMoveBehaviour() = default;
    virtual ResultMove MoveCalculate(float _deltaTime, const MoveParam& _param, const class MyTransform& _transform) = 0;    // 移動計算
};