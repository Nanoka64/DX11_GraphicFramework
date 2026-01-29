#include "pch.h"
#include "RenderPipeline.h"
#include "RendererEngine.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "GameObjectManager.h"
#include "MeshFactory.h"
#include "Component_SpriteRenderer.h"
#include "Component_SkyRenderer.h"
#include "Component_BillboardRenderer.h"
#include "DX_RenderTarget.h"
#include "GaussianBlur.h"

using namespace VECTOR4;
using namespace VECTOR3;
using namespace VECTOR2;
using namespace Tool;


constexpr float DOF_MAX_RANGE = 2800.0f;
constexpr float DOF_MIN_RANGE = 300.0f;


//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
RenderPipeline::RenderPipeline() :
    m_pAlbedo_RT(nullptr),
    m_pNormal_RT(nullptr),
    m_pDepth_RT(nullptr),
    m_pSpecular_RT(nullptr),
    m_pSceneFinal_RT(nullptr),
    m_pLuminance_RT(nullptr),
    m_pShadowMap_RT(nullptr),
    m_GaussWeights(),
    m_pDoF_GaussianBlur(nullptr),
    m_pBloomGaussianBlur(nullptr),
    m_pShadowGaussianBlur(nullptr),
    m_pEmissive_RT(nullptr),
    m_DoF_BlurIncensity(2.0f),
    m_ShadowData(),
    m_DofData(),
    m_ScreenWidth(0),
    m_ScreenHeight(0)
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
RenderPipeline::~RenderPipeline()
{
}

//*---------------------------------------------------------------------------------------
//*【?】パイプラインの作成
//*
//* [引数]
//* renderer : 描画エンジンの参照
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool RenderPipeline::Setup(RendererEngine &renderer)
{
    bool result = true;

    // シャドウバイアスの設定
    m_ShadowData.baseShadowBias = 0.0005f;
    m_ShadowData.depthBiasClamp = 0.001f;
    m_ShadowData.slopeScaledBias = 0.1f;

    // 被写界深度の設定
    m_DofData.dof_MaxRange = DOF_MAX_RANGE;
    m_DofData.dof_MinRange = DOF_MIN_RANGE;

    // スクリーンの大きさを取得
    m_ScreenWidth = static_cast<float>(renderer.get_ScreenWidth());
    m_ScreenHeight = static_cast<float>(renderer.get_ScreenHeight());

    // レンダリングターゲットの作成
    if (!CreateRenderTargets(renderer))
    {
        return false;
    }
    // ポストエフェクトの作成
    if (!CreatePostEffect(renderer)) 
    {
        return false;
    }
    // レンダーターゲット用スプライトの作成
    if (!CreateRenderTargetSprites(renderer)) 
    { 
        return false; 
    }

    return true;
}



