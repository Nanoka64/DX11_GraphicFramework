#include "pch.h"
#include "GameObject.h"

//*---------------------------------------------------------------------------------------
//* @:GameObject3D Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
GameObject::GameObject():
	m_IsCalcUpdate(false)
{

}

//*---------------------------------------------------------------------------------------
//* @:GameObject3D Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
GameObject::~GameObject()
{

}


//*---------------------------------------------------------------------------------------
//* @:Object Class 
//*【?】コンポーネントの更新
//* 引数：描画エンジンの参照
//* 戻値：void
//*----------------------------------------------------------------------------------------
void GameObject::ComponentUpdate(RendererEngine& renderer)
{
	// 初めての更新の場合はコンポーネントのStartメソッドを呼ぶようにする
	if (m_IsCalcUpdate == false)
	{
		for (auto &comp : m_pComponentList)
		{
			comp.get()->Start(renderer);
		}
	}

	for (auto& comp : m_pComponentList)
	{
		comp.get()->Update(renderer);
	}

	m_IsCalcUpdate = true;
}

//*---------------------------------------------------------------------------------------
//* @:Object Class 
//*【?】コンポーネントの更新後の更新
//* 引数：描画エンジンの参照
//* 戻値：void
//*----------------------------------------------------------------------------------------
void GameObject::ComponentLateUpdate(RendererEngine& renderer)
{
	for (auto& comp : m_pComponentList)
	{
		comp.get()->LateUpdate(renderer);
	}
}


//*---------------------------------------------------------------------------------------
//* @:Object Class 
//*【?】コンポーネントの描画
//* 引数：描画エンジンの参照
//* 戻値：void
//*----------------------------------------------------------------------------------------
void GameObject::ComponentRender(RendererEngine& renderer)
{
	for (auto& comp : m_pComponentList)
	{
		comp.get()->Draw(renderer);
	}
}


//*---------------------------------------------------------------------------------------
//* @:Object Class 
//*【?】Transformの取得
//* 引数：なし
//* 戻値：弱参照ポインタ
//*----------------------------------------------------------------------------------------
std::weak_ptr<Transform> GameObject::get_Transform() const
{
	return m_pTransform;
}

//*---------------------------------------------------------------------------------------
//* @:Object Class 
//*【?】Transformの取得
//* 引数：なし
//* 戻値：弱参照ポインタ
//*----------------------------------------------------------------------------------------
std::vector<std::shared_ptr<IComponent>> GameObject::get_ComponentList()const
{
	return m_pComponentList;
}
