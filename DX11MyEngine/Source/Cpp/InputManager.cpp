#include "pch.h"
#include "InputManager.h"
#include "InputConfig.h"
using namespace Tool;
//--------------------------------------------------------------------------------------
//      * InputManager Class - コンストラクタ - *
//--------------------------------------------------------------------------------------
InputManager::~InputManager()
{
}

//--------------------------------------------------------------------------------------
//      * InputManager Class - 初期化 (ウインドウハンドル渡す)- *
//--------------------------------------------------------------------------------------
bool InputManager::Init(HWND hWnd)
{
    HRESULT hr = S_OK;

    // コンフィグ初期化
    InitDefaultKeyConfig();

    m_InputStopTime = 0;
    m_InputStopFlag = false;

    hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, nullptr);
    if (FAILED(hr)) {
        MessageBox(NULL, "DirectInput8Create", "Error", MB_OK);
        return false;
    }

    // キーデバイスのセットアップ
    if (SetupKeyDevice(hWnd) == false)
    {
        MessageBox(NULL, "キーデバイスの作成ができませんでした", "Error", MB_OK);
        return false;
    }

    // マウスデバイスのセットアップ
    if (SetupMouseDevice(hWnd) == false)
    {
        MessageBox(NULL, "マウスデバイスの作成ができませんでした", "Error", MB_OK);
        return false;
    }

    // キー入力
    m_CrntInputType = INPUT_TYPE::KEYBORAD_AND_MOUSE;

    // 正常終了
    return true;
}

//--------------------------------------------------------------------------------------
//      * InputManager Class - 更新 - *
//--------------------------------------------------------------------------------------
void InputManager::Update()
{
    HRESULT hr = S_OK;

    // キー処理
    if (!KeyDeviceProcess())
    {
        return;
    }
    // マウス処理
    if (!MouseDeviceProcess())
    {
        return;
    }

    // マウス座標取得
    GetCursorPos(&m_MousePos);

    // マウス座標をスクリーン座標からウインドウ座標へ変換
    ScreenToClient(FindWindowA(g_WindowClassNameA, nullptr), &m_MousePos);


    Master::m_pDebugger->BeginDebugWindow(U8ToChar(u8"マウス情報"));
    Master::m_pDebugger->DG_TextValue("CrntCount : %d",m_CrntMouseState._count[0]);
    Master::m_pDebugger->DG_TextValue("PrevCount : %d", m_PrevMouseState._count[0]);
    Master::m_pDebugger->DG_TextValue("X : %d", m_MousePos.x);
    Master::m_pDebugger->DG_TextValue("Y : %d", m_MousePos.y);
    Master::m_pDebugger->DG_TextValue("lX : %d", m_CrntMouseState._state.lX);
    Master::m_pDebugger->DG_TextValue("lY : %d", m_CrntMouseState._state.lY);   
    Master::m_pDebugger->EndDebugWindow();


    if (m_InputStopTime <= 0)
    {
        m_InputStopTime = 0;
        m_InputStopFlag = false;
    }
    else
    {
        m_InputStopFlag = true;
        m_InputStopTime--;
    }
}

//--------------------------------------------------------------------------------------
//      * InputManager Class - リソース解放 - *
//--------------------------------------------------------------------------------------
void InputManager::Term()
{
    // デバイスの制御を停止
    if (m_pKeyDevice != nullptr)
    {
        m_pKeyDevice->Unacquire();
        SAFE_RELEASE(m_pKeyDevice);

    }
    if (m_pMouseDevice != nullptr)
    {
        m_pMouseDevice->Unacquire();
        SAFE_RELEASE(m_pMouseDevice);

    }
    if (m_pDInput != nullptr)
    {
        SAFE_RELEASE(m_pDInput);
    }
}

//--------------------------------------------------------------------------------------
//      * InputManager Class - 押されたかどうか- *
//--------------------------------------------------------------------------------------
bool InputManager::GetInput(GAME_CONFIG key)
{
    if (m_InputStopFlag == true) return false;

    if (m_CrntGameConfigCountersMap[key] > 0) return true;

    return false;
}

