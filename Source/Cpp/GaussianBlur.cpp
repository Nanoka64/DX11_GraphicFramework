#include "pch.h"
#include "GaussianBlur.h"
#include "Texture.h"
#include "MeshFactory.h"
#include "ResourceManager.h"
#include "DX_RenderTarget.h"
#include "RendererEngine.h"
#include "Component_SpriteRenderer.h"
#include "GameObject.h"


/// <summary>
/// ※ 魔導書にあった関数
/// ガウシアン関数を利用して重みテーブルを計算する
/// </summary>
/// <param name="weightsTbl">重みテーブルの記録先</param>
/// <param name="sizeOfWeightsTbl">重みテーブルのサイズ</param>
/// <param name="sigma">分散具合。この数値が大きくなると分散具合が強くなる</param>
void CalcWeightsTableFromGaussian(float* weightsTbl, int sizeOfWeightsTbl, float sigma)
{
    if (sigma <= 0.0f)
    {
        return;
    }

    // 重みの合計を記録する変数を定義する
    float total = 0;

    // ここからガウス関数を用いて重みを計算している
    // ループ変数のxが基準テクセルからの距離
    for (int x = 0; x < sizeOfWeightsTbl; x++)
    {
        weightsTbl[x] = expf(-0.5f * (float)(x * x) / sigma);
        total += 2.0f * weightsTbl[x];
    }

    // 重みの合計で除算することで、重みの合計を1にしている
    for (int i = 0; i < sizeOfWeightsTbl; i++)
    {
        weightsTbl[i] /= total;
    }
}



GaussianBlur::GaussianBlur()
{}

GaussianBlur::~GaussianBlur()
{}

bool GaussianBlur::Setup(RendererEngine& renderer, std::shared_ptr<Texture> pTex, int id)
{
    if (pTex == nullptr){
        return false;
    }
    m_Id = id;
	m_pTexture = pTex;

    // RTの初期化
	if (!InitRenderTargets(renderer)) {
		return false;
	}

    // ガウシアンブラー用の重みテーブルを計算する
    CalcWeightsTableFromGaussian(m_weights, NUM_WEIGHTS, 4.0f);

    // スプライトの初期化
	if (!InitSprites(renderer)){
		return false;
	}

    return true;
}

