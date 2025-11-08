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
    LPDIRECTINPUTDEVICE8 m_pKeyDevice = nullptr;

public:
    static InputManager& GetInstance() {
        static InputManager instance;
        return instance;
    }

    ~InputManager();

    bool Init(HWND hWnd);
    void Update();
    void Release();

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
};

