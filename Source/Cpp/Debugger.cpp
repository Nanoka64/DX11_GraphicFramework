#include "pch.h"
#include "Debugger.h"
#include "RendererEngine.h"


//**************************************************************************************
//      * Debugger Class - コンストラクタ - *
//=======================================================================================
Debugger::Debugger()
{

}

//**************************************************************************************
//      * Debugger Class - デストラクタ - *
//=======================================================================================
Debugger::~Debugger()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

//**************************************************************************************
//      * Debugger Class - 初期化 - *
// 引数 1.ウインドウハンドル
//      2.描画管理クラスのポインタ
//=======================================================================================
bool Debugger::Init(HWND hWnd, std::shared_ptr<class RendererEngine> renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // キーボードの入力を有効化

    ImGui::StyleColorsDark();

    // ImGuiのWindows用バックエンドとDirectX11用バックエンドの初期化をする
    if(!ImGui_ImplWin32_Init(hWnd))
        return false;
    if(!ImGui_ImplDX11_Init(renderer.get()->get_Device(), renderer.get()->get_DeviceContext()))
        return false;

    return true;
}


//**************************************************************************************
//      * Debugger Class - 描画前に呼ぶやつ - *
//=======================================================================================
void Debugger::BeginFrame(float winW, float winH)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(winW, winH);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}


//**************************************************************************************
//      * Debugger Class - 描画後に呼ぶやつ - *
//=======================================================================================
void Debugger::EndFrame()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//**************************************************************************************
//      * Debugger Class - デバッグウインドウ - *
// 引数：ラベル名
//=======================================================================================
bool Debugger::BeginDebugWindow(std::string label)
{
    return ImGui::Begin(label.c_str());
}

//**************************************************************************************
//      * Debugger Class - デバッグウインドウの終了 - *
//     BeginDebugWindowを呼んだ後は必ず呼ぶ
//=======================================================================================
void Debugger::EndDebugWindow()
{
    ImGui::End();
}

//**************************************************************************************
//      * Debugger Class - テキスト表示- *
// 引数 1.テキスト
//=======================================================================================
void Debugger::DG_Text(std::string tex)
{
    ImGui::Text(tex.c_str());
}

//**************************************************************************************
//      * Debugger Class - この関数の上のテキストと下のテキストを繋げて表示するか - *
// 引数 1.繋げる際のオフセット開始位置
//=======================================================================================
void Debugger::DG_SameLine(float offsetStart_X)
{
    ImGui::SameLine(offsetStart_X);
}

//**************************************************************************************
//      * Debugger Class - カラーピッカー  - *
// 引数 1.ラベル名
//      2.カラーの格納先
//=======================================================================================
/****** RGB ******/
void Debugger::DG_ColorPicker3(std::string label, float col[])
{
    ImGui::ColorPicker3(label.c_str(), col);
}

void Debugger::DG_ColorPicker3(std::string label, VECTOR3::VEC3* col3V)
{
    float col[3] = { col3V->x, col3V->y, col3V->z };

    ImGui::ColorPicker3(label.c_str(), col);
    *col3V = col;
}

/****** RGBA ******/
void Debugger::DG_ColorPicker4(std::string label, float col[])
{
    ImGui::ColorPicker4(label.c_str(), col);
}

void Debugger::DG_ColorPicker4(std::string label, VECTOR4::VEC4* col4V)
{
    float col[4] = { col4V->x, col4V->y, col4V->z,col4V->w };

    ImGui::ColorPicker4(label.c_str(), col);
    *col4V = col;
}


//**************************************************************************************
//      * Debugger Class - カラーエディット - *
// 引数 1.ラベル名
//      2.カラーの格納先
//=======================================================================================
/****** RGB ******/
void Debugger::DG_ColorEdit3(std::string label, float col[])
{
    ImGui::ColorPicker3(label.c_str(), col);
}

void Debugger::DG_ColorEdit3(std::string label, VECTOR3::VEC3* col3V)
{
    float col[3] = { col3V->x, col3V->y, col3V->z };

    ImGui::ColorPicker3(label.c_str(), col);
    *col3V = col;
}

/****** RGBA ******/
void Debugger::DG_ColorEdit4(std::string label, float col[])
{
    ImGui::ColorPicker4(label.c_str(), col);
}