void GaussianBlur::ExcuteOnGPU(RendererEngine& renderer, float blurPow)
{
    // ガウシアンブラー用の重みテーブルを計算する
    CalcWeightsTableFromGaussian(m_weights, NUM_WEIGHTS, blurPow);

    Master::m_pDebugger->BeginDebugWindow("PostEffect");
    Master::m_pDebugger->DG_BulletText("Horizontal");
    Master::m_pDebugger->DG_Image(m_pHorizontalBlur->get_SRV(), VECTOR2::VEC2(600, 300));
    Master::m_pDebugger->DG_BulletText("Vertical");
    Master::m_pDebugger->DG_Image(m_pVerticalBlur->get_SRV(),VECTOR2::VEC2(600,300));
    Master::m_pDebugger->EndDebugWindow();
    // ************************************************************************
    // 
    // 水平ブラー
    // 
    // ************************************************************************
    // ビューポートの設定
    renderer.set_ViewPort(0, 0, m_pTexture.lock()->get_Width() / 2.0f, m_pTexture.lock()->get_Height());

    // 水平ブラー用レンダリングターゲットに変更
    renderer.RegisterRenderTarget(m_pHorizontalBlur->get_RTV(), nullptr);
    renderer.ClearRenderTargetView(m_pHorizontalBlur);
    // 水平ブラー
    auto horizontalBlurSprite = m_pHorizontalBlurSprite.lock()->get_Component<SpriteRenderer>();
    horizontalBlurSprite->setToGPU_ExtendUserPS_CBuffer(renderer, 0, &m_weights);
    horizontalBlurSprite->Draw(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();

    // ************************************************************************
    // 
    // 垂直ブラー
    // 
    // ************************************************************************
    // ビューポートの設定
    renderer.set_ViewPort(0, 0, m_pTexture.lock()->get_Width() / 2.0f, m_pTexture.lock()->get_Height() / 2.0f);
    // 垂直ブラー用レンダリングターゲットに変更
    renderer.RegisterRenderTarget(m_pVerticalBlur->get_RTV(), nullptr);
    renderer.ClearRenderTargetView(m_pVerticalBlur);

    // 垂直ブラー
    auto verticalBlurSprite = m_pVerticalBlurSprite.lock()->get_Component<SpriteRenderer>();
    verticalBlurSprite->setToGPU_ExtendUserPS_CBuffer(renderer, 0, &m_weights);
    verticalBlurSprite->Draw(renderer);

    // レンダリングターゲット解除
    renderer.ReleaseRenderTargetSetNull();
}

void GaussianBlur::Term()
{
    SAFE_DELETE(m_pHorizontalBlur);
    SAFE_DELETE(m_pVerticalBlur);
}

bool GaussianBlur::InitRenderTargets(RendererEngine& renderer)
{
    bool result = true;

    // ****************************************************************
    // 水平ブラー
    // ****************************************************************
    m_pHorizontalBlur = new DX_RenderTarget();
    result = m_pHorizontalBlur->Create(
        renderer,
        m_pTexture.lock()->get_Width() / 2.0f,
        m_pTexture.lock()->get_Height(),
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_D32_FLOAT
    );
    if (result == false)return false;

    // ****************************************************************
    // 垂直ブラー
    // ****************************************************************
    m_pVerticalBlur = new DX_RenderTarget();
    result = m_pVerticalBlur->Create(
        renderer,
        m_pTexture.lock()->get_Width() / 2.0f,
        m_pTexture.lock()->get_Height() / 2.0f,
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_D32_FLOAT
    );
    if (result == false)return false;

    return result;
}

bool GaussianBlur::InitSprites(RendererEngine& renderer)
{
    /*************************************
    * 水平ブラー用スプライト
    *************************************/
    CreateSpriteInfo horizontalBlurSprite;
    horizontalBlurSprite.pRenderer = &renderer;
    horizontalBlurSprite.pPSConstantBuffers = new ConstantBufferInfo(); // VS定数バッファにブラー用の重みテーブルをセット
    horizontalBlurSprite.pPSConstantBuffers->SetSlot = 7;               // スロット7にセット
    horizontalBlurSprite.pPSConstantBuffers->pUserExpandConstantBuffer = &m_weights;
    horizontalBlurSprite.pPSConstantBuffers->UserExpandConstantBufferSize = sizeof(m_weights);
    horizontalBlurSprite.PSConstBufferNum = 1;
    horizontalBlurSprite.IsActive = false;
    horizontalBlurSprite.ObjTag = "HorizontalBlurSprite" + std::to_string(m_Id);
    horizontalBlurSprite.Width = 1.0f;      // サイズの変更はRTだけでいい
    horizontalBlurSprite.Height = 1.0f;
    horizontalBlurSprite.pTextureMap[0] = m_pTexture;
    horizontalBlurSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    horizontalBlurSprite.ShaderType = SHADER_TYPE::POST_GAUSSIAN_BLUR_HORIZONTAL;
    m_pHorizontalBlurSprite = MeshFactory::CreateSprite(horizontalBlurSprite);


    /*************************************
    * 垂直ブラー用スプライト
    *************************************/
    CreateSpriteInfo verticalBlurSprite;
    verticalBlurSprite.pRenderer = &renderer;
    verticalBlurSprite.pPSConstantBuffers = new ConstantBufferInfo();   // VS定数バッファにブラー用の重みテーブルをセット
    verticalBlurSprite.pPSConstantBuffers->SetSlot = 7;                 // スロット7にセット
    verticalBlurSprite.pPSConstantBuffers->pUserExpandConstantBuffer = &m_weights;
    verticalBlurSprite.pPSConstantBuffers->UserExpandConstantBufferSize = sizeof(m_weights);
    verticalBlurSprite.PSConstBufferNum = 1;
    verticalBlurSprite.IsActive = false;
    verticalBlurSprite.ObjTag = "VerticalBlurSprite" + std::to_string(m_Id);
    verticalBlurSprite.Width = 1.0f;
    verticalBlurSprite.Height = 1.0f;
    verticalBlurSprite.pTextureMap[0] = ResourceManager::Instance().Convert_SRVToTexture("RT_HorizontalBlur" + std::to_string(m_Id), m_pHorizontalBlur->get_SRV_ComPtr());
    verticalBlurSprite.Type = SPRITE_USAGE_TYPE::RENDER_TARGET;
    verticalBlurSprite.ShaderType = SHADER_TYPE::POST_GAUSSIAN_BLUR_VERTICAL;
    m_pVerticalBlurSprite = MeshFactory::CreateSprite(verticalBlurSprite);

    return true;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GaussianBlur::get_AfterBlurTexture()const
{
    return m_pVerticalBlur->get_SRV_ComPtr();
}
