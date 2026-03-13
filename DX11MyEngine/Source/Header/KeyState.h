#pragma once
//////////////////////////////////////////////////////////////////////////////////////////
//
//
//						友人のコードです。
// 
//
//////////////////////////////////////////////////////////////////////////////////////////
#include <Windows.h>



/*--------------------------------------------------------------------------------------------------------------
* フラグの種類(使ってないのもある)
*/
enum class FLAG_TYPE
{
	/*現在の状態*/
	NOW,
	/*押した始めた*/
	DOWN,
	/*離した*/
	UP,

	/*最大*/
	MAX
};


/*--------------------------------------------------------------------------------------------------------------
* コントローラーキーの種類
*/
enum class CONTROLLER_KEY_TYPE
{
	/*有無*/
	EXISTENCE = 0,

	/*Aボタン*/
	A,
	/*Bボタン*/
	B,
	/*Xボタン*/
	X,
	/*Yボタン*/
	Y,

	/*Lボタン(左トリガー)*/
	L,
	/*LTボタン(左奥トリガー)*/
	LT,
	/*Rボタン(右トリガー)*/
	R,
	/*RTボタン(右奥トリガー)*/
	RT,

	/*左ボタン*/
	LEFT,
	/*右ボタン*/
	RIGHT,
	/*上ボタン*/
	UP,
	/*下ボタン*/
	DOWN,

	/*左(左スティック)*/
	LEFT_STICK_LEFT,
	/*右(左スティック)*/
	LEFT_STICK_RIGHT,
	/*上(左スティック)*/
	LEFT_STICK_UP,
	/*下(左スティック)*/
	LEFT_STICK_DOWN,
	/*押し込み(左スティック)*/
	LEFT_STICK_PUSH,

	/*左(右スティック)*/
	RIGHT_STICK_LEFT,
	/*右(右スティック)*/
	RIGHT_STICK_RIGHT,
	/*上(右スティック)*/
	RIGHT_STICK_UP,
	/*下(右スティック)*/
	RIGHT_STICK_DOWN,
	/*押し込み(右スティック)*/
	RIGHT_STICK_PUSH,

	/*スタート*/
	START,
	/*バック*/
	BACK,

	/*最大キー数*/
	KEY_MAX
};
/*--------------------------------------------------------------------------------------------------------------
* 変動するコントローラーキーナンバー
*/
enum class CONTROLLER_KEY_VARIABLE_NUMBER
{
	/*右スティック(X軸)*/
	STICK_RIGHT_X = 0,
	/*右スティック(Y軸)*/
	STICK_RIGHT_Y,
	/*左スティック(X軸)*/
	STICK_LEFT_X,
	/*左スティック(Y軸)*/
	STICK_LEFT_Y,
	/*右奥トリガー*/
	RIGHT_TRIGGER,
	/*左奥トリガー*/
	LEFT_TRIGGER
};
/*--------------------------------------------------------------------------------------------------------------
* コントローラーナンバー
*/
enum class CONTROLLER_KEY_NUMBER
{
	/*キーボード*/
	KEY_BOARD = 0,
	/*コントローラー(1)*/
	CONTROLLER_1,
	/*コントローラー(2)*/
	CONTROLLER_2,
	/*コントローラー(3)*/
	CONTROLLER_3,
	/*コントローラー(4)*/
	CONTROLLER_4,

	/*コントローラー最大数*/
	MAX_CONTROLLER
};

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
* キー状態
*/
class KeyState
{
	/*----------------------------------------------------------------------------------------------------
	* 【共通】
	*/
public:

	/*コンストラクタ*/
	KeyState();
	/*デストラクタ*/
	~KeyState();

	/*更新*/
	void Update();

	/*フラグのポジションを取得*/
	static unsigned int FlagPos(int keyNumber) { return 1U << keyNumber; }
	/*フラグのポジションを取得(コントローラー)*/
	static unsigned int FlagPos_Controller(CONTROLLER_KEY_TYPE keyNumber) { return FlagPos((int)keyNumber); }

private:
	/*キーフラグを設定(コントローラーは現在のキーのみ設定)*/
	void SetKey();

