#pragma once
//#include "Object.h"
#include "Component_Transform.h"

/// <summary>
/// オブジェクトの状態定義
/// </summary>
enum class OBJECT_STATE
{
	STATIC,		// 静的
	DYNAMIC		// 動的
};


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
	std::vector<std::shared_ptr<IComponent>> m_pComponentList;	// コンポーネントリスト
    std::shared_ptr<MyTransform> m_pTransform;	// トランスフォームコンポーネントはデフォルトで持つ

	bool m_IsCalcUpdate;	// 更新処理がすでに呼ばれたかどうか
	bool m_IsShadow;		// シャドウをかけるかどうか
	OBJECT_STATE m_State;	// オブジェクトの状態（静的か動的か）

	/* オブジェクトマネージャをフレンドとして登録 */
	friend class GameObjectManager;
public:
    GameObject();
    virtual ~GameObject();

	virtual bool Init(RendererEngine& renderer) { return true; };
	virtual void Update(RendererEngine& renderer) {};
	virtual void Draw(RendererEngine& renderer) {};
	virtual bool Term(RendererEngine& renderer) { return true; };

	void OnCollisionEnter(const class CollisionInfo &info);
	void OnCollisionStay(const class CollisionInfo &info);
	void OnCollisionExit(const class CollisionInfo &info);

	bool get_IsShadow()const { return m_IsShadow; }
	void set_IsShadow(bool _flag) { m_IsShadow = _flag; }

	OBJECT_STATE get_State()const { return m_State; }
	void set_State(const OBJECT_STATE& _state) { m_State = _state; }

	// ****************************************************************************************************************************************
	/* コンポーネント関連 */
	// ****************************************************************************************************************************************
	/// <summary>
	/// コンポーネントの更新
	/// </summary>
	void ComponentUpdate(RendererEngine& renderer);

	/// <summary>
	/// 更新後の更新
	/// </summary>
	/// <param name="renderer"></param>
	void ComponentLateUpdate(RendererEngine &renderer);

	/// <summary>
	/// コンポーネントの描画
	/// </summary>
	/// <param name="renderer"></param>
	void ComponentRender(RendererEngine& renderer);


	/// <summary>
	/// Transformの取得
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<class MyTransform> get_Transform()const;


	// 生ポで書いたほうがいいかも
	//MyTransform *get_Transform() const;



	/// <summary>
	/// コンポーネントのリストを取得
	/// </summary>
	/// <returns></returns>
	std::vector<std::shared_ptr<IComponent>> get_ComponentList()const;

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


		// 見つからなければ追加
		auto newComp = std::make_shared<T>(shared_from_this(), updateRank);

		// 更新レイヤーから挿入位置を探索
		auto it = std::find_if(m_pComponentList.begin(), m_pComponentList.end(),
			[&](const std::shared_ptr<IComponent> &comp)
			{
				return newComp->get_UpdateRank() < comp->get_UpdateRank();
			}
		);

		// 挿入
		m_pComponentList.insert(it, newComp);

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

