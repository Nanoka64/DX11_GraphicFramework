#include "pch.h"
#include "InputFactory.h"
#include "InputManager.h"// 入力処理

/*
* 
* 入力状態を取得するメソッド群
* 
*/

/// <summary>
/// 長押しの場合の入力判定を取る
/// 指定のフレーム以上になったら押してる判定
/// </summary>
/// <param name="key"></param>
/// <param name="repeatFrame"></param>
/// <returns></returns>
bool Input::GetInputHold(CONFIG_INPUT key, int repeatFrame)
{
	return InputManager::GetInstance().GetInputHold(key, repeatFrame);
}

/// <summary>
/// 長押しの場合の入力判定を取る
/// 最初の押し始めは入力判定になる
/// その後、の入力判定間隔を指定できる
/// </summary>
/// <param name="KEY_INPUT"></param>
/// <param name="waitFrame"></param>
/// <param name="repeatFrame"></param>
/// <returns></returns>
bool Input::GetInputHoldRepeat(CONFIG_INPUT key, int waitFrame, int repeatFrame)
{
	return InputManager::GetInstance().GetInputHoldRepeat(key, waitFrame, repeatFrame);
}

/// <summary>
/// ボタンを離した瞬間
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool Input::GetInputUp(CONFIG_INPUT key)
{
	return InputManager::GetInstance().GetInputUp(key);
}

/// <summary>
/// ボタンが押された瞬間
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool Input::GetInputDown(CONFIG_INPUT key)
{
	return InputManager::GetInstance().GetInputDown(key);
}

/// <summary>
/// 押されたかどうか
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool Input::GetInput(CONFIG_INPUT key)
{
	return InputManager::GetInstance().GetInput(key);
}