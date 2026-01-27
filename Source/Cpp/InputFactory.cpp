#include "pch.h"
#include "InputFactory.h"

/*
* 
* 入力状態を取得するメソッド群
* 
*/

//-----------------------------------------------------------------------------
// ■ キー入力
//-----------------------------------------------------------------------------
/// <summary>
/// 押されたかどうか
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool GetInput(GAME_CONFIG key)
{
	return Master::m_pInputManager->GetInput(key);
}

/// <summary>
/// ボタンが押された瞬間
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool GetInputDown(GAME_CONFIG key)
{
	return Master::m_pInputManager->GetInputDown(key);
}

/// <summary>
/// ボタンを離した瞬間
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool GetInputUp(GAME_CONFIG key)
{
	return Master::m_pInputManager->GetInputUp(key);
}

/// <summary>
/// 長押しの場合の入力判定を取る
/// 指定のフレーム以上になったら押してる判定
/// </summary>
/// <param name="key"></param>
/// <param name="repeatFrame"></param>
/// <returns></returns>
bool GetInputHold(GAME_CONFIG key, int repeatFrame)
{
	return Master::m_pInputManager->GetInputHold(key, repeatFrame);
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
bool GetInputHoldRepeat(GAME_CONFIG key, int waitFrame, int repeatFrame)
{
	return Master::m_pInputManager->GetInputHoldRepeat(key, waitFrame, repeatFrame);
}

//-----------------------------------------------------------------------------
// ■ マウス入力
//-----------------------------------------------------------------------------

/// <summary>
/// 押されたかどうか
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool GetMouseClick(MOUSE_BUTTON_STATE key)
{
	return Master::m_pInputManager->GetMouseClick(key);
}

/// <summary>
/// 押された瞬間
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool GetMouseClickDown(MOUSE_BUTTON_STATE key)
{
	return Master::m_pInputManager->GetMouseClickDown(key);
}

/// <summary>
/// 離された瞬間
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool GetMouseClickUp(MOUSE_BUTTON_STATE key)
{
	return Master::m_pInputManager->GetMouseClickUp(key);
}