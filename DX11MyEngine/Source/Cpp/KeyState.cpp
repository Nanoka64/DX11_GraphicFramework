//////////////////////////////////////////////////////////////////////////////////////////
//
//
//						友人のコードです。
// 
//
//////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <Xinput.h>

#include <iostream>

#include "../Header/KeyState.h"

#pragma comment(lib, "Xinput.lib")

/*----------------------------------------------------------------------------------------------------
* 【共通】
*/
// コンストラクタ
KeyState::KeyState()
{
	// キーフラグ(コントローラー)
	memset(munControllerKeyFlags, 0, sizeof(unsigned int) * (int)FLAG_TYPE::MAX * (int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER);

	// キー変動値
	memset(mshNowRightStickX, 0, sizeof(short) * (int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER);
	memset(mshNowRightStickY, 0, sizeof(short) * (int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER);
	memset(mshNowLeftStickX, 0, sizeof(short) * (int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER);
	memset(mshNowLeftStickY, 0, sizeof(short) * (int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER);
	memset(mshNowRightTrigger, 0, sizeof(short) * (int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER);
	memset(mshNowLeftTrigger, 0, sizeof(short) * (int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER);

}

// デストラクタ
KeyState::~KeyState()
{
}

// 更新
void KeyState::Update()
{
	unsigned int prevKey[CONTROLLER_MAX];
	memcpy(prevKey, munControllerKeyFlags[(int)FLAG_TYPE::NOW], sizeof(unsigned int) * CONTROLLER_MAX);
	memset(munControllerKeyFlags[(int)FLAG_TYPE::NOW], 0, sizeof(unsigned int) * CONTROLLER_MAX);

	SetKey();

	for (int i = 0; i < CONTROLLER_MAX; i++)
	{
		munControllerKeyFlags[(int)FLAG_TYPE::DOWN][i] = ((~prevKey[i]) & munControllerKeyFlags[(int)FLAG_TYPE::NOW][i]);
		munControllerKeyFlags[(int)FLAG_TYPE::UP][i] = (prevKey[i] & (~munControllerKeyFlags[(int)FLAG_TYPE::NOW][i]));
	}
}

// キーフラグを設定(コントローラーは現在のキーのみ設定)
void KeyState::SetKey()
{

	int controllerNumber = (int)CONTROLLER_KEY_NUMBER::CONTROLLER_1;
	// 【コントローラー】
	for (int i = controllerNumber; i < (XUSER_MAX_COUNT + controllerNumber); i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		// 取得
		unsigned long result = XInputGetState(i - controllerNumber, &state);

		// 接続がないなら何もしない
		if (result == ERROR_SUCCESS)
		{
			SetNowKey_Controller(true, CONTROLLER_KEY_TYPE::EXISTENCE, i);
			// ボタン取得
			unsigned short buttons = state.Gamepad.wButtons;

			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_A, CONTROLLER_KEY_TYPE::A, i);
			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_B, CONTROLLER_KEY_TYPE::B, i);
			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_X, CONTROLLER_KEY_TYPE::X, i);
			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_Y, CONTROLLER_KEY_TYPE::Y, i);

			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_DPAD_LEFT, CONTROLLER_KEY_TYPE::LEFT, i);
			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_DPAD_RIGHT, CONTROLLER_KEY_TYPE::RIGHT, i);
			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_DPAD_UP, CONTROLLER_KEY_TYPE::UP, i);
			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_DPAD_DOWN, CONTROLLER_KEY_TYPE::DOWN, i);

			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER, CONTROLLER_KEY_TYPE::R, i);
			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_LEFT_SHOULDER, CONTROLLER_KEY_TYPE::L, i);

			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_LEFT_THUMB, CONTROLLER_KEY_TYPE::LEFT_STICK_PUSH, i);
			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_RIGHT_THUMB, CONTROLLER_KEY_TYPE::RIGHT_STICK_PUSH, i);

			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_START, CONTROLLER_KEY_TYPE::START, i);
			SetNowKey_Controller(buttons & XINPUT_GAMEPAD_BACK, CONTROLLER_KEY_TYPE::BACK, i);

			// RT LT 取得
			mshNowLeftTrigger[i] = state.Gamepad.bLeftTrigger;
			mshNowRightTrigger[i] = state.Gamepad.bRightTrigger;
			SetNowKey_Controller(mshNowLeftTrigger[i] > 0, CONTROLLER_KEY_TYPE::LT, i);
			SetNowKey_Controller(mshNowRightTrigger[i] > 0, CONTROLLER_KEY_TYPE::RT, i);

			// スティック取得
			mshNowLeftStickX[i] = state.Gamepad.sThumbLX;
			mshNowLeftStickY[i] = state.Gamepad.sThumbLY;

			mshNowRightStickX[i] = state.Gamepad.sThumbRX;
			mshNowRightStickY[i] = state.Gamepad.sThumbRY;

			SetNowKey_Controller(mshNowLeftStickX[i] > 0, CONTROLLER_KEY_TYPE::LEFT_STICK_RIGHT, i);
			SetNowKey_Controller(mshNowLeftStickX[i] < 0, CONTROLLER_KEY_TYPE::LEFT_STICK_LEFT, i);

			SetNowKey_Controller(mshNowLeftStickY[i] < 0, CONTROLLER_KEY_TYPE::LEFT_STICK_UP, i);
			SetNowKey_Controller(mshNowLeftStickY[i] > 0, CONTROLLER_KEY_TYPE::LEFT_STICK_DOWN, i);

			SetNowKey_Controller(mshNowRightStickX[i] > 0, CONTROLLER_KEY_TYPE::RIGHT_STICK_RIGHT, i);
			SetNowKey_Controller(mshNowRightStickX[i] < 0, CONTROLLER_KEY_TYPE::RIGHT_STICK_LEFT, i);

			SetNowKey_Controller(mshNowRightStickY[i] < 0, CONTROLLER_KEY_TYPE::RIGHT_STICK_UP, i);
			SetNowKey_Controller(mshNowRightStickY[i] > 0, CONTROLLER_KEY_TYPE::RIGHT_STICK_DOWN, i);
		}
	}

}

