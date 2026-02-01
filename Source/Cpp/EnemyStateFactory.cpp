#include "pch.h"
#include "EnemyStateFactory.h"
#include "Component_EnemyController.h"
#include "RendererEngine.h"

// アリ
#include "Ant_IdleState.h"



//*---------------------------------------------------------------------------------------
//*【?】ステートの作成
//*
//* [引数]
//* &_out : 出力先
//* &_createState : 作成する敵ステート
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* なし 
//*----------------------------------------------------------------------------------------
void EnemyStateFactory::Create(StateMachine<EnemyController>& _out, int _createState, RendererEngine& _renderer)
{
	switch (_createState)
	{
	case ENEMY_TYPE_ANT_Normal:
		CreateAntState(_out, _renderer);
		break;
	default:
		break;
	};
}


//*---------------------------------------------------------------------------------------
//*【?】蟻ステートの作成
//*
//* [引数]
//* &_out : 出力先
//* &renderer : 描画エンジンの参照
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
void EnemyStateFactory::CreateAntState(StateMachine<class EnemyController>& _out, RendererEngine& _renderer)
{
	// ルートシーン作成
	std::shared_ptr<Ant_IdleState> pIdle = std::make_shared<Ant_IdleState>();

	//描画インターフェイス設定
	// add_childの中で子シーンにもセットしてるのでaddする前に呼んで！
	pIdle->set_Renderer(&_renderer);

	// ********************************************************************************
	// 子ステート作成
	// ********************************************************************************
	//std::shared_ptr<c_Title_LoadProcess> c_LoadProcess = std::make_shared<c_Title_LoadProcess>();		// ロード処理

	// 親を設定
	//c_LoadProcess->set_Parent(pTitleScene);

	// 子を登録
	//pTitleScene->add_Child(c_TITLE::c_TITLE_LOAD_PROCESS, (c_LoadProcess));

	//ステートマシンに登録
	_out.RegisterState(ANT_STATE::ANT_STATE_IDLE, std::move(pIdle));
}
