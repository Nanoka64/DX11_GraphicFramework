#include "pch.h"
#include "BlendManager.h"
#include "IDX_BlendState.h"
#include "RendererEngine.h"

//*---------------------------------------------------------------------------------------
//* @:BlendManager Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
BlendManager::BlendManager():
	m_pContext(nullptr)
{

}

//*---------------------------------------------------------------------------------------
//* @:BlendManager Class 
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
BlendManager::~BlendManager()
{

}

//*---------------------------------------------------------------------------------------
//* @:BlendManager Class 
//*【?】初期化
//* 引数：1.デバイスの共有ポインタ
//* 返値：成功したか
//*----------------------------------------------------------------------------------------
bool BlendManager::Init(std::shared_ptr<RendererEngine> renderer)
{
    m_pRenderer = renderer;
	m_pContext = renderer->get_DeviceContext();

	// 作成
	for (int i = 0; i < (int)BLEND_MODE::NUM; i++)
	{
		std::unique_ptr<IDX_BlendState> pBlend = std::make_unique<IDX_BlendState>();
	
		m_pBlendStateArray[i] = std::move(BlendStateFactory((BLEND_MODE)i));
	}
    return true;
}

void BlendManager::Term()
{
}

//*---------------------------------------------------------------------------------------
//* @:BlendManager Class 
//*【?】ブレンドステートを設定する
//* 引数：1.反映させるレンダリングターゲット番号
//* 引数：2.ブレンド種別
//* 返値：なし
//*----------------------------------------------------------------------------------------
void BlendManager::DeviceToSetBlendState( BLEND_MODE type)
{
	// OMにブレンドステートオブジェクトを設定
	FLOAT BlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	m_pContext->OMSetBlendState(m_pBlendStateArray[(int)type]->get(), BlendFactor, 0xffffffff);
}


//*---------------------------------------------------------------------------------------
//* @:BlendManager Class 
//*【?】レンダリングターゲットごとブレンドを設定する
//* 引数：1.反映させるレンダリングターゲット番号
//* 引数：2.ブレンド種別
//* 返値：なし
//*----------------------------------------------------------------------------------------
void BlendManager::DeviceToSetIndependentBlendState(int rt_No, BLEND_MODE type)
{

}



//*---------------------------------------------------------------------------------------
//* @:BlendManager Class 
//*【?】デバイスコンテキストにブレンドステートを設定する
//* 引数：1.ブレンド種別
//* 引数：2.出力先
//* 返値：生成したステートのユニークポインタ
//*----------------------------------------------------------------------------------------
std::unique_ptr<IDX_BlendState>  BlendManager::BlendStateFactory(BLEND_MODE type)
{
	BlendSetupDataInfo info{};

	// 以下はデフォルトとして指定しているが、種別ごとにswichの中で書き換える
	info.alphaToCoverageEnable						= FALSE;
	info.independentBlendEnable						= FALSE;
	info.renderTargets[0].blendOpAlpha = D3D11_BLEND_OP_ADD;

	// 色と同じく、アルファも通常の source-over 合成にする。
	// 以前は描画元のアルファで出力先を上書きしていたため、画面表示だけなら問題が見えなくても、
	// デバッグエディタで完成済みバックバッファをテクスチャとして再描画すると二重に透過されていた。
	// outAlpha = srcAlpha + destAlpha * (1 - srcAlpha) とすることで、背景まで描画済みの画面は
	// 常に不透明なまま保たれ、半透明UIの色だけが一度正しく合成される。
	info.renderTargets[0].destBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	info.renderTargets[0].srcBlendAlpha = D3D11_BLEND_ONE;
	info.renderTargets[0].renderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	switch (type)
	{
	case BLEND_MODE::NONE:	/* 何もしない ====================*/ 
	{
		info.renderTargets[0].blendEnable			= FALSE;
		break;
	}
	case BLEND_MODE::ALPHA:	/* アルファブレンド ====================*/
	{
		info.alphaToCoverageEnable					= FALSE;
		info.renderTargets[0].blendEnable			= TRUE;
		info.renderTargets[0].blendOp				= D3D11_BLEND_OP_ADD;
		info.renderTargets[0].destBlend				= D3D11_BLEND_INV_SRC_ALPHA;
		info.renderTargets[0].srcBlend				= D3D11_BLEND_SRC_ALPHA;
		break;
	}
	case BLEND_MODE::ALPHA_TRANS:	/* アルファ合成ブレンド ====================*/
	{
		info.alphaToCoverageEnable					= FALSE;
		info.renderTargets[0].blendEnable			= TRUE;
		info.renderTargets[0].blendOp				= D3D11_BLEND_OP_ADD;
		info.renderTargets[0].destBlend				= D3D11_BLEND_INV_SRC_ALPHA;
		info.renderTargets[0].srcBlend				= D3D11_BLEND_SRC_ALPHA;
		break;
	}
	case BLEND_MODE::ADD:	/* 加算合成 ====================*/
	{
		info.alphaToCoverageEnable					= FALSE;
		info.renderTargets[0].blendEnable			= TRUE;
		info.renderTargets[0].blendOp				= D3D11_BLEND_OP_ADD;
		info.renderTargets[0].destBlend				= D3D11_BLEND_ONE;
		info.renderTargets[0].srcBlend				= D3D11_BLEND_SRC_ALPHA;
		break;
	}
	case BLEND_MODE::SUB:	/* 減算合成 ====================*/
	{
		info.alphaToCoverageEnable					= FALSE;
		info.renderTargets[0].blendEnable			= TRUE;
		info.renderTargets[0].blendOp				= D3D11_BLEND_OP_REV_SUBTRACT;
		info.renderTargets[0].destBlend				= D3D11_BLEND_ONE;
		info.renderTargets[0].srcBlend				= D3D11_BLEND_SRC_ALPHA;
		break;
	}
	default:
		MessageBoxA(NULL, "ブレンドステートの種別が見つかりません", "Error", MB_OK);
		assert(false);
		return {};
	}

	std::unique_ptr<IDX_BlendState> pOut = std::make_unique<IDX_BlendState>();

	// 作成
	if (pOut->Create(*m_pRenderer.lock(), info) == false)
	{
		MessageBoxA(NULL, "ブレンドステートの作成ができませんでした", "Error", MB_OK);
		assert(false);
		return {};
	}

	// 正常終了
	return pOut;
}
