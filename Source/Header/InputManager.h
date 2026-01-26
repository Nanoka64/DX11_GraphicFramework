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
//           ：https://cammy.co.jp/technical/%e3%83%9e%e3%82%a6%e3%82%b9%e3%81%ae%e7%8a%b6%e6%85%8b%e5%8f%96%e5%be%97directinput/
// =======================================================================================

class InputManager
{
private:
    // DirectInput用
    LPDIRECTINPUT8   m_pDInput = nullptr;
    
    // 入力デバイスへのアクセスに使う用らしい
    LPDIRECTINPUTDEVICE8 m_pKeyDevice = nullptr;    // キーボード用
    LPDIRECTINPUTDEVICE8 m_pMouseDevice = nullptr;  // マウス用

    /// <summary>
    /// コンフィグ情報
    /// </summary>
    struct ConfigInfo
    {
        MOUSE_BUTTON_STATE _mouse;
        KEY_STATE _key;
        GAMEPAD_STATE _pad;
    };

    // 必要なキーと入力フレームを連想配列で管理
    std::unordered_map<enum class CONFIG_INPUT,  int> m_PrevKeyState;       // ひとつ前に押されていたキーの状態
    std::unordered_map<enum class CONFIG_INPUT,  int> m_CrntKeyState;       // 現在押されているキーの状態
    std::unordered_map<enum class CONFIG_INPUT,  int> m_ConfigKeyMap;       // コンフィグ管理用

    std::unordered_map<enum class GAME_CONFIG, ConfigInfo> m_GameConfigKeyMap;   // ゲームシーン用コンフィグ管理
    std::unordered_map<enum class SYSTEM_CONFIG, ConfigInfo> m_SystemConfigKeyMap; // システム用コンフィグ管理

    INPUT_TYPE m_CrntInputType;     // 現在の入力方式

    int m_InputStopTime = 0;        // 入力を受け付けない時間
    bool m_InputStopFlag = false;   // 入力を受け付けないフラグ
    POINT m_MousePos;               // マウス座標


    /// <summary>
    /// マウス情報
    /// </summary>
    struct MouseState{
        // rgbButtons[4]
        // [0] = 左クリック
        // [1] = 右クリック
        // [2] = ホイールクリック
        // [3] = その他
        DIMOUSESTATE _state; 
        int _count[static_cast<int>(MOUSE_BUTTON_STATE::NUM)];  // 押されている時間のカウント(ボタンの個数分)
    };

    MouseState m_CrntMouseState;  // 現在のマウス状態
    MouseState m_PrevMouseState;  // ひとつ前のマウス状態

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
    
    bool GetMouseClick(MOUSE_BUTTON_STATE _button);       // マウスボタンが押されているか
    bool GetMouseClickDown(MOUSE_BUTTON_STATE _button);   // 押された瞬間
    bool GetMouseClickUp(MOUSE_BUTTON_STATE _button);     // 離された瞬間
    
    inline POINT GetMousePos()const { return m_MousePos; };   // マウス座標の取得
    inline LONG GetMousePosSlopeX()const{return m_CrntMouseState._state.lX;};   // Xの移動量の差の取得
    inline LONG GetMousePosSlopeY()const{return m_CrntMouseState._state.lY;};   // Yの移動量の差の取得
    inline LONG GetMousePosSlopeZ()const{return m_CrntMouseState._state.lZ;};   // Zの移動量の差の取得
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


    bool KeyDeviceProcess();
    bool MouseDeviceProcess();

};