	/*----------------------------------------------------------------------------------------------------
	* 【コントローラー(キーボードのコントローラー準拠も)】
	*/
private:
	/*コントローラーフラグズ*/
	unsigned int munControllerKeyFlags[(int)FLAG_TYPE::MAX][(int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER];

	/*右スティック横*/
	short mshNowRightStickX[(int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER];
	/*右スティック縦*/
	short mshNowRightStickY[(int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER];

	/*左スティック横*/
	short mshNowLeftStickX[(int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER];
	/*左スティック縦*/
	short mshNowLeftStickY[(int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER];

	/*左奥トリガー押し込み度合*/
	short mshNowLeftTrigger[(int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER];
	/*右奥トリガー押し込み度合*/
	short mshNowRightTrigger[(int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER];

	/*コントローラー最大数*/
	static constexpr int CONTROLLER_MAX = (const int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER;

public:
	/*押し始めたキーフラグを全取得(コントローラー)*/
	unsigned int GetDownKeyFlags_Controller(int controllerNumber) { return munControllerKeyFlags[(int)FLAG_TYPE::DOWN][controllerNumber]; }
	/*押し始めたキーフラグを全取得(コントローラー)*/
	unsigned int GetDownKeyFlags_Controller(CONTROLLER_KEY_NUMBER controllerNumber) { return GetDownKeyFlags_Controller((int)controllerNumber); }
	/*押し始めたキーフラグを全コントローラー分全取得(コントローラー)*/
	unsigned int GetAllDownKeyFlags_Controller(bool keyBoardFlag = true);

	/*押しているキーフラグを全取得(コントローラー)*/
	unsigned int GetNowKeyFlags_Controller(int controllerNumber) { return munControllerKeyFlags[(int)FLAG_TYPE::NOW][controllerNumber]; }
	/*押しているキーフラグを全取得(コントローラー)*/
	unsigned int GetNowKeyFlags_Controller(CONTROLLER_KEY_NUMBER controllerNumber) { return GetNowKeyFlags_Controller((int)controllerNumber); }
	/*押しているキーフラグを全コントローラー分全取得(コントローラー)*/
	unsigned int GetAllNowKeyFlags_Controller(bool keyBoardFlag = true);

	/*離したキーフラグを全取得(コントローラー)*/
	unsigned int GetUpKeyFlags_Controller(int controllerNumber) { return munControllerKeyFlags[(int)FLAG_TYPE::UP][controllerNumber]; }
	/*離したキーフラグを全取得(コントローラー)*/
	unsigned int GetUpKeyFlags_Controller(CONTROLLER_KEY_NUMBER controllerNumber) { return GetUpKeyFlags_Controller((int)controllerNumber); }
	/*離したキーフラグを全コントローラー分全取得(コントローラー)*/
	unsigned int GetAllUpKeyFlags_Controller(bool keyBoardFlag = true);


	/*キーナンバーに応じたキーの押しているなら「true」を返す(コントローラー)*/
	bool GetKey_Controller(int keyNumber, int controllerNumber);
	/*キーナンバーに応じたキーの押しているなら「true」を返す(コントローラー)*/
	bool GetKey_Controller(CONTROLLER_KEY_TYPE keyNumber) { return GetKey_Controller((int)keyNumber, 0); }
	/*キーナンバーに応じたキーの押しているなら「true」を返す(コントローラー)*/
	bool GetKey_Controller(CONTROLLER_KEY_TYPE keyNumber, int controllerNumber) { return GetKey_Controller((int)keyNumber, controllerNumber); }
	/*キーナンバーに応じたキーの押しているなら「true」を返す(コントローラー)*/
	bool GetKey_Controller(CONTROLLER_KEY_TYPE keyNumber, CONTROLLER_KEY_NUMBER controllerNumber) { return GetKey_Controller((int)keyNumber, (int)controllerNumber); }
	/*全コントローラーのキーナンバーに応じたキーを誰かが押しているなら「true」を返す(コントローラー)*/
	bool GetKeyAllController(int keyNumber, bool keyBoard = true) { for (int i = (keyBoard ? 0 : 1); i < (int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER; i++) { if (GetKey_Controller(keyNumber, i)) { return true; } } return false; }
	/*全コントローラーのキーナンバーに応じたキーを誰かが押しているなら「true」を返す(コントローラー)*/
	bool GetKeyAllController(CONTROLLER_KEY_TYPE keyNumber, bool keyBoard = true) { return GetKeyAllController((int)keyNumber, keyBoard); }

	/*キーナンバーに応じたキーが押された瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyDown_Controller(int keyNumber, int controllerNumber);
	/*キーナンバーに応じたキーが押された瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyDown_Controller(CONTROLLER_KEY_TYPE keyNumber) { return GetKeyDown_Controller((int)keyNumber, 0); }
	/*キーナンバーに応じたキーが押された瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyDown_Controller(CONTROLLER_KEY_TYPE keyNumber, int controllerNumber) { return GetKeyDown_Controller((int)keyNumber, controllerNumber); }
	/*キーナンバーに応じたキーが押された瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyDown_Controller(CONTROLLER_KEY_TYPE keyNumber, CONTROLLER_KEY_NUMBER controllerNumber) { return GetKeyDown_Controller((int)keyNumber, (int)controllerNumber); }
	/*全コントローラーのキーナンバーに応じたキーを誰かが押した瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyDownAllController(int keyNumber, bool keyBoard = true) { for (int i = (keyBoard ? 0 : 1); i < (int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER; i++) { if (GetKeyDown_Controller(keyNumber, i)) { return true; } } return false; }
	/*全コントローラーのキーナンバーに応じたキーを誰かが押した瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyDownAllController(CONTROLLER_KEY_TYPE keyNumber, bool keyBoard = true) { return GetKeyDownAllController((int)keyNumber, keyBoard); }

	/*キーナンバーに応じたキーが押されなくなった瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyUp_Controller(int keyNumber, int controllerNumber);
	/*キーナンバーに応じたキーが押されなくなった瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyUp_Controller(CONTROLLER_KEY_TYPE keyNumber) { return GetKeyUp_Controller((int)keyNumber, 0); }
	/*キーナンバーに応じたキーが押されなくなった瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyUp_Controller(CONTROLLER_KEY_TYPE keyNumber, int controllerNumber) { return GetKeyUp_Controller((int)keyNumber, controllerNumber); }
	/*キーナンバーに応じたキーが押されなくなった瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyUp_Controller(CONTROLLER_KEY_TYPE keyNumber, CONTROLLER_KEY_NUMBER controllerNumber) { return GetKeyUp_Controller((int)keyNumber, (int)controllerNumber); }
	/*全コントローラーのキーナンバーに応じたキーを誰かが離した瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyUpAllController(int keyNumber, bool keyBoard = true) { for (int i = (keyBoard ? 0 : 1); i < (int)CONTROLLER_KEY_NUMBER::MAX_CONTROLLER; i++) { if (GetKeyUp_Controller(keyNumber, i)) { return true; } } return false; }
	/*全コントローラーのキーナンバーに応じたキーを誰かが押した瞬間なら「true」を返す(コントローラー)*/
	bool GetKeyUpAllController(CONTROLLER_KEY_TYPE keyNumber, bool keyBoard = true) { return GetKeyUpAllController((int)keyNumber, keyBoard); }


	/*キーの変動値取得(コントローラー)*/
	short GetKeyVariable_Controller(CONTROLLER_KEY_VARIABLE_NUMBER getNumber, int controllerNumber);
	/*キーの変動値取得(コントローラー)*/
	short GetKeyVariable_Controller(CONTROLLER_KEY_VARIABLE_NUMBER getNumber, CONTROLLER_KEY_NUMBER controllerNumber = CONTROLLER_KEY_NUMBER::MAX_CONTROLLER) { return GetKeyVariable_Controller(getNumber, (int)controllerNumber); }

private:

	/*フラグを現在のキーに反映させる(コントローラー)*/
	void SetNowKey_Controller(bool flag, CONTROLLER_KEY_TYPE number, int controllerNumber = 0);
};