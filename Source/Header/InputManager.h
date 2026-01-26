#pragma once
//--------------------------------------------------------------------------------------
//      * Includes *
//--------------------------------------------------------------------------------------
#include "InputConfig.h"



// =======================================================================================
//
// *---        InputManager Class         ---*
//         * シングルトンパターン *
// 
//			     入力情報の管理
//          入力処理を取る際は基本的に 
//      InputFactory.h using namespace Input
//                                      から取る
//           このクラスからは呼ばない
// 参考サイト：https://yttm-work.jp/directx/directx_0028.html
// =======================================================================================

class InputManager
{
private:
    // 必要なキーと入力フレームを連想配列で管理
    std::unordered_map<enum class CONFIG_INPUT, int> m_PrevKeyState;   // ひとつ前に押されていたキーの状態
    std::unordered_map<enum class CONFIG_INPUT, int> m_CrntKeyState;   // 現在押されているキーの状態
    std::unordered_map<enum class CONFIG_INPUT, int> m_ConfigKeyMap;   // コンフィグ用

    int m_InputStopTime = 0;    // 入力を受け付けない時間
    bool m_InputStopFlag = false;    // 入力を受け付けないフラグ

    // DirectInput用
    LPDIRECTINPUT8   m_pDInput = nullptr;

    // 入力デバイスへのアクセスに使う用らしい
    LPDIRECTINPUTDEVICE8 m_pKeyDevice = nullptr;    // キーボード用
    LPDIRECTINPUTDEVICE8 m_pMouseDevice = nullptr;  // マウス用

    POINT m_MousePos;   // マウス座標


public:
    InputManager() = default;
    ~InputManager();

    bool Init(HWND hWnd);
    void Update();
    void Term();

    void GetHitInputStateAll();   // 全ての入力の押下状態を調べる

    /* 入力情報の取得メソッド */
    bool GetInputHold(CONFIG_INPUT key, int repeatFrame);      // 入力判定になるまでのフレーム指定用
    bool GetInputHoldRepeat(CONFIG_INPUT key, int waitFrame, int repeatFrame);   // 長押しの場合、連続入力を阻止する用
    bool GetInputUp(CONFIG_INPUT key);      // キーが離された瞬間
    bool GetInputDown(CONFIG_INPUT key);    // キーが押された瞬間
    bool GetInput(CONFIG_INPUT key);        // キーが押されているか
    /* ---------------------- */

    void InitDefaultKeyConfig();            // コンフィグデフォルト設定

    void ClearInput();  // シーン遷移などの際には必ず呼ぶ

private:
    // コピー禁止
    InputManager(const InputManager &) = delete;
    InputManager &operator=(const InputManager &) = delete;
    // ------------------------------------------------------

    bool SetupKeyDevice(HWND hWnd);
    bool SetupMouseDevice(HWND hWnd);
    bool SetupPadDevice(HWND hWnd);  // 追加予定

};

