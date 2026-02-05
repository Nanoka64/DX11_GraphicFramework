#pragma once
#include<memory>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// =======================================================================================
//      * Debugger Class *
//        - シングルトン - 
//    Imgui使用 デバッグ全般
// https://qiita.com/ousttrue/items/ae7c8d5715adffc5b1fa
// =======================================================================================
class Debugger
{
private:

public:
	Debugger();
	~Debugger();

	// コピーコンストラクタと代入演算子を削除
	Debugger(const Debugger&) = delete;;
	Debugger& operator = (const Debugger&) = delete;


	// 初期化
	bool Init(HWND hWnd, std::shared_ptr<class RendererEngine> renderer);

	// 描画開始前の処理
	void BeginFrame(float winW,float winH);

	// 描画終了後の処理
	void EndFrame();

	bool BeginDebugWindow(const std::string &_label);
	void EndDebugWindow();

	void DG_Text(std::string tex);																			// テキスト表示
	void DG_SameLine(float offsetStart_X = 0.0f);															// この関数の上と下のテキストを横につなげる
	
	void DG_ColorPicker3(const std::string &label, float col[]);													// カラーピッカー3
	void DG_ColorPicker3(const std::string &label, VECTOR3::VEC3* col3V);											// カラーピッカー3
	void DG_ColorPicker4(const std::string &label, float col[]);													// カラーピッカー4
	void DG_ColorPicker4(const std::string &label, VECTOR4::VEC4* col4V);											// カラーピッカー4
	void DG_ColorEdit3(const std::string &label, float col[]);														// カラーエディット3
	void DG_ColorEdit3(const std::string &label, VECTOR3::VEC3* col3V);											// カラーエディット3
	void DG_ColorEdit4(const std::string &label, float col[]);														// カラーエディット4
	void DG_ColorEdit4(const std::string &label, VECTOR4::VEC4* col4V);											// カラーエディット4

	void DG_DragInt(const std::string &label, int valNum, int* val, float slideSpeed, int _min, int _max);			// int
	void DG_DragFloat(const std::string &label, int valNum, float* val, float slideSpeed, float _min, float _max);	// float
	void DG_DragVec3(const std::string &label, VECTOR3::VEC3* val, float slideSpeed, float _min, float _max);		// VEC3
	void DG_SliderInt(const std::string &label, int valNum, int* val, int _min, int _max);							// スライダー -int-
	void DG_SliderFloat(const std::string &label, int valNum, float* val, float _min, float _max);					// スライダー -float-
	void DG_SliderFloat(const std::string &label, VECTOR3::VEC3* val, float _min, float _max);						// スライダー -VEC3-
	bool DG_Button(const std::string &label, VECTOR2::VEC2 size = { 0.0f,0.0f });									// ボタン
	bool DG_RadioButton(const std::string &label, bool active);													// ラジオボタン
	bool DG_CheckBox(const std::string &label, bool *active);														// チェックボックス

	void DG_Image(ID3D11ShaderResourceView* pSrv, VECTOR2::VEC2 size = { 200.0f,200.0f }, VECTOR2::VEC2 uv0 = { 0.0f,0.0f }, VECTOR2::VEC2 uv1 = { 1.0f,1.0f });
	void DG_Bullet();
	void DG_BulletText(const std::string& label);
	void DG_Spacing();
	void DG_Separator();
	bool DG_TreeNode(const std::string& label);
	void DG_TreePop();

	void DG_Dummy(const VECTOR2::VEC2 &_size);	// サイズ分、隙間を開けることができる
	bool DG_Selectable(const std::string &_label, bool _selected, ImGuiSelectableFlags _flag, VECTOR2::VEC2 _size);
	

	// 後で作る
	bool DG_Combo()
	{
		// 選択肢が出てきて選べるみたいなやつ
		//ImGui::Combo("combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");
	}

	/* テンプレート */
	
	// テキスト表示
	template<typename ...Args>
	void DG_TextValue(const std::string& tex, Args&& ...args)
	{
		ImGui::Text(tex.c_str(), std::forward<Args>(args)...);
	}	
	
	// バレットテキスト表示
	template<typename ...Args>
	void DG_BulletText(const std::string& tex, Args&& ...args)
	{
		ImGui::BulletText(tex.c_str(), std::forward<Args>(args)...);
	}
};

