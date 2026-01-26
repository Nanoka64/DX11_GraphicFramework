#pragma once

//--------------------------------------------------------------------------------------
//      * マクロ定義 *
//--------------------------------------------------------------------------------------
#define PAD_MAX 16      //パッドキーの最大数
#define KEY_MAX 256     // キーの種類


enum class INPUT_TYPE
{
    KEYBORAD_AND_MOUSE,
    GAMEPAD,
};

//--------------------------------------------------------------------------------------
//      * 列挙 ENUM *
//--------------------------------------------------------------------------------------
/* コンフィグのボタン定義 */
enum class CONFIG_INPUT {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    MOVE_F,
    MOVE_B,
    MOVE_L,
    MOVE_R,
    JUMP,
    C,
    PAUSE,

    MOVE_FORWARD,

    MAX,
};

/// <summary>
/// ゲームプレイでのコンフィグ設定
/// </summary>
enum class GAME_CONFIG
{
    // 移動
    MOVE_FORWARD,   // 前進
    MOVE_BACK,      // 後退
    MOVE_LEFT,      // 左
    MOVE_RIGHT,     // 右
    MOVE_JUMP,      // ジャンプ
    MOVE_DASH,      // ダッシュ
    MOVE_ROLLING,   // ローリング

    // 武器
    WEAPON_FIRE,    // 発射
    WEAPON_RELOAD,  // リロード
    WEAPON_CHANGE1, // 武器切り替え１
    WEAPON_CHANGE2, // 武器切り替え２
    WEAPON_ZOOM,    // ズーム

    
    PAUSE,   // ポーズ画面

    NUM,
};

enum class SYSTEM_CONFIG
{

};

/// <summary>
/// キー
/// </summary>
enum class KEY_STATE
{
    // アルファベット
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // 数字
    D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,

    // 特殊キー
    ESCAPE,
    SPACE,
    ENTER,
    LSHIFT,
    RSHIFT,
    LCTRL,
    RCTRL,
    ALT,
    TAB,

    // 矢印
    UP,
    DOWN,
    LEFT,
    RIGHT,

    MAX_KEYS // 配列サイズ確保用
};

/// <summary>
/// マウスのボタン
/// </summary>
enum class MOUSE_BUTTON_STATE
{
    LEFT,   // 左クリック
    RIGHT,  // 右クリック
    CENTER, // ホイールクリック

    // その他
    OTHER01,

    NUM,
};

/// <summary>
/// ゲームパッド
/// </summary>
enum class GAMEPAD_STATE
{

    NUM,
};