//*---------------------------------------------------------------------------------------
//*【?】パイプラインの実行
//*
//* [引数]
//* renderer : 描画エンジンの参照
//*
//* [返値]
//* なし 
//*----------------------------------------------------------------------------------------
void RenderPipeline::Execute(RendererEngine &renderer)
{

    // レンダーターゲットデバッグ
    {
        Master::m_pDebugger->BeginDebugWindow(U8ToChar(u8"レンダーターゲット"));

        if (Master::m_pDebugger->DG_TreeNode("G-Buffer"))
        {
            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"アルベド"));
            Master::m_pDebugger->DG_Image(m_pAlbedo_RT->get_SRV(), VEC2(400, 200));
            Master::m_pDebugger->DG_Separator();

            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"法線"));
            Master::m_pDebugger->DG_Image(m_pNormal_RT->get_SRV(), VEC2(400, 200));
            Master::m_pDebugger->DG_Separator();

            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"スペキュラ"));
            Master::m_pDebugger->DG_Image(m_pSpecular_RT->get_SRV(), VEC2(400, 200));
            Master::m_pDebugger->DG_Separator();

            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"エミッシブ"));
            Master::m_pDebugger->DG_Image(m_pEmissive_RT->get_SRV(), VEC2(400, 200));
            Master::m_pDebugger->DG_Separator();

            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"深度"));
            Master::m_pDebugger->DG_Image(m_pDepth_RT->get_DepthSRV_ComPtr().Get(), VEC2(400, 200));
            Master::m_pDebugger->DG_Separator();

            Master::m_pDebugger->DG_TreePop();// G-Buffer終了
        }
        Master::m_pDebugger->DG_Separator();

        if (Master::m_pDebugger->DG_TreeNode(U8ToChar(u8"ポストエフェクト")))
        {
            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"輝度抽出"));
            Master::m_pDebugger->DG_Image(m_pLuminance_RT->get_SRV(), VEC2(400, 200));
            Master::m_pDebugger->DG_Separator();

            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"被写界深度ブラー"));
            Master::m_pDebugger->DG_Image(m_pDoF_GaussianBlur->get_AfterBlurTexture().Get(), VEC2(400, 200));
            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"強度"));
            Master::m_pDebugger->DG_SliderFloat("##DofBlur", 1, &m_DoF_BlurIncensity, 0.1f, 32.0f);
            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"ブラー開始距離"));
            Master::m_pDebugger->DG_SameLine();
            Master::m_pDebugger->DG_SliderFloat("##DofMinRange", 1, &m_DofData.dof_MinRange, 0.1f, 1000.0f);
            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"ブラー最大位置"));
            Master::m_pDebugger->DG_SameLine();
            Master::m_pDebugger->DG_SliderFloat("##DofMaxRange", 1, &m_DofData.dof_MaxRange, 1.0f, 10000.0f);
            Master::m_pDebugger->DG_Separator();

            Master::m_pDebugger->DG_TreePop();  // ポストエフェクト終了
        }
        Master::m_pDebugger->DG_Separator();

        if (Master::m_pDebugger->DG_TreeNode(U8ToChar(u8"シャドウ")))
        {
            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"シャドウマップ"));
            Master::m_pDebugger->DG_Image(m_pShadowMap_RT->get_SRV_ComPtr().Get(), VEC2(400, 200));    
            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"ブラー掛け"));
            Master::m_pDebugger->DG_Image(m_pShadowGaussianBlur->get_AfterBlurTexture().Get(), VEC2(400, 200));
            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"バイアス"));
            Master::m_pDebugger->DG_SameLine();
            Master::m_pDebugger->DG_DragFloat("##BaseBias", 1, &m_ShadowData.baseShadowBias,   0.0001f, 0.0f,   0.1f);
            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"傾斜バイアス"));
            Master::m_pDebugger->DG_SameLine();
            Master::m_pDebugger->DG_DragFloat("##SlopeBias", 1, &m_ShadowData.slopeScaledBias, 0.0001f, 0.0f,   1.0);
            Master::m_pDebugger->DG_BulletText(U8ToChar(u8"最大バイアス"));
            Master::m_pDebugger->DG_SameLine();
            Master::m_pDebugger->DG_DragFloat("##ClampBias", 1, &m_ShadowData.depthBiasClamp,  0.0001f, 0.0001f,  0.1f);

            Master::m_pDebugger->DG_Separator();

            Master::m_pDebugger->DG_TreePop();  // シャドウマップ終了
        }
        Master::m_pDebugger->DG_Separator();

        Master::m_pDebugger->DG_BulletText(U8ToChar(u8"最終合成（トーンマッピングなし）"));
        Master::m_pDebugger->DG_Image(m_pSceneFinal_RT->get_SRV(), VEC2(400, 200));
        Master::m_pDebugger->DG_Separator();

        Master::m_pDebugger->EndDebugWindow();
    }

    /* シャドウパス */
    Shadow_PathRender(renderer);
    // パス終了時にSRVを解除する
    ID3D11ShaderResourceView* nullSRVs[8] = { nullptr };
    renderer.get_DeviceContext()->PSSetShaderResources(0, 8, nullSRVs);
    renderer.get_DeviceContext()->VSSetShaderResources(0, 8, nullSRVs);

    /* ジオメトリパス */
    Geometry_PathRender(renderer);
    renderer.get_DeviceContext()->PSSetShaderResources(0, 8, nullSRVs);
    renderer.get_DeviceContext()->VSSetShaderResources(0, 8, nullSRVs);

    /* ディファードライティングパス */
    Lighting_PathRender(renderer);
    renderer.get_DeviceContext()->PSSetShaderResources(0, 8, nullSRVs);
    renderer.get_DeviceContext()->VSSetShaderResources(0, 8, nullSRVs);

    /* フォワードパス */
    Forward_PathRender(renderer);
 
    /* ポストエフェクトパス */
    PostEffect_PathRender(renderer);

    /* 最終パス（フレームバッファにコピー） */
    CopyToFrameBuffer_PathRender(renderer);
}