/// <summary>
/// ボタンが押された瞬間
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool InputManager::GetInputDown(GAME_CONFIG key)
{
    if (m_InputStopFlag) return false;

    //現在キーは押されている、かつ
    //一つ前のキーが押されていない
    if (m_CrntGameConfigCountersMap[key] > 0 && m_PrevGameConfigCountersMap[key] == 0)
    {
        return true;
    }

    return false;
}


/// <summary>
/// ボタンを離した瞬間
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool InputManager::GetInputUp(GAME_CONFIG key)
{
    if (m_InputStopFlag) return false;

    //現在キーは押されていない、かつ
    //一つ前のキーが押されていた
    if (m_CrntGameConfigCountersMap[key] == 0 && m_PrevGameConfigCountersMap[key] > 0)
    {
        return true;
    }

    return false;
}


/// <summary>
/// 長押しの場合の入力判定を取る
/// 指定のフレーム以上になったら押してる判定
/// </summary>
/// <param name="key"></param>
/// <param name="repeatFrame"></param>
/// <returns></returns>
bool InputManager::GetInputHold(GAME_CONFIG key, int repeatFrame)
{
    if (m_InputStopFlag == true) return false;

    //現在キーは押されていない、かつ
    //一つ前のキーが押されていた
    if (m_CrntGameConfigCountersMap[key] >= repeatFrame)
    {
        return true;
    }

    return false;
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
bool InputManager::GetInputHoldRepeat(GAME_CONFIG key, int waitFrame, int repeatFrame)
{
    if (m_InputStopFlag == true) return false;

    int frame = m_CrntGameConfigCountersMap[key];

    // 押し始め
    if (frame == 1) return true;

    if (frame >= waitFrame && (frame - waitFrame) % repeatFrame == 0)
    {
        return true;
    }

    return false;
}


/// <summary>
/// クリックされているかどうか
/// </summary>
/// <param name="_button"></param>
/// <returns></returns>
bool InputManager::GetMouseClick(MOUSE_BUTTON_STATE _button)const
{
    if (m_InputStopFlag) return false;
    if (_button == MOUSE_BUTTON_STATE::NUM)return false;

    // 一フレームでも押されていればtrue
    if (m_CrntMouseState._count[static_cast<int>(_button)] > 0)
    {
        return true;
    }

    return false;
}

/// <summary>
/// 離された瞬間
/// </summary>
/// <param name="_button"></param>
/// <returns></returns>
bool InputManager::GetMouseClickUp(MOUSE_BUTTON_STATE _button)const
{
    if (m_InputStopFlag) return false;
    if (_button == MOUSE_BUTTON_STATE::NUM)return false;

    //現在キーは押されていない、かつ
    //一つ前のキーが押されていた
    if (m_CrntMouseState._count[static_cast<int>(_button)] == 0 &&
        m_PrevMouseState._count[static_cast<int>(_button)] > 0)
    {
        return true;
    }

    return false;
}


/// <summary>
/// 押された瞬間
/// </summary>
/// <param name="_button"></param>
/// <returns></returns>
bool InputManager::GetMouseClickDown(MOUSE_BUTTON_STATE _button)const
{
    if (m_InputStopFlag) return false;
    if (_button == MOUSE_BUTTON_STATE::NUM)return false;

    // 現在は押されている、かつ
    // 一つ前のが押されていない
    if (m_CrntMouseState._count[static_cast<int>(_button)] > 0 &&
        m_PrevMouseState._count[static_cast<int>(_button)] == 0)
    {
        return true;
    }
    return false;
}


/// <summary>
/// 長押しの場合の入力判定を取る
/// 最初の押し始めは入力判定になる
/// その後、の入力判定間隔を指定できる
/// </summary>
/// <param name="_button"></param>
/// <returns></returns>
bool InputManager::GetMouseClickHoldRepeat(MOUSE_BUTTON_STATE _button, int _waitFrame, int _repeatFrame)const
{
    if (m_InputStopFlag) return false;
    if (_button == MOUSE_BUTTON_STATE::NUM)return false;
    int frame = m_CrntMouseState._count[static_cast<int>(_button)];

    // 押し始め
    if (frame == 1) return true;

    if (frame >= _waitFrame && (frame - _waitFrame) % _repeatFrame == 0)
    {
        return true;
    }

    return false;
}


void InputManager::ClearInput()
{
    m_InputStopFlag = true;
    m_InputStopTime = 10;
}

//--------------------------------------------------------------------------------------
//      * InputManager Class - デフォルトコンフィグ- *
//--------------------------------------------------------------------------------------
void InputManager::InitDefaultKeyConfig()
{
    // ゲームシーンコンフィグ
    // 移動
    m_GameConfigMap[GAME_CONFIG::MOVE_FORWARD]._key   = DIK_W;                    // 前進
    m_GameConfigMap[GAME_CONFIG::MOVE_BACK]._key      = DIK_S;                    // 後退
    m_GameConfigMap[GAME_CONFIG::MOVE_LEFT]._key      = DIK_A;                    // 左
    m_GameConfigMap[GAME_CONFIG::MOVE_RIGHT]._key     = DIK_D;                    // 右
    m_GameConfigMap[GAME_CONFIG::MOVE_JUMP]._key      = DIK_SPACE;                // ジャンプ
    m_GameConfigMap[GAME_CONFIG::MOVE_DASH]._key      = DIK_LSHIFT;               // ダッシュ
    
    // 視点移動
    m_GameConfigMap[GAME_CONFIG::VIEW_UP]._key        = DIK_UP;                   // 上↑
    m_GameConfigMap[GAME_CONFIG::VIEW_DOWN]._key      = DIK_DOWN;                 // 下↓
    m_GameConfigMap[GAME_CONFIG::VIEW_LEFT]._key      = DIK_LEFT;                 // 左←
    m_GameConfigMap[GAME_CONFIG::VIEW_RIGHT]._key     = DIK_RIGHT;                // 右→

    // 武器
    m_GameConfigMap[GAME_CONFIG::WEAPON_FIRE]._key    = DIK_F;                     // 発射
    m_GameConfigMap[GAME_CONFIG::WEAPON_FIRE]._mouse  = MOUSE_BUTTON_STATE::LEFT;  // 発射 マウス
    m_GameConfigMap[GAME_CONFIG::WEAPON_RELOAD]._key  = DIK_R;                     // リロード
    m_GameConfigMap[GAME_CONFIG::WEAPON_CHANGE1]._key = DIK_1;                     // 武器切り替え１
    m_GameConfigMap[GAME_CONFIG::WEAPON_CHANGE2]._key = DIK_2;                     // 武器切り替え２
    m_GameConfigMap[GAME_CONFIG::WEAPON_ZOOM]._key    = DIK_G;                     // ズーム
    m_GameConfigMap[GAME_CONFIG::WEAPON_ZOOM]._mouse  = MOUSE_BUTTON_STATE::RIGHT; // ズーム マウス

    m_GameConfigMap[GAME_CONFIG::PAUSE]._key          = DIK_ESCAPE;                 // ポーズ画面

    // キー状態初期化
    for (int i = 0; i < (int)GAME_CONFIG::NUM; i++) {
        GAME_CONFIG action = static_cast<GAME_CONFIG>(i);
        m_PrevGameConfigCountersMap[action] = 0;
        m_CrntGameConfigCountersMap[action] = 0;
    }
    

    //// キー状態初期化
    //for (int i = 0; i < (int)CONFIG_INPUT::MAX; i++) {
    //    CONFIG_INPUT action = static_cast<CONFIG_INPUT>(i);
    //    m_CrntKeyState[action] = 0;
    //    m_PrevKeyState[action] = 0;
    //}
}

//*---------------------------------------------------------------------------------------
//*【?】キーデバイスのセットアップ
//*
//* [引数]
//* hWnd : ウィンドウハンドル
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool InputManager::SetupKeyDevice(HWND hWnd)
{
    HRESULT hr = S_OK;
    // デバイスの作成
    hr = m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pKeyDevice, nullptr);
    if (FAILED(hr)) {
        MessageBox(NULL, "Key CreateDevice", "Error", MB_OK);
        return false;
    }

    // フォーマットの設定（どんな情報を受け取るか）
    hr = m_pKeyDevice->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(hr)) {
        MessageBox(NULL, "Key SetDataFormat", "Error", MB_OK);
        return false;
    }

    // 協調モードの設定
    // （詳しくは
    // https://yttm-work.jp/directx/directx_0028.html 
    // http://www.marupeke296.com/DI_InitDirectInput.htmlを見て）
    // ・入力の権利設定
    // DISCL_FOREGROUND : ウィンドウがアクティブの時にのみ権利を得ることが出来る（基本こっち）
    // DISCL_BACKGROUND : 非アクティブ時でも操作を受け付けてしまう
    
    // ・入力デバイスの優先権
    // DISCL_NONEXCLUSIVE : デバイスを共有する（基本こっち）
    // DISCL_EXCLUSIVE : デバイスを占有する
    hr = m_pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    if (FAILED(hr)) {
        MessageBox(NULL, "Key SetCooperativeLevel", "Error", MB_OK);
        return false;
    }

    return true;
}


