#pragma once
#include "ConstantUIData.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:UIManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】UIの管理を行う
//      プールからスプライトやボタンを取り出す役割を担う
//
// ***************************************************************************************
class UIManager
{
private:
    std::unordered_map<UIData::UI_TYPE, ObjectPool<GameObject>> m_UIObjectPoolMap;      // UIの連想配列プール
    std::unordered_map <UIData::UI_TYPE, std::vector<GameObject *>> m_ExtractedUIMap;   // 取り出したUIオブジェクトを一時的に保持する（役割が終わっていたら返す）

    class ButtonUI* m_pCrntFocusedButton;    // 現在選択されているボタン

public:
	UIManager();
	~UIManager();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="renderer">描画エンジンの参照</param>
    /// <returns>成功/失敗</returns>
    bool Init(RendererEngine &renderer);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="renderer">描画エンジンの参照</param>
    void Update(RendererEngine &renderer);

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="renderer">描画エンジンの参照</param>
    void Draw(RendererEngine &renderer);

    /// <summary>
    /// スプライトの取り出し
    /// 所有権の都合上、生ポインタでオブジェクトを返すけど、delete、長期保持は絶対にしないで！
    /// </summary>
    /// <param name="renderer">描画エンジン</param>
    /// <param name="_transformData">UIトランスフォームのパラメータ</param>
    /// <param name="_param">スプライトのパラメータ</param>
    /// <returns></returns>
    GameObject* GetSprite(RendererEngine &renderer, const UIData::RectTransformData &_transformData, const UIData::SpriteUIData &_param);

    /// <summary>
    /// ボタンの取り出し
    /// 所有権の都合上、生ポインタでオブジェクトを返すけど、delete、長期保持は絶対にしないで！
    /// </summary>
    /// <param name="renderer">描画エンジン</param>
    /// <param name="_transformData">UIトランスフォームのパラメータ</param>
    /// <param name="_param">ボタンのパラメータ</param>
    /// <returns></returns>
    GameObject* GetButton(RendererEngine &renderer, const UIData::RectTransformData &_transformData, const UIData::ButtonUIData &_param);

private:
	// コピー禁止
	UIManager(const UIManager &) = delete;
	UIManager &operator=(const UIManager &) = delete;
	// ------------------------------------------------------


	void ButtonInputProcess(RendererEngine& renderer);    // ボタンの入力処理
};

