#include "pch.h"
#include "EffectManager.h"
#include "RendererEngine.h"
//#include <Effekseer.h>


EffectManager::EffectManager()
{
}

EffectManager::~EffectManager()
{

}

bool EffectManager::Setup(RendererEngine& renderer)
{
    //auto pDevice = renderer.get_Device();
    //auto pContext = renderer.get_DeviceContext();
    //UINT screenW = renderer.get_ScreenWidth();
    //UINT screenH = renderer.get_ScreenHeight();

    //// エフェクトのマネージャーの作成
    //m_EfkManager = ::Effekseer::Manager::Create(8000);

    //// Create a  graphics device
    //// 描画デバイスの作成
    //::Effekseer::Backend::GraphicsDeviceRef graphicsDevice;
    //graphicsDevice = ::EffekseerRendererDX11::CreateGraphicsDevice(pDevice, pContext);

    //// Create a renderer of effects
    //// エフェクトのレンダラーの作成
    //::EffekseerRenderer::RendererRef efkRenderer;
    //efkRenderer = ::EffekseerRendererDX11::Renderer::Create(graphicsDevice, 8000);

    //// Specify rendering modules
    //// 描画モジュールの設定
    //m_EfkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
    //m_EfkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
    //m_EfkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
    //m_EfkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
    //m_EfkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

    //// Specify a texture, model, curve and material loader
    //// It can be extended by yourself. It is loaded from a file on now.
    //// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
    //// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
    //m_EfkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
    //m_EfkManager->SetModelLoader(efkRenderer->CreateModelLoader());
    //m_EfkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
    //m_EfkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

    //// Setup the coordinate system. This must be matched with your application.
    //// 座標系を設定する。アプリケーションと一致させる必要がある。
    //m_EfkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);

    //// Specify a position of view
    //// 視点位置を確定
    //::Effekseer::Vector3D viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

    //// Specify a projection matrix
    //// 投影行列を設定
    //::Effekseer::Matrix44 projectionMatrix;
    //projectionMatrix.PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)screenW / (float)screenH, 1.0f, 500.0f);

    //// Specify a camera matrix
    //// カメラ行列を設定
    //::Effekseer::Matrix44 cameraMatrix;
    //cameraMatrix.LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));

    //// Load an effect
    //// エフェクトの読込
    //m_EffectMap["Laser01"] = Effekseer::Effect::Create(m_EfkManager, u"Resource/Effect/Laser01.efkefc");

    return true;
}


void EffectManager::Update(RendererEngine& renderer)
{
    //// Handle for the effect instance. Used to control the played effect.
    //// エフェクトのインスタンスのハンドル。再生したエフェクトの制御に使う。
    //Effekseer::Handle efkHandle = 0;
    //// Elapsed frames
    //// 経過したフレーム数のカウンタ
    //static int32_t time = 0;


    //// Stop and play effects every 120 frames(for sample)
    //// 120フレームごとに、エフェクトを停止、再生する(サンプル用)
    //if (time % 120 == 0)
    //{
    //    // Play an effect
    //    // エフェクトの再生
    //    efkHandle = m_EfkManager->Play(m_EffectMap["Laser01"], 0, 0, 0);
    //}

    //if (time % 120 == 119)
    //{
    //    // Stop effects
    //    // エフェクトの停止
    //    m_EfkManager->StopEffect(efkHandle);
    //}

    //// Move the effect
    //// エフェクトの移動
    //m_EfkManager->AddLocation(efkHandle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));

    //time++;
}