void Debugger::DG_ColorEdit4(std::string label, VECTOR4::VEC4* col4V)
{
    float col[4] = { col4V->x, col4V->y, col4V->z,col4V->w };

    ImGui::ColorPicker4(label.c_str(), col);
    *col4V = col;
}

//**************************************************************************************
//      * Debugger Class - int ドラッグして値を変えたり、直接入力で変えることも出来る - *
// 引数 1.ラベル名
//      2.値の数（４まで対応）  
//      3.値の参照＆格納先
//      4.値の変更スピード
//      5.最小値
//      6.最大値
//=======================================================================================
void Debugger::DG_DragInt(std::string label, int valNum, int* val, float slideSpeed, int _min, int _max)
{
    switch (valNum)
    {
    case 1:
        ImGui::DragInt(label.c_str(), val, slideSpeed, _min, _max);
        break;
    case 2:
        ImGui::DragInt2(label.c_str(), val, slideSpeed, _min, _max);
        break;
    case 3:
        ImGui::DragInt3(label.c_str(), val, slideSpeed, _min, _max);
        break;
    default:
        return;
    }
}

//**************************************************************************************
//      * Debugger Class - float ドラッグして値を変えたり、直接入力で変えることも出来る - *
// 引数 1.ラベル名
//      2.値の数（４まで対応）  
//      3.値の参照＆格納先
//      4.値の変更スピード
//      5.最小値
//      6.最大値
//=======================================================================================
void Debugger::DG_DragFloat(std::string label, int valNum, float* val, float slideSpeed, float _min, float _max)
{
    switch (valNum)
    {
    case 1:
        ImGui::DragFloat(label.c_str(), val, slideSpeed, _min, _max);
        break;
    case 2:
        ImGui::DragFloat2(label.c_str(), val, slideSpeed, _min, _max);
        break;
    case 3:
        ImGui::DragFloat3(label.c_str(), val, slideSpeed, _min, _max);
        break;
    default:
        return;
    }
}


//**************************************************************************************
//      * Debugger Class - VEC3 ドラッグして値を変えたり、直接入力で変えることも出来る - *
// 引数 1.ラベル名
//      2.値の参照＆格納先
//      3.値の変更スピード
//      4.最小値
//      5.最大値
//=======================================================================================
void Debugger::DG_DragVec3(std::string label, VECTOR3::VEC3* val, float slideSpeed, float _min, float _max)
{
    float temp[3] = { val->x, val->y,  val->z };
    if (ImGui::DragFloat3(label.c_str(), temp, slideSpeed, _min, _max))
    {
        val->x = temp[0];
        val->y = temp[1];
        val->z = temp[2];
    }
}



//**************************************************************************************
//      * Debugger Class - int スライダー - *
// 引数 1.ラベル名
//      2.値の数（４まで対応）  
//      3.値の参照＆格納先
//      4.最小値
//      5.最大値
//=======================================================================================
void Debugger::DG_SliderInt(std::string label, int valNum, int* val, int _min, int _max)
{
    switch (valNum)
    {
    case 1:
        ImGui::SliderInt(label.c_str(), val, _min, _max);
        break;
    case 2:
        ImGui::SliderInt2(label.c_str(), val, _min, _max);
        break;
    case 3:
        ImGui::SliderInt3(label.c_str(), val, _min, _max);
        break;    
    case 4:
        ImGui::SliderInt4(label.c_str(), val, _min, _max);
        break;
    default:
        return;
    }
}

//**************************************************************************************
//      * Debugger Class - float スライダー - *
// 引数 1.ラベル名
//      2.値の数（４まで対応）  
//      3.値の参照＆格納先
//      4.最小値
//      5.最大値
//=======================================================================================
void Debugger::DG_SliderFloat(std::string label, int valNum, float* val, float _min, float _max)
{
    switch (valNum)
    {
    case 1:
        ImGui::SliderFloat(label.c_str(), val, _min, _max);
        break;
    case 2:
        ImGui::SliderFloat2(label.c_str(), val, _min, _max);
        break;
    case 3:
        ImGui::SliderFloat3(label.c_str(), val, _min, _max);
        break;   
    case 4:
        ImGui::SliderFloat4(label.c_str(), val, _min, _max);
        break;
    default:
        return;
    }
}