//*---------------------------------------------------------------------------------------
//*【?】解放
//*
//* [引数]
//* なし
//* [返値]
//* なし 
//*----------------------------------------------------------------------------------------
void RenderPipeline::Release()
{
    SAFE_DELETE(m_pAlbedo_RT);
    SAFE_DELETE(m_pNormal_RT);
    SAFE_DELETE(m_pSpecular_RT);
    SAFE_DELETE(m_pEmissive_RT);
    SAFE_DELETE(m_pDepth_RT);
    SAFE_DELETE(m_pSceneFinal_RT);
    SAFE_DELETE(m_pLuminance_RT);
    SAFE_DELETE(m_pShadowMap_RT);
}


//*---------------------------------------------------------------------------------------
//*【?】ジオメトリパス Gバッファ作成
//*
//* [引数]
//* renderer : 描画エンジンの参照
//* [返値] なし
//*----------------------------------------------------------------------------------------
void RenderPipeline::Geometry_PathRender(RendererEngine &renderer)
{
    // Gバッファ
    DX_RenderTarget *gbuffer[] = {
        m_pAlbedo_RT ,
        m_pNormal_RT,
        m_pSpecular_RT,
        m_pEmissive_RT,
        m_pDepth_RT,
    };

    // ビューポートの設定
    renderer.set_ViewPort(0, 0, m_ScreenWidth, m_ScreenHeight);
    renderer.RegisterCullMode(CULL_MODE::BACK);     // 裏カリング

    // レンダリングターゲットの設定とクリア
    renderer.RegisterRenderTargets(ARRAYSIZE(gbuffer), gbuffer, m_pDepth_RT->get_DSV());
    renderer.ClearRenderTargetViews(ARRAYSIZE(gbuffer), gbuffer);

    // メイン描画パス
    renderer.set_CrntRenderPass(RENDER_PASS::MAIN);

    // オブジェクト描画
    Master::m_pGameObjectManager->ObjectMainRenderPass(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();
}

//*---------------------------------------------------------------------------------------
//*【?】シャドウパス ライティングの前に
//*
//* [引数]
//* renderer : 描画エンジンの参照
//* [返値] なし
//*----------------------------------------------------------------------------------------
void RenderPipeline::Shadow_PathRender(RendererEngine &renderer)
{
    // レンダリングターゲットの設定とクリア
    renderer.RegisterRenderTargetAndViewPort(m_pShadowMap_RT);
    renderer.ClearRenderTargetView(m_pShadowMap_RT);

    // シャドウパス
    renderer.set_CrntRenderPass(RENDER_PASS::SHADOW);

    // 表カリング 影が浮いているような感じ（ピーターパン現象）を防ぐため
    // ※表カリングにすると影が白くなってしまうので一旦裏カリングに
    renderer.RegisterCullMode(CULL_MODE::BACK);     

    // ライトの更新
    Master::m_pLightManager->Update();

    // シャドウ用オブジェクト描画
    Master::m_pGameObjectManager->ObjectShadowRenderPass(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();

    // シャドウマップへブラーを掛ける
    m_pShadowGaussianBlur->ExcuteOnGPU(renderer, 0.5f);
}

//*---------------------------------------------------------------------------------------
//*【?】ディファードライティングパス
//*
//* [引数]
//* renderer : 描画エンジンの参照
//* [返値] なし
//*----------------------------------------------------------------------------------------
void RenderPipeline::Lighting_PathRender(RendererEngine &renderer)
{
    // ビューポートの設定
    renderer.set_ViewPort(0, 0, m_ScreenWidth, m_ScreenHeight);

    // 最終合成用レンダリングターゲットに変更
    renderer.RegisterRenderTarget(m_pSceneFinal_RT->get_RTV(), m_pSceneFinal_RT->get_DSV());
    renderer.ClearRenderTargetView(m_pSceneFinal_RT);

    // 裏カリング
    renderer.RegisterCullMode(CULL_MODE::BACK);

    // ディファードスプライト
    //Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::NONE);
    //m_pDefferdLighting_Sprite->setToGPU_ExtendUserPS_CBuffer(renderer, 0, &m_ShadowData);
    m_pDefferdLighting_Sprite->Draw(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();
}


//*---------------------------------------------------------------------------------------
//*【?】フォワードパス
//*
//* [引数]
//* renderer : 描画エンジンの参照
//* [返値] なし
//*----------------------------------------------------------------------------------------
void RenderPipeline::Forward_PathRender(RendererEngine &renderer)
{
    // ************************************************************************
    // 
    // フォワードの場合はこの下に記述
    // 
    // ************************************************************************
    // Gbuffer作成時の深度バッファを設定
    // ライティングパス時のRTに合成する
    // 深度はGbuffer作成時のもの
    renderer.RegisterRenderTarget(m_pSceneFinal_RT->get_RTV(), m_pDepth_RT->get_DSV());

    // ************************************************************************
    // 
    // ここはスカイボックス描画（一番遠い場所にあるため）
    // 
    // ************************************************************************
    // スカイボックス用のデプスステンシル登録
    renderer.RegisterDepthStencilState(renderer.get_DepthTestDisabled_DSS(), 0);

    // 表カリング スカイボックスはボックスの内側に表示しているため
    renderer.RegisterCullMode(CULL_MODE::FRONT);    

    auto skybox = Master::m_pGameObjectManager->get_ObjectByTag("Skybox");
    skybox->get_Component<SkyRenderer>()->Draw(renderer);

    // スカイボックス深度ステンシル設定解除
    renderer.RegisterDepthStencilState(NULL, 0);

    // ************************************************************************
    // 
    // ここからフォワードオブジェクトの描画
    // 
    // ************************************************************************

    // デフォルトの深度ステンシル設定に戻す
    renderer.RegisterDefaultDepthStencilState();
    
    // カリングなし
    renderer.RegisterCullMode(CULL_MODE::BACK);

    Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::ALPHA);
    // 透明度アリオブジェクトの描画
    Master::m_pGameObjectManager->Alpha_ObjectRenderPass(renderer);

    Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::NONE);


    // オブジェクトの書き込み後に深度バッファをクリアする
    //renderer.ClearRenderTargetView(m_pDepth_RT);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();
}


//*---------------------------------------------------------------------------------------
//*【?】ポストエフェクトパス
//*
//* [引数]
//* renderer : 描画エンジンの参照
//* [返値] なし
//*----------------------------------------------------------------------------------------
void RenderPipeline::PostEffect_PathRender(RendererEngine &renderer)
{
    // ************************************************************************
    // 
    // 被写界深度
    //
    // ************************************************************************

    // 被写界深度用ガウスブラー実行
    m_pDoF_GaussianBlur->ExcuteOnGPU(renderer, m_DoF_BlurIncensity);
   
    // 加算モード
    Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::ALPHA);

    // 裏カリング
    renderer.RegisterCullMode(CULL_MODE::BACK);

    // ビューポートの設定
    renderer.set_ViewPort(0, 0, m_ScreenWidth, m_ScreenHeight);

    // 被写界深度用レンダリングターゲットに変更
    renderer.RegisterRenderTarget(m_pSceneFinal_RT->get_RTV(), nullptr);

    // 定数バッファに送信
    m_pDoF_Sprite->setToGPU_ExtendUserPS_CBuffer(renderer, 0, &m_DofData);

    // 被写界深度用スプライト
    m_pDoF_Sprite->Draw(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();

    Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::NONE);

    // ************************************************************************
    // 
    // 輝度抽出
    //
    // ************************************************************************
    
    // 裏カリング
    renderer.RegisterCullMode(CULL_MODE::BACK);
     
    // ビューポートの設定
    renderer.set_ViewPort(0, 0, m_ScreenWidth, m_ScreenHeight);

    // 輝度抽出用レンダリングターゲットに変更
    renderer.RegisterRenderTarget(m_pLuminance_RT->get_RTV(), m_pLuminance_RT->get_DSV());
    renderer.ClearRenderTargetView(m_pLuminance_RT);

    // 輝度スプライト
    m_pLuminance_Sprite->Draw(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();


    // ************************************************************************
    // 
    // 取り出した輝度にブラーを掛けて、ブルーム効果にする
    // 輝度抽出テクスチャにダウンサンプリングしたガウスブラーを掛ける
    //
    // ************************************************************************
    m_pBloomGaussianBlur[0].ExcuteOnGPU(renderer, 8.0f);
    m_pBloomGaussianBlur[1].ExcuteOnGPU(renderer, 8.0f);
    m_pBloomGaussianBlur[2].ExcuteOnGPU(renderer, 8.0f);
    m_pBloomGaussianBlur[3].ExcuteOnGPU(renderer, 8.0f);

    // 加算モード
    Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::ADD);

    // ブルーム合成用レンダリングターゲットに変更
    // ビューポートの設定
    renderer.set_ViewPort(0, 0, m_ScreenWidth, m_ScreenHeight);
    renderer.RegisterRenderTarget(m_pSceneFinal_RT->get_RTV(), nullptr);    // 深度テストなし

    // ブルームスプライト
    m_pBloom_Sprite->Draw(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();

    Master::m_pBlendManager->DeviceToSetBlendState(BLEND_MODE::NONE);
}


//*---------------------------------------------------------------------------------------
//*【?】フレームバッファにコピーするパス
//*
//* [引数]
//* renderer : 描画エンジンの参照
//* [返値] なし
//*----------------------------------------------------------------------------------------
void RenderPipeline::CopyToFrameBuffer_PathRender(RendererEngine &renderer)
{
    // レンダリングターゲットをフレームバッファに変更
    renderer.ChangeRenderTargetFrameBuffer();

    // ビューポートの設定
    renderer.set_ViewPort(0, 0, m_ScreenWidth, m_ScreenHeight);

    // トーンマッピングを適用する
    m_pFinalSceneToneMappingFilter_Sprite->Draw(renderer);
}




//*---------------------------------------------------------------------------------------
//*【?】レンダーターゲットの作成
//*
//* [引数]
//* renderer : 描画エンジンの参照
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool RenderPipeline::CreateRenderTargets(RendererEngine &renderer)
{
    bool result = true;

    //========================================================================================
    //
    //
    /* レンダリングターゲット作成 */
    //
    //
    //========================================================================================
    // ****************************************************************
    // アルベド
    // ****************************************************************
    m_pAlbedo_RT = new DX_RenderTarget();
    result = m_pAlbedo_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_UNKNOWN
    );
    if (result == false)return false;

    // ****************************************************************
    // 法線
    // ****************************************************************
    m_pNormal_RT = new DX_RenderTarget();
    result = m_pNormal_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_UNKNOWN
    );
    if (result == false)return false;

    // ****************************************************************
    // スペキュラ
    // ****************************************************************
    m_pSpecular_RT = new DX_RenderTarget();
    result = m_pSpecular_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_UNKNOWN
    );
    if (result == false)return false;

    // ****************************************************************
    // エミッシブ
    // ****************************************************************
    m_pEmissive_RT = new DX_RenderTarget();
    result = m_pEmissive_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R16G16B16A16_FLOAT,
        DXGI_FORMAT_UNKNOWN
    );
    if (result == false)return false;

    // ****************************************************************
    // デプス
    // ****************************************************************
    m_pDepth_RT = new DX_RenderTarget();
    result = m_pDepth_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_UNKNOWN,
        DXGI_FORMAT_D32_FLOAT
    );
    if (result == false)return false;

    // ****************************************************************
    // シーン最終合成用
    // ****************************************************************
    m_pSceneFinal_RT = new DX_RenderTarget();
    result = m_pSceneFinal_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_UNKNOWN
    );
    if (result == false)return false;

    // ****************************************************************
    // 輝度抽出用
    // ****************************************************************
    m_pLuminance_RT = new DX_RenderTarget();
    result = m_pLuminance_RT->Create(
        renderer,
        renderer.get_ScreenWidth(),     // シーンと同じに
        renderer.get_ScreenHeight(),
        1,
        1,
        DXGI_FORMAT_R11G11B10_FLOAT,    // 輝度はそこまで精度を気にする必要なさそう（多分）
        DXGI_FORMAT_UNKNOWN
    );
    if (result == false)return false;

    // ****************************************************************
    // LVPからの深度値書き込みシャドウマップ（VSM）
    // ****************************************************************
    m_pShadowMap_RT = new DX_RenderTarget();
    result = m_pShadowMap_RT->Create(
        renderer,
        // 影の品質は何も対策しなければ解像度依存
        2048,       
        2048,
        1,
        1,
        // ・Rにライトから見た深度値 ・Gにライトから見た深度値の二乗をいれる
        DXGI_FORMAT_R32G32_FLOAT,   
        DXGI_FORMAT_D32_FLOAT
    );
    if (result == false)return false;

    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】ポストエフェクトの作成
