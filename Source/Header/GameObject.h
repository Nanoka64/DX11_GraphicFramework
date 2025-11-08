#pragma once
#include "Object.h"
#include "Component_Transform.h"

class IComponent;

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:GameObject3D Class --- */
//
//  ★継承：Transform ← Object ★
//
// 【?】ゲームにに存在するもの全ての基底クラス
//
//	※enable_shared_from_thisについて※
//		コンポーネントに対してオーナーであるthisを渡す際に、
//		そのままmake_sharedと渡すことはできない。
//		なので、安全に取得するにはenable_shared_from_thisを継承させ、
//		shared_from_this()メソッドを使用して渡すことによって解決できるらしい。
//		コンストラクタが呼ばれた後に適用されるため、コンストラクタ内でコンポーネントの追加は出来ない。
//
// ***************************************************************************************
class GameObject : public std::enable_shared_from_this<GameObject> , public Object
{
private:	
	/* オブジェクトマネージャをフレンドとして登録 */
	friend class GameObjectManager;
    
	std::vector<std::shared_ptr<IComponent>> m_pComponentList;	// コンポーネントリスト
    std::shared_ptr<Transform> m_pTransform;	// トランスフォームコンポーネントはデフォルトで持つ


public:
    GameObject();
    virtual ~GameObject();

	virtual bool Init(RendererManager& renderer) { return true; };
	virtual void Update(RendererManager& renderer) {};
	virtual void Draw(RendererManager& renderer) {};
	virtual bool Term(RendererManager& renderer) { return true; };


	// ****************************************************************************************************************************************
	/* コンポーネント関連 */
	// ****************************************************************************************************************************************

	/// <summary>
	/// コンポーネントの更新
	/// </summary>
	void ComponentUpdate(RendererManager& renderer);

	/// <summary>
	/// コンポーネントの描画
	/// </summary>
	/// <param name="renderer"></param>
	void ComponentRender(RendererManager& renderer);

	/// <summary>
	/// Transformの取得
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<Transform> get_Transform()const;

	/// <summary>
	/// コンポーネントの取得
	/// </summary>
	/// <typeparam name="T">取得するコンポーネントの種類</typeparam>
	/// <returns></returns>
	template<typename T>
	std::shared_ptr<T> get_Component() const
	{
		for (auto& comp : m_pComponentList)
		{
			std::shared_ptr<T> resComp = std::dynamic_pointer_cast<T>(comp);
			if (resComp != nullptr)
			{
				return resComp;
			}
		}
		return nullptr;
	};


	/// <summary>
	/// コンポーネントの追加
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template<typename T>
	std::shared_ptr<T> add_Component(int updateRank = 100)
	{
		for (auto& comp : m_pComponentList)
		{
			std::shared_ptr<T> resComp = std::dynamic_pointer_cast<T>(comp);

			// すでにあるならそれを返す
			if (resComp != nullptr)
			{
				return resComp;
			}
		}

		auto newComp = std::make_shared<T>(shared_from_this(), updateRank);

		// 見つからなければ追加する
		m_pComponentList.push_back(newComp);

		return newComp;
	}


	/// <summary>
	/// コンポーネントを外す
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T>
	void remove_Component()
	{
		auto begin = m_pComponentList.begin();
		auto end = m_pComponentList.end();

		m_pComponentList.erase(std::remove_if(begin, end,
			[](const std::shared_ptr<IComponent>& comp)
			{
				return (std::dynamic_pointer_cast<T>(comp) != nullptr);
			}),
			end
		);
	}
};

