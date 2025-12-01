#include "pch.h"
#include "RootSceneState.h"

//*---------------------------------------------------------------------------------------
//* @:RootSceneState Class 
//*【?】描画
//* 引数：1.RendererManager
//* 返値：int 現在のシーン状態
//*----------------------------------------------------------------------------------------
int RootSceneState::Update(RendererEngine &renderer)
{
    // シーンの更新
    int newState = m_ChildSceneStateMap[m_CrntChildSceneState]->Update(renderer);

    // シーン状態が変わったら現在のシーンを終了し、新しいシーンを呼ぶ
    if (newState != m_CrntChildSceneState)
    {
        m_ChildSceneStateMap[m_CrntChildSceneState]->OnExit(renderer);
        m_CrntChildSceneState = newState;
        m_ChildSceneStateMap[m_CrntChildSceneState]->OnEnter(renderer);
    }
    return 0;
}


//*---------------------------------------------------------------------------------------
//* @:RootSceneState Class 
//*【?】描画
//* 引数：1.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void RootSceneState::Draw(RendererEngine &renderer)
{
    m_ChildSceneStateMap[m_CrntChildSceneState]->Draw(renderer);
}


//*---------------------------------------------------------------------------------------
//* @:RootSceneState Class 
//*【?】子供シーン要素の登録
//* 引数：1.id
//* 引数：2.登録する子シーン共有ポインタ
//* 返値：void
//*----------------------------------------------------------------------------------------
void RootSceneState::RegisterChildScene(const int id, std::shared_ptr<IChildSceneState>pScene)
{
    m_ChildSceneStateMap[id] = pScene;
}


//*---------------------------------------------------------------------------------------
//* @:RootSceneState Class 
//*【?】終了
//* 引数：1.id
//* 引数：2.RendererManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void RootSceneState::SetCurrentChildScene(const int id, RendererEngine &renderer)
{
    m_CrntChildSceneState = id;
    m_ChildSceneStateMap[m_CrntChildSceneState]->OnEnter(renderer);
}