//*
//* [引数]
//* renderer : 描画エンジンの参照
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool RenderPipeline::CreatePostEffect(RendererEngine &renderer)
{
    bool result = true;

    /*************************************************************************
    * 輝度抽出テクスチャにダウンサンプリングしたガウスブラーを掛ける
    *************************************************************************/
    m_pBloomGaussianBlur = new GaussianBlur[BLUR_COUNT]();

    result = m_pBloomGaussianBlur[0].Setup(
        renderer,
        Master::m_pResourceManager->Convert_SRVToTexture(
            "RT_Luminance",
            m_pLuminance_RT->get_SRV_ComPtr(),
            m_pLuminance_RT->get_Width(),
            m_pLuminance_RT->get_Height()),
            DXGI_FORMAT_R11G11B10_FLOAT,
        0
    );
    if (!result)return false;

    // 2 / 1
    result = m_pBloomGaussianBlur[1].Setup(
        renderer,
        Master::m_pResourceManager->Convert_SRVToTexture(
            "DownBlur1",
            m_pBloomGaussianBlur[0].get_AfterBlurTexture(),
            m_pLuminance_RT->get_Width() / 2,
            m_pLuminance_RT->get_Height() / 2),
            DXGI_FORMAT_R11G11B10_FLOAT,
        1
    );
    if (!result)return false;

    // 4 / 1
    result = m_pBloomGaussianBlur[2].Setup(
        renderer,
        Master::m_pResourceManager->Convert_SRVToTexture(
            "DownBlur2",
            m_pBloomGaussianBlur[1].get_AfterBlurTexture(),
            m_pLuminance_RT->get_Width() / 4,
            m_pLuminance_RT->get_Height() / 4),
            DXGI_FORMAT_R11G11B10_FLOAT,
        2
    );
    if (!result)return false;

    // 8 / 1
    result = m_pBloomGaussianBlur[3].Setup(
        renderer,
        Master::m_pResourceManager->Convert_SRVToTexture(
            "DownBlur3",
            m_pBloomGaussianBlur[2].get_AfterBlurTexture(),
            m_pLuminance_RT->get_Width() / 8,
            m_pLuminance_RT->get_Height() / 8),
            DXGI_FORMAT_R11G11B10_FLOAT,
        3
    );
    if (!result)return false;



    /*************************************************************************
    * 被写界深度用ブラーの作成
    *************************************************************************/
    m_pDoF_GaussianBlur = new GaussianBlur();
    result = m_pDoF_GaussianBlur->Setup(renderer,
        Master::m_pResourceManager->Convert_SRVToTexture(
            "RT_SceneFinal",
            m_pSceneFinal_RT->get_SRV_ComPtr(),
            m_pSceneFinal_RT->get_Width(),
            m_pSceneFinal_RT->get_Height()
        ),
            DXGI_FORMAT_R11G11B10_FLOAT,
        4
    );
    if (!result)return false;
    
    /*************************************************************************
    * シャドウのVSM用ブラーの作成
    *************************************************************************/
    m_pShadowGaussianBlur = new GaussianBlur();
    result = m_pShadowGaussianBlur->Setup(renderer,
        Master::m_pResourceManager->Convert_SRVToTexture(
            "ShadowMap",
            m_pShadowMap_RT->get_SRV_ComPtr(),
            m_pShadowMap_RT->get_Width(),
            m_pShadowMap_RT->get_Height()
        ),
            DXGI_FORMAT_R32G32_FLOAT,
        5
    );
    if (!result)return false;

    // シャドウマップへブラーを掛ける
   // m_pShadowGaussianBlur->ExcuteOnGPU(renderer, 4.0f);

    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】レンダーターゲット用スプライトの作成
//*
//* [引数]
//* renderer : 描画エンジンの参照
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool RenderPipeline::CreateRenderTargetSprites(RendererEngine &renderer)
{
    bool result = true;

    //========================================================================================
    //
    //
    /* レンダリングターゲット用スプライト */
    //
    //
    //========================================================================================
    /*************************************************************************
    * アルベド用
    *************************************************************************/
    CreateSpriteInfo sprite;
    sprite.pRenderer = &renderer;
    sprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    sprite.ShaderType = SHADER_TYPE::FORWARD_UNLIT_UI_SPRITE;
    sprite.IsActive = false;    // ２重更新されてしまうのでobjマネージャ側では何もしないように
    sprite.ObjTag = "GBuffer_A_Sprite";
    sprite.Width = 1.0f;
    sprite.Height = 1.0f;
    sprite.pTextureMap[0] = Master::m_pResourceManager->Convert_SRVToTexture("GBuffer_A", m_pAlbedo_RT->get_SRV_ComPtr(), m_pAlbedo_RT->get_Width(), m_pAlbedo_RT->get_Height());
    
    // スプライト取得
    m_pAlbed_Sprite = MeshFactory::CreateSprite(sprite)->get_Component<SpriteRenderer>();
    
    sprite.pTextureMap.clear();

    /*************************************************************************
    * 法線用
    *************************************************************************/
    sprite.ObjTag = "GBuffer_B_Sprite";
    sprite.Width = 1.0f;
    sprite.Height = 1.0f;
    sprite.pTextureMap[0] = Master::m_pResourceManager->Convert_SRVToTexture("GBuffer_B", m_pNormal_RT->get_SRV_ComPtr(), m_pNormal_RT->get_Width(), m_pNormal_RT->get_Height());
    
    // スプライト取得
    m_pNormal_Sprite = MeshFactory::CreateSprite(sprite)->get_Component<SpriteRenderer>();

    sprite.pTextureMap.clear();

    /*************************************************************************
    * スペキュラ用
    *************************************************************************/
    sprite.ObjTag = "GBuffer_C_Sprite";
    sprite.pTextureMap[0] = Master::m_pResourceManager->Convert_SRVToTexture("GBuffer_C", m_pSpecular_RT->get_SRV_ComPtr(), m_pSpecular_RT->get_Width(), m_pSpecular_RT->get_Height());
    
    // スプライト取得
    m_pSpecular_Sprite = MeshFactory::CreateSprite(sprite)->get_Component<SpriteRenderer>();

    sprite.pTextureMap.clear();

    /*************************************************************************
    * エミッシブ用
    *************************************************************************/
    sprite.ObjTag = "GBuffer_D_Sprite";
    sprite.pTextureMap[0] = Master::m_pResourceManager->Convert_SRVToTexture("GBuffer_D", m_pEmissive_RT->get_SRV_ComPtr(), m_pSpecular_RT->get_Width(), m_pSpecular_RT->get_Height());
    
    // スプライト取得
    m_pEmissive_Sprite = MeshFactory::CreateSprite(sprite)->get_Component<SpriteRenderer>();

    sprite.pTextureMap.clear();

    /*************************************************************************
    * Z値用
    *************************************************************************/
    sprite.ObjTag = "Depth_Sprite";
    sprite.pTextureMap[0] = Master::m_pResourceManager->Convert_SRVToTexture("Depth", m_pDepth_RT->get_DepthSRV_ComPtr(), m_pDepth_RT->get_Width(), m_pDepth_RT->get_Height());
    
    // スプライト取得
    m_pDepth_Sprite = MeshFactory::CreateSprite(sprite)->get_Component<SpriteRenderer>();

    sprite.pTextureMap.clear();

    /*************************************************************************
    * ライティングパス出力用
    *************************************************************************/
    sprite.ObjTag = "DefferdLightingSprite";
    sprite.Width = 1.0f;
    sprite.Height = 1.0f;
    sprite.PSConstBufferNum = 1;
    sprite.pPSConstantBuffers =  new ExpandConstantBufferInfo();
    sprite.pPSConstantBuffers->SetSlot = 9;
    sprite.pPSConstantBuffers->UserExpandConstantBufferSize = sizeof(m_ShadowData);
    sprite.pPSConstantBuffers->pUserExpandConstantBuffer = &m_ShadowData;

    sprite.pTextureMap[0] = Master::m_pResourceManager->Convert_SRVToTexture("GBuffer_A");
    sprite.pTextureMap[1] = Master::m_pResourceManager->Convert_SRVToTexture("GBuffer_B");
    sprite.pTextureMap[2] = Master::m_pResourceManager->Convert_SRVToTexture("GBuffer_C");
    sprite.pTextureMap[3] = Master::m_pResourceManager->Convert_SRVToTexture("GBuffer_D");
    sprite.pTextureMap[4] = Master::m_pResourceManager->Convert_SRVToTexture("Depth");
    sprite.pTextureMap[5] = Master::m_pResourceManager->Convert_SRVToTexture(
        // ブラーを掛けたシャドウマップを設定
        "BlurShadowMap",
        m_pShadowGaussianBlur->get_AfterBlurTexture(),  
        m_pShadowMap_RT->get_Width(),
        m_pShadowMap_RT->get_Height()
    );
    sprite.ShaderType = SHADER_TYPE::DEFERRED_STD_RT_SPRITE;

    // スプライト取得
    m_pDefferdLighting_Sprite = MeshFactory::CreateSprite(sprite)->get_Component<SpriteRenderer>();
    sprite.pTextureMap.clear();

    /*************************************************************************
    * 輝度抽出用スプライト
    *************************************************************************/
    CreateSpriteInfo luminanceSprite;
    luminanceSprite.pRenderer = &renderer;
    luminanceSprite.IsActive = false;
    luminanceSprite.ObjTag = "LuminanceSprite";
    luminanceSprite.Width = 1.0f;
    luminanceSprite.Height = 1.0f;
    luminanceSprite.pTextureMap[0] = Master::m_pResourceManager->Convert_SRVToTexture(
        "RT_SceneFinal",
        m_pSceneFinal_RT->get_SRV_ComPtr(),
        m_pSceneFinal_RT->get_Width(),
        m_pSceneFinal_RT->get_Height()
    );

    // 深度テクスチャ
    luminanceSprite.pTextureMap[1] = Master::m_pResourceManager->Convert_SRVToTexture("Depth");
    luminanceSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    luminanceSprite.ShaderType = SHADER_TYPE::POST_LUMINANCE_FILTER;

    // スプライト取得
    m_pLuminance_Sprite = MeshFactory::CreateSprite(luminanceSprite)->get_Component<SpriteRenderer>();

    /*************************************************************************
    * ブルーム用スプライト
    *************************************************************************/
    CreateSpriteInfo bloomSprite;
    bloomSprite.pRenderer = &renderer;
    bloomSprite.IsActive = false;
    bloomSprite.ObjTag = "bloomSprite";
    bloomSprite.Width = 1.0f;
    bloomSprite.Height = 1.0f;
    bloomSprite.pTextureMap[0] = Master::m_pResourceManager->Convert_SRVToTexture(
        "RT_Luminance"
    );
    bloomSprite.pTextureMap[1] = Master::m_pResourceManager->Convert_SRVToTexture(
        "DownBlur1"
    );
    bloomSprite.pTextureMap[2] = Master::m_pResourceManager->Convert_SRVToTexture(
        "DownBlur2"
    );
    bloomSprite.pTextureMap[3] = Master::m_pResourceManager->Convert_SRVToTexture(
        "DownBlur3"
    );
    bloomSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    bloomSprite.ShaderType = SHADER_TYPE::POST_KAWASE_FILTER;

    // スプライト取得
    m_pBloom_Sprite = MeshFactory::CreateSprite(bloomSprite)->get_Component<SpriteRenderer>();


    /*************************************************************************
    * 被写界深度用スプライト
    *************************************************************************/
    CreateSpriteInfo depthOfFieldSprite;
    depthOfFieldSprite.pRenderer = &renderer;
    depthOfFieldSprite.IsActive = false;
    depthOfFieldSprite.ObjTag = "DepthOfFieldSprite";
    depthOfFieldSprite.Width = 1.0f;
    depthOfFieldSprite.Height = 1.0f;
    depthOfFieldSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    depthOfFieldSprite.ShaderType = SHADER_TYPE::POST_DEPTH_OF_FILED;   // 被写界深度
    depthOfFieldSprite.pTextureMap[0] = 
        Master::m_pResourceManager->Convert_SRVToTexture(
            "DOF_GaussianBlur", 
            m_pDoF_GaussianBlur->get_AfterBlurTexture(), 
            m_pSceneFinal_RT->get_Width(),
            m_pSceneFinal_RT->get_Height()
        );
    // 深度テクスチャ設定
    depthOfFieldSprite.pTextureMap[1] = Master::m_pResourceManager->Convert_SRVToTexture("Depth");

    depthOfFieldSprite.pPSConstantBuffers = new ExpandConstantBufferInfo(); // VS定数バッファにブラー用の重みテーブルをセット
    depthOfFieldSprite.pPSConstantBuffers->SetSlot = 8;               // スロット8にセット
    depthOfFieldSprite.pPSConstantBuffers->pUserExpandConstantBuffer = &m_DofData;
    depthOfFieldSprite.pPSConstantBuffers->UserExpandConstantBufferSize = sizeof(m_DofData);
    depthOfFieldSprite.PSConstBufferNum = 1;

    // スプライト取得
    m_pDoF_Sprite = MeshFactory::CreateSprite(depthOfFieldSprite)->get_Component<SpriteRenderer>();


    /*************************************************************************
    * トーンマッピング用スプライト （これをそのままフレームバッファに表示する）
    *************************************************************************/
    CreateSpriteInfo toneMappingSprite;
    toneMappingSprite.pRenderer = &renderer;
    toneMappingSprite.IsActive = false;
    toneMappingSprite.ObjTag = "ToneMappingSprite";
    toneMappingSprite.Width = 1.0f;
    toneMappingSprite.Height = 1.0f;
    toneMappingSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    toneMappingSprite.ShaderType = SHADER_TYPE::POST_TONEMAPPING;   // トーンマッピング
    toneMappingSprite.pTextureMap[0] = 
        Master::m_pResourceManager->Convert_SRVToTexture("RT_SceneFinal");
    // スプライト取得
    m_pFinalSceneToneMappingFilter_Sprite = MeshFactory::CreateSprite(toneMappingSprite)->get_Component<SpriteRenderer>();

    return true;
}


