#pragma once
class GameObject;

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:GameObjectManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】GameObjectクラスの更新、削除等の管理をする
//		GameObjectの共有ポインタをlistで持つ
//
// ***************************************************************************************
class GameObjectManager
{
private:
	std::list<std::shared_ptr<class GameObject>> m_pObjectList;

public:
	GameObjectManager();
	~GameObjectManager();

	bool Init(RendererEngine &renderer);					// 初期化
	bool Term(RendererEngine &renderer);					// 終了処理
	void ObjectUpdate(RendererEngine &renderer);			// 更新
	void ObjectLateUpdate(RendererEngine &renderer);		// 遅延更新

	void ObjectMainRenderPass(RendererEngine &renderer);	// 通常描画パス
	void ObjectShadowRenderPass(RendererEngine &renderer);	// シャドウ描画パス
	

	
	/// <summary>
	/// オブジェクトの内部的生成
	/// </summary>
	/// <param name=""></param>
	/// <param name="pos"></param>
	/// <param name="rot"></param>
	/// <returns></returns>
	std::shared_ptr<GameObject>Internal_Instantiate(std::shared_ptr<GameObject> pObj, VECTOR3::VEC3 pos = { 0.f,0.f,0.f }, VECTOR3::VEC3 rot = { 0.f,0.f,0.f }, std::weak_ptr<class Transform> parent = {});


	/// <summary>
	/// リストからオブジェクトを削除
	/// </summary>
	/// <param name="object"></param>
	void remove_Object(std::shared_ptr<GameObject> object);


	/// <summary>
	/// リストからオブジェクトを削除　※タグ指定
	/// </summary>
	/// <param name="tag"></param>
	void remove_ObjectByTag(const std::string &tag);
	

	/// <summary>
	/// 指定タグのオブジェクトを一つ取得
	/// </summary>
	/// <param name="tag"></param>
	/// <returns></returns>
	std::shared_ptr<GameObject>get_ObjectByTag(const std::string& tag);


	/// <summary>
	/// 指定タグのオブジェクトをすべてリストにして取得
	/// </summary>
	/// <param name="tag"></param>
	/// <returns></returns>
	std::list<std::shared_ptr<GameObject>>get_ObjectListByTag(const std::string &tag);


	/// <summary>
	/// オブジェクトリストを取得
	/// </summary>
	/// <returns></returns>
	std::list<std::shared_ptr<GameObject>>get_ObjectList();


	/// <summary>
	/// オブジェクト数取得
	/// </summary>
	/// <returns></returns>
	size_t get_ObjectNum()const;


	/// <summary>
	/// 全てのオブジェクトを削除
	/// </summary>
	void clear_AllObject();


	/// <summary>
	/// dontDestroyフラグが立っていないオブジェクトを全て削除
	/// </summary>
	void clear_NotIsDontDestroyObject();




	/// <summary>
	/// 条件に合ったオブジェクトを参照リストで取得する
	/// 取得したいオブジェクトの条件式を呼び出し元で指定する
	/// std::functionだとコストがかかるらしいので、テンプレート化しラムダ式を渡す
	/// </summary>
	/// <typeparam name="Function"></typeparam>
	/// <param name="ifFunc">条件ラムダ式</param>
	/// <returns></returns>
	template<typename Function>
	std::list<std::shared_ptr<GameObject>> get_ObjectListByIfFunction(Function ifFunc)
	{
		std::list<std::shared_ptr<GameObject>> resList;

		for (auto &obj : m_pObjectList)
		{
			if (ifFunc(obj))
			{
				resList.push_back(obj);
			}
		}
		return resList;
	}


private:
	// コピー禁止
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager&operator=(const GameObjectManager&) = delete;
	// ------------------------------------------------------


	/// <summary>
	/// リストにオブジェクトを追加
	/// </summary>
	/// <param name="object"></param>
	void add_Object(std::shared_ptr<GameObject> object);
};

namespace GIGA_Engine
{
	//*---------------------------------------------------------------------------------------
	//* @:ObjectManager Class 
	//*【?】オブジェクトの生成（外部公開）
	//* 引数：1.追加するオブジェクトの共有ポインタ
	//* 引数：2.位置
	//* 引数：3.回転
	//* 引数：4.親
	//* 返値：void
	//*----------------------------------------------------------------------------------------
	std::shared_ptr<GameObject>Instantiate(std::shared_ptr<GameObject> pObj, VECTOR3::VEC3 pos = { 0.f,0.f,0.f }, VECTOR3::VEC3 rot = { 0.f,0.f,0.f }, std::weak_ptr<Transform> parent = {});
}