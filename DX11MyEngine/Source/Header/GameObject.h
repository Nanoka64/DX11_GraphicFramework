#pragma once
//#include "Object.h"
#include "Component_Transform.h"
#include "Component_RectTransform.h"

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
//  ※3/12
//　　　コンポーネントを連想配列で持つようにした。ダイナミックキャストするよりも多分こっちの方が速い
//　　　ただし、親クラスから子クラスを取得することは出来ない。
//		↓みたいなこと
//		obj->add_Component<BoxCollider>();
//      obj->get_Component<Collider>();
// 
//	※検証した結果
//		コンポーネントの数が多いほど、unordered_mapの力が発揮されるけど、少ないとvectorでdynamic_cast回してたのと
//		ほぼ同等の速度になる。Updateを回すだけなら多分vectorのほうが速い。
//		なので更新はvector、取得時にはunordered_mapとかでもいいかもしれない。
//
// ***************************************************************************************
class GameObject : public std::enable_shared_from_this<GameObject> , public Object
{
private:	
	std::unordered_map<std::type_index, std::shared_ptr<IComponent>> m_pComponentMap;	// コンポーネント配列

    std::shared_ptr<MyTransform> m_pTransform;	// トランスフォームコンポーネントはデフォルトで持つ（2DならRectTransformを持つ）

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

	void OnCollisionEnter(const class CollisionInfo& _other);	// 当たった瞬間
	void OnCollisionStay(const class CollisionInfo& _other);	// 当たっている間
	void OnCollisionExit(const class CollisionInfo& _other);	// 離れた瞬間
	void OnTriggerEnter(const class CollisionInfo& _other);		// トリガー 当たった瞬間
	void OnTriggerStay(const class CollisionInfo& _other);		// トリガー 当たっている間
	void OnTriggerExit(const class CollisionInfo& _other);		// トリガー 離れた瞬間

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
	std::weak_ptr<MyTransform> get_Transform()const;

	/// <summary>
	/// RectTransformの取得
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<RectTransform> get_RectTransform()const;


	// 生ポで書いたほうがいいかも
	//MyTransform *get_Transform() const;



	/// <summary>
	/// コンポーネントのリストを取得
	/// </summary>
	/// <returns></returns>
	std::unordered_map<std::type_index, std::shared_ptr<IComponent>> get_ComponentMap()const;

	/// <summary>
	/// コンポーネントの取得
	/// ※ 型が完全に一致していないと取得できない
	/// 　 子クラスから親クラスへの変換取得などは不可
	/// </summary>
	/// <typeparam name="T">取得するコンポーネントの型</typeparam>
	/// <returns></returns>
	template<typename T>
	std::shared_ptr<T> get_Component() const
	{
		auto it = m_pComponentMap.find(std::type_index(typeid(T)));

		if (it != m_pComponentMap.end())
		{
			return std::static_pointer_cast<T>(it->second);
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
		auto typeIdx = std::type_index(typeid(T));
		auto it = m_pComponentMap.find(typeIdx);

		// 既に存在するならキャストして返す
		if (it != m_pComponentMap.end())
		{
			return std::static_pointer_cast<T>(it->second);
		}

		// 生成
		auto newComp = std::make_shared<T>(shared_from_this(), updateRank);
		m_pComponentMap[typeIdx] = newComp;

		return newComp;
	}


	/// <summary>
	/// コンポーネントを外す
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T>
	void remove_Component()
	{
		m_pComponentMap.erase(std::type_index(typeid(T)));
	}
};