/*----------------------------------------------------------------------------------------------------
* 【コントローラー】
*/
// 押し始めたキーフラグを全コントローラー分全取得(コントローラー)
unsigned int KeyState::GetAllDownKeyFlags_Controller(bool keyBoardFlag)
{
	unsigned int result = 0;

	for (int i = 0; i < CONTROLLER_MAX; i++)
	{
		if (!keyBoardFlag && (i == (int)CONTROLLER_KEY_NUMBER::KEY_BOARD))
		{
			continue;
		}
		result |= munControllerKeyFlags[(int)FLAG_TYPE::DOWN][i];
	}

	return result;
}

// 押しているキーフラグを全コントローラー分全取得(コントローラー)
unsigned int KeyState::GetAllNowKeyFlags_Controller(bool keyBoardFlag)
{
	unsigned int result = 0;

	for (int i = 0; i < CONTROLLER_MAX; i++)
	{
		if (!keyBoardFlag && (i == (int)CONTROLLER_KEY_NUMBER::KEY_BOARD))
		{
			continue;
		}
		result |= munControllerKeyFlags[(int)FLAG_TYPE::NOW][i];
	}

	return result;
}

// 離したキーフラグを全コントローラー分全取得(コントローラー)
unsigned int KeyState::GetAllUpKeyFlags_Controller(bool keyBoardFlag)
{
	unsigned int result = 0;

	for (int i = 0; i < CONTROLLER_MAX; i++)
	{
		if (!keyBoardFlag && (i == (int)CONTROLLER_KEY_NUMBER::KEY_BOARD))
		{
			continue;
		}
		result |= munControllerKeyFlags[(int)FLAG_TYPE::UP][i];
	}

	return result;
}

// キーナンバーに応じたキーの押しているなら「true」を返す(コントローラー)
bool KeyState::GetKey_Controller(int keyNumber, int controllerNumber)
{
	return (((munControllerKeyFlags[(int)FLAG_TYPE::NOW][controllerNumber] >> keyNumber) & 1) != 0);
}


// キーナンバーに応じたキーが押された瞬間なら「true」を返す(コントローラー)
bool KeyState::GetKeyDown_Controller(int keyNumber, int controllerNumber)
{
	return (((munControllerKeyFlags[(int)FLAG_TYPE::DOWN][controllerNumber] >> keyNumber) & 1) != 0);
}

// キーナンバーに応じたキーが押されなくなった瞬間なら「true」を返す(コントローラー)
bool KeyState::GetKeyUp_Controller(int keyNumber, int controllerNumber)
{
	return (((munControllerKeyFlags[(int)FLAG_TYPE::UP][controllerNumber] >> keyNumber) & 1) != 0);
}

// キーの変動値取得(コントローラー)
short KeyState::GetKeyVariable_Controller(CONTROLLER_KEY_VARIABLE_NUMBER getNumber, int controllerNumber)
{
	switch (getNumber)
	{
	case CONTROLLER_KEY_VARIABLE_NUMBER::STICK_RIGHT_X:
		return mshNowRightStickX[controllerNumber];

	case CONTROLLER_KEY_VARIABLE_NUMBER::STICK_RIGHT_Y:
		return mshNowRightStickY[controllerNumber];

	case CONTROLLER_KEY_VARIABLE_NUMBER::STICK_LEFT_X:
		return mshNowLeftStickX[controllerNumber];

	case CONTROLLER_KEY_VARIABLE_NUMBER::STICK_LEFT_Y:
		return mshNowLeftStickY[controllerNumber];

	case CONTROLLER_KEY_VARIABLE_NUMBER::RIGHT_TRIGGER:
		return mshNowRightTrigger[controllerNumber];

	case CONTROLLER_KEY_VARIABLE_NUMBER::LEFT_TRIGGER:
		return mshNowLeftTrigger[controllerNumber];
	}

	return (short)0;
}

// フラグを現在のキーに反映させる(コントローラー)
void KeyState::SetNowKey_Controller(bool flag, CONTROLLER_KEY_TYPE number, int controllerNumbe)
{
	if (flag)
	{
		munControllerKeyFlags[(int)FLAG_TYPE::NOW][controllerNumbe] |= ((unsigned int)1 << (int)number);
	}
}