//*---------------------------------------------------------------------------------------
//*【?】マウスデバイスのセットアップ
//*
//* [引数]
//* hWnd : ウィンドウハンドル
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool InputManager::SetupMouseDevice(HWND hWnd)
{
    HRESULT hr = S_OK;

    // デバイスの作成（マウス用）
    hr = m_pDInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, nullptr);
    if (FAILED(hr)) {
        MessageBox(NULL, "Mouse CreateDevice", "Error", MB_OK);
        return false;
    }

    // フォーマットの設定（どんな情報を受け取るか）
    hr = m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
    if (FAILED(hr)) {
        MessageBox(NULL, "Mouse SetDataFormat", "Error", MB_OK);
        return false;
    }

    // 協調モードの設定（詳しくは SetupKeyDevice の方に書いてある）
    hr = m_pMouseDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    if (FAILED(hr)) {
        MessageBox(NULL, "Mouse SetCooperativeLevel", "Error", MB_OK);
        return false;
    }
    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】キー情報の更新
//*
//* [引数]
//* なし 
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool InputManager::KeyDeviceProcess()
{
    HRESULT hr = S_OK;

    //一時的に保持するキー情報
    char keyState[KEY_MAX]{};

    // **************************************************
    // キーデバイス状態の取得
    // **************************************************
    hr = m_pKeyDevice->Acquire();
    if (FAILED(hr)) {
        return false;
    }
    // デバイスとの同期を取る
    hr = m_pKeyDevice->Poll();
    if (FAILED(hr)) {
        return false;
    }
    hr = m_pKeyDevice->GetDeviceState(sizeof(keyState), keyState);
    if (FAILED(hr)) {
        return false;
    }

    //
    // キー状態の更新
    //
    for (int i = 0; i < static_cast<int>(GAME_CONFIG::NUM); i++)
    {
        GAME_CONFIG action = (GAME_CONFIG)i;
        int keyCode = m_GameConfigMap[action]._key;
        if (keyCode == -1)continue;

        //一つ前のキー入力を保存
        m_PrevGameConfigCountersMap[action] = m_CrntGameConfigCountersMap[action];

        //現在キーが押されているとき
        if (keyState[keyCode] & 0x80)
        {
            //押されている間フレーム数カウントアップ
            m_CrntGameConfigCountersMap[action]++;
        }
        else if (keyState[keyCode] == 0)
        {
            //押されていなければゼロに
            m_CrntGameConfigCountersMap[action] = 0;
        }
    }

    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】マウス情報の更新
//*
//* [引数]
//* なし 
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool InputManager::MouseDeviceProcess()
{
    HRESULT hr = S_OK;

    // **************************************************
    // マウスデバイス状態の取得
    // **************************************************
    hr = m_pMouseDevice->Acquire();
    if (FAILED(hr)) {
        return false;
    }
    // デバイスとの同期を取る
    hr = m_pMouseDevice->Poll();
    if (FAILED(hr)) {
        return false;
    }

    // 前回の状態として保持
    m_PrevMouseState = m_CrntMouseState;

    for (int i = 0; i < static_cast<int>(MOUSE_BUTTON_STATE::NUM); i++)
    {
        // 押されていればカウントアップ
        if (m_CrntMouseState._state.rgbButtons[i] & 0x80)
        {
            m_CrntMouseState._count[i]++;
        }
        else
        {
            m_CrntMouseState._count[i] = 0;
        }
    }

    //c_dfDIMouseを設定した場合はDIMOUSESTATEを取得する
    hr = m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_CrntMouseState._state);
    if (FAILED(hr)) {
        return false;
    }
    return true;
}
