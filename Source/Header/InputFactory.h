#pragma once
#include "InputConfig.h"

/*
*
* 入力状態を取得するメソッド群
* 入力状態を取る場合はこのヘッダをインクルードすればＯＫ
*
*/
namespace Input
{
    /// <summary>
    /// 長押しの場合の入力判定を取る
    /// 指定のフレーム以上になったら押してる判定
    /// </summary>
    /// <param name="key"></param>
    /// <param name="repeatFrame"></param>
    /// <returns></returns>
    bool GetInputHold(CONFIG_INPUT key, int repeatFrame);      // 入力判定になるまでのフレーム指定用

    /// <summary>
    /// 長押しの場合の入力判定を取る
    /// 最初の押し始めは入力判定になる
    /// その後、の入力判定になる間隔を指定できる
    /// </summary>
    /// <param name="KEY_INPUT">キー</param>
    /// <param name="waitFrame"></param>
    /// <param name="repeatFrame"></param>
    /// <returns></returns>
    bool GetInputHoldRepeat(CONFIG_INPUT key, int waitFrame, int repeatFrame);   // 長押しの場合、連続入力を阻止する用

    /// <summary>
    /// ボタンを離した瞬間
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    bool GetInputUp(CONFIG_INPUT key);      // キーが離された瞬間

    /// <summary>
    /// ボタンが押された瞬間
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    bool GetInputDown(CONFIG_INPUT key);    // キーが押された瞬間

    /// <summary>
    /// 押されたかどうか（連続的に判定）
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    bool GetInput(CONFIG_INPUT key);        // キーが押されているか
};

