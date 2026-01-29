#pragma once
namespace SceneStateEnums
{
    // ルートになるシーン定義
    enum SCENE_STATE
    {
        SCENE_STATE_TITLE,  // タイトル
        SCENE_STATE_GAME,   // ゲーム
        SCENE_STATE_RESULT, // リザルト

        SCENE_STATE_NUM,
    };

    // 子シーンの命名規則
    // Childを表す「c_」の後にシーン名（例：c_TITLE）を書く
    // タイトルの子シーン
    enum c_TITLE
    {
        c_TITLE_LOAD_PROCESS,   // ロード処理（一番最初に呼ばれる）
        c_TITLE_MISSION_SELECT, // ミッション選択
        c_TITLE_SOLDIER_SELECT, // 兵科選択
        c_TITLE_CONFIG,         // 設定

        c_GO_GAME_SCENE,        // 特殊フラグ ゲームシーンへ行くように親へ伝える

        c_TITLE_NUM, 
    };

    /// <summary>
    /// ゲームシーン
    /// </summary>
    enum c_GAME
    {
        c_GAME_LOAD,            // ロード
        c_GAME_PLAY,            // プレイ

        c_GO_RESULT_SCENE,      // 特殊フラグ リザルトシーンへ行く

        c_GAME_NUM,
    };

    /// <summary>
    /// リザルトシーン
    /// </summary>
    enum c_RESULT
    {
        c_GETITEM_CHECK,        // 取得したアイテムの確認

        c_GO_TITLE_SCENE,       // 特殊フラグ タイトルシーンへ

        c_RESULT_NUM,           
    };
};