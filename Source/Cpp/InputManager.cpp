#include "pch.h"
#include "InputManager.h"
#include "InputConfig.h"

//--------------------------------------------------------------------------------------
//      * InputManager Class - コンストラクタ - *
//--------------------------------------------------------------------------------------
InputManager::~InputManager()
{
    this->Release();
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
    if (FAILED(hr))return false;
    
    hr = m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pKeyDevice, nullptr);
    if (FAILED(hr))return false;

    hr = m_pKeyDevice->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(hr))return false;

    hr = m_pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
    if (FAILED(hr))return false;

    // 正常終了
    return true;
}

//--------------------------------------------------------------------------------------
//      * InputManager Class - 更新 - *
//--------------------------------------------------------------------------------------
void InputManager::Update()
{
    //一時的に保持するキー情報
    char keyState[KEY_MAX]{};

    m_pKeyDevice->Acquire();
    m_pKeyDevice->GetDeviceState(sizeof(keyState), &keyState);



    for (int i = 0; i < (int)CONFIG_INPUT::MAX; i++)
    {
        CONFIG_INPUT action = (CONFIG_INPUT)i;
        int keyCode = m_ConfigKeyMap[action];

        //一つ前のキー入力を保存
        m_PrevKeyState[action] = m_CrntKeyState[action];

        //現在キーが押されているとき
        if (keyState[keyCode] & 0x80)
        {
            //押されている間フレーム数カウントアップ
            m_CrntKeyState[action]++;
        }
        else if (keyState[keyCode] == 0)
        {
            //押されていなければゼロに
            m_CrntKeyState[action] = 0;
        }
    }

    if (m_InputStopTime < 0)
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
void InputManager::Release()
{
    SAFE_RELEASE(m_pKeyDevice);
    SAFE_RELEASE(m_pDInput);
}

//--------------------------------------------------------------------------------------
//      * InputManager Class - 押下状態調べる - *
//--------------------------------------------------------------------------------------
void InputManager::GetHitInputStateAll()
{

}

//--------------------------------------------------------------------------------------
//      * InputManager Class - 押されたかどうか- *
//--------------------------------------------------------------------------------------
bool InputManager::GetInput(CONFIG_INPUT key)
{
    if (m_InputStopFlag == true) return false;

    if (m_CrntKeyState[key] > 0) return true;

    return false;
}



/// <summary>
/// 長押しの場合の入力判定を取る
/// 指定のフレーム以上になったら押してる判定
/// </summary>
/// <param name="key"></param>
/// <param name="repeatFrame"></param>
/// <returns></returns>
bool InputManager::GetInputHold(CONFIG_INPUT key, int repeatFrame)
{
    if (m_InputStopFlag == true) return false;

    //現在キーは押されていない、かつ
    //一つ前のキーが押されていた
    if (m_CrntKeyState[key] >= repeatFrame)
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
bool InputManager::GetInputHoldRepeat(CONFIG_INPUT key, int waitFrame, int repeatFrame)
{
    if (m_InputStopFlag == true) return false;

    int frame = m_CrntKeyState[key];

    // 押し始め
    if (frame == 1) return true;

    if (frame >= waitFrame && (frame - waitFrame) % repeatFrame == 0)
    {
        return true;
    }

    return false;
}


/// <summary>
/// ボタンが押された瞬間
/// </summary>
/// <param name="key"></param>
/// <returns></returns>
bool InputManager::GetInputDown(CONFIG_INPUT key)
{
    if (m_InputStopFlag) return false;

    //現在キーは押されている、かつ
    //一つ前のキーが押されていない
    if (m_CrntKeyState[key] > 0 && m_PrevKeyState[key] == 0)
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
bool InputManager::GetInputUp(CONFIG_INPUT key)
{
    if (m_InputStopFlag) return false;

    //現在キーは押されていない、かつ
    //一つ前のキーが押されていた
    if (m_CrntKeyState[key] == 0 && m_PrevKeyState[key] > 0)
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
    //m_ConfigKeyMap[CONFIG_INPUT::left] = DIK_LEFT;
    //m_ConfigKeyMap[CONFIG_INPUT::right] = DIK_RIGHT;
    //m_ConfigKeyMap[CONFIG_INPUT::up] = DIK_UP;
    //m_ConfigKeyMap[CONFIG_INPUT::down] = DIK_DOWN;
    //m_ConfigKeyMap[CONFIG_INPUT::shot] = DIK_Z;
    //m_ConfigKeyMap[CONFIG_INPUT::charge] = DIK_X;
    //m_ConfigKeyMap[CONFIG_INPUT::bom] = DIK_C;
    //m_ConfigKeyMap[CONFIG_INPUT::slow] = DIK_LSHIFT;
    //m_ConfigKeyMap[CONFIG_INPUT::pause] = DIK_ESCAPE;

    m_ConfigKeyMap[CONFIG_INPUT::LEFT] = DIK_LEFT;
    m_ConfigKeyMap[CONFIG_INPUT::RIGHT]= DIK_RIGHT;
    m_ConfigKeyMap[CONFIG_INPUT::UP]   = DIK_UP;
    m_ConfigKeyMap[CONFIG_INPUT::DOWN] = DIK_DOWN;
    m_ConfigKeyMap[CONFIG_INPUT::MOVE_F] = DIK_W;
    m_ConfigKeyMap[CONFIG_INPUT::MOVE_B] = DIK_S;
    m_ConfigKeyMap[CONFIG_INPUT::MOVE_L] = DIK_A;
    m_ConfigKeyMap[CONFIG_INPUT::MOVE_R] = DIK_D;
    m_ConfigKeyMap[CONFIG_INPUT::JUMP] = DIK_SPACE;
    m_ConfigKeyMap[CONFIG_INPUT::PAUSE] = DIK_ESCAPE;

    // キー状態初期化
    for (int i = 0; i < (int)CONFIG_INPUT::MAX; i++) {
        CONFIG_INPUT action = static_cast<CONFIG_INPUT>(i);
        m_CrntKeyState[action] = 0;
        m_PrevKeyState[action] = 0;
    }
}