//**************************************************************************************
//      * Debugger Class - float スライダー - *
// 引数 1.ラベル名
//      2.値の数（４まで対応）  
//      3.値の参照＆格納先
//      4.最小値
//      5.最大値
//=======================================================================================
void Debugger::DG_SliderFloat(std::string label, VECTOR3::VEC3* val, float _min, float _max)
{
    float temp[3] = { val->x,val->y,val->z };

    if (ImGui::SliderFloat3(label.c_str(), temp, _min, _max))
    {
        val->x = temp[0];
        val->y = temp[1];
        val->z = temp[2];
    }
}

//**************************************************************************************
//      * Debugger Class - ボタン - *
// 引数 1.ラベル名
//      2.大きさ 
// 
// 返り値：押されたかどうか
//=======================================================================================
bool Debugger::DG_Button(std::string label, VECTOR2::VEC2 size)
{
    return ImGui::Button(label.c_str(), { size.x,size.y });
}


//**************************************************************************************
//      * Debugger Class - ラジオボタン 丸い奴 - *
// 引数 1.ラベル名
//      2.アクティブフラグ
// 
// 返り値：アクティブ状態かどうか 
//=======================================================================================
bool Debugger::DG_RadioButton(std::string label, bool active)
{
    return ImGui::RadioButton(label.c_str(), active);
}

//**************************************************************************************
//      * Debugger Class - ラジオボタン 丸い奴 - *
// 引数 1.ラベル名
//      2.アクティブフラグ 参照
// 
// 返り値：アクティブ状態かどうか 
//=======================================================================================
bool Debugger::DG_CheckBox(std::string label, bool* active)
{
    return ImGui::Checkbox(label.c_str(), active);
}

//*---------------------------------------------------------------------------------------
//*【?】画像表示
//*
//* [引数]
//* pSrv : 表示するテクスチャ(SRV)
//* size : サイズ（サイズに合わせて引き伸ばされる）
//* uv0 : 左上のUV座標
//* uv1 : 右下のUV座標
//*----------------------------------------------------------------------------------------
void Debugger::DG_Image(ID3D11ShaderResourceView* pSrv, VECTOR2::VEC2 size , VECTOR2::VEC2 uv0, VECTOR2::VEC2 uv1)
{
    // 2. ImGuiの描画コード内で呼び出し
    if (pSrv)
    {
        // 画像のサイズを指定（例: 200x200ピクセル）
        ImVec2 imageSize = ImVec2(size.x, size.y);
        ImVec2 imageUV0 = ImVec2(uv0.x,uv0.y);
        ImVec2 imageUV1 = ImVec2(uv1.x, uv1.y);

        // SRVをImTextureIDにキャストして渡す
        ImGui::Image((ImTextureID)pSrv, imageSize, imageUV0, imageUV1);
    }
}

//*---------------------------------------------------------------------------------------
//*【?】文の最初の点みたいなやつ「・」←これ
//*
//* [引数]
//* なし
//*----------------------------------------------------------------------------------------
void Debugger::DG_Bullet()
{
    ImGui::Bullet();
}

//*---------------------------------------------------------------------------------------
//*【?】「・」が最初についたテキスト
//*
//* [引数]
//* text : 文字
//*----------------------------------------------------------------------------------------
void Debugger::DG_BulletText(const std::string &text)
{
    ImGui::BulletText(text.c_str());
}

//*---------------------------------------------------------------------------------------
//*【?】スペース
//*
//* [引数]
//* なし
//*----------------------------------------------------------------------------------------
void Debugger::DG_Spacing()
{
    ImGui::Spacing();
}

//*---------------------------------------------------------------------------------------
//*【?】区切り線
//*
//* [引数]
//* なし
//*----------------------------------------------------------------------------------------
void Debugger::DG_Separator()
{
    ImGui::Separator();
}

//*---------------------------------------------------------------------------------------
//*【?】項目をツリー上に表示
//*     エラーが出るので必ず、ifで確認する  
//*     最後に必ずTreePopを呼ぶ
//* 
//* [引数]
//* label : ツリーの見出し
//* 
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool Debugger::DG_TreeNode(const std::string& label)
{
    return ImGui::TreeNode(label.c_str());
}

//*---------------------------------------------------------------------------------------
//*【?】現在のインデント（下げ）を一つ戻し、
//*     そのツリー階層が終了したことを伝える 
//*
//* [引数]
//* なし
//*----------------------------------------------------------------------------------------
void Debugger::DG_TreePop()
{
    ImGui::TreePop();
}



