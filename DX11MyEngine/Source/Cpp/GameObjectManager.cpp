#include "pch.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "Master.h"
#include "RendererEngine.h"
#include  <algorithm>

using namespace VECTOR3;


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】コンストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
GameObjectManager::GameObjectManager()
{

}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】デストラクタ
//* 引数：なし
//*----------------------------------------------------------------------------------------
GameObjectManager::~GameObjectManager()
{

}

//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】初期化
//* 引数：1.RendererEngine
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool GameObjectManager::Init(RendererEngine &renderer)
{
    m_pObjectList.clear();
    return true;
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::ObjectUpdate(RendererEngine &renderer)
{
    std::vector<std::shared_ptr<GameObject>> collisionList;    // 衝突判定用

    for (auto it = m_pObjectList.begin(); it != m_pObjectList.end(); it++)
    {
        if ((*it)->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE) == true) 
        {
            (*it).get()->Update(renderer);
            (*it).get()->ComponentUpdate(renderer);
        }
    }
}

//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】更新後の更新
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::ObjectLateUpdate(RendererEngine &renderer)
{
    std::vector<std::shared_ptr<GameObject>> deleteList;       // 削除用

    
    // ******************************************************************
    //
    // 不透明オブジェクト
    // 
    // ******************************************************************
    for (auto it = m_pObjectList.begin(); it != m_pObjectList.end(); it++)
    {
        if ((*it)->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE) == true) 
        {
            (*it).get()->Update(renderer);
            (*it).get()->ComponentLateUpdate(renderer);
        }
        // 削除フラグが立っていれば削除リストに追加
        if ((*it).get()->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_DELETE) == true)
        {
            deleteList.push_back((*it));
        }
    }

    // ******************************************************************
    //
    //  透明度有りオブジェクト
    // 
    // ******************************************************************
    for (auto it = m_pTransparent_ObjectList.begin(); it != m_pTransparent_ObjectList.end(); it++)
    {
        if ((*it)->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE) == true) 
        {
            (*it).get()->Update(renderer);
            (*it).get()->ComponentLateUpdate(renderer);
        }
        // 削除フラグが立っていれば削除リストに追加
        if ((*it).get()->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_DELETE) == true)
        {
            deleteList.push_back((*it));
        }
    }

    // 削除する
    for (auto& del : deleteList)
    {
        remove_Object(del);
    }
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】メインパス描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::ObjectMainRenderPass(RendererEngine &renderer)
{
    int id = 0;
    Master::m_pDebugger->BeginDebugWindow(Tool::U8ToChar(u8"コンポーネント確認"));
    Master::m_pDebugger->DG_BulletText(Tool::U8ToChar(u8"こちらでは追加されている\nすべてのコンポーネントを確認できます。"));
    Master::m_pDebugger->DG_Separator();
    Master::m_pDebugger->DG_BulletText("Count : %d", m_pObjectList.size());

    // 描画
    for (auto& obj : m_pObjectList)
    {
        // *** 静的 ***
        if (obj->get_State() == OBJECT_STATE::STATIC)
        {
            renderer.RegisterDefaultDepthStencilState(1);
        }
        // *** 動的 ***
        else if (obj->get_State() == OBJECT_STATE::DYNAMIC)
        {
            renderer.RegisterDefaultDepthStencilState(0);
        }

        if (obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE) == true) {
            obj->Draw(renderer);
            obj->ComponentRender(renderer);
        }

        //******************************************************
        // 各オブジェクトのコンポーネントをツリー上に表示
        //******************************************************
        id++;
        // IDを名前に入れて一意にする（##ID は表示されず、内部的な識別子になる）
        std::string label = obj->get_Tag() + "##" + std::to_string(id);

        if (Master::m_pDebugger->DG_TreeNode(label))
        {
            // オブジェクトが持つコンポーネントを一覧表示
            for (auto& component : obj->get_ComponentList())
            {
                Master::m_pDebugger->DG_BulletText(component->get_Tag().c_str());
            }
            Master::m_pDebugger->DG_TreePop();
        }
    }
    Master::m_pDebugger->EndDebugWindow();
}



//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】シャドウパス描画
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::ObjectShadowRenderPass(RendererEngine &renderer)
{
    // 描画
    for (auto &obj : m_pObjectList)
    {
        if (!obj->get_IsShadow() ||
            !obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE))
        {
            continue;
        }
        //obj->Draw(renderer);
        obj->ComponentRender(renderer);
    }
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】透明度ありオブジェクトの描画パス
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::Alpha_ObjectRenderPass(RendererEngine &renderer)
{
    VEC3 camPos = renderer.get_CameraPosition();    // カメラ座標
    
    // カメラ座標から遠い順にソートする
    m_pTransparent_ObjectList.sort(
        [camPos](const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b)
        {
            float a_CamDist = VEC3::DistanceSq(a->get_Transform().lock()->get_VEC3ToPos(), camPos);
            float b_CamDist = VEC3::DistanceSq(b->get_Transform().lock()->get_VEC3ToPos(), camPos);
            return (a_CamDist > b_CamDist);
        }
    );

    // 描画
    for (auto &obj : m_pTransparent_ObjectList)
    {
        if (!obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE))
        {
            continue;
        }
        //obj->Draw(renderer);
        obj->ComponentRender(renderer);
    }
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】終了
//* 引数：1.RendererEngine
//* 返値：bool
//*----------------------------------------------------------------------------------------
bool GameObjectManager::Term(RendererEngine &renderer)
{
    m_pObjectList.clear();
    return true;
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】オブジェクトの生成
//      この関数を通じてオブジェクトを生成する。
//* 引数：1.RendererEngine
//* 返値：生成したオブジェクトの共有ポインタ
//*----------------------------------------------------------------------------------------
std::shared_ptr<GameObject> GameObjectManager::Internal_Instantiate(std::shared_ptr<GameObject> pObj, bool isTransparent, VECTOR3::VEC3 pos, VECTOR3::VEC3 rot, std::weak_ptr<MyTransform> parent)
{
    // Transformは全てのオブジェクトに共通するコンポーネントとするため、生成時に追加する
    // (Unity風に)
    pObj->add_Component<MyTransform>();
    pObj->m_pTransform = pObj->get_Component<MyTransform>();
    pObj->m_pTransform->set_Pos(pos);
    pObj->m_pTransform->set_RotateToDeg(rot);

    // 透明度があるならフラグ立てる
    if (isTransparent){
        pObj->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_TRANSPARENT);
    }

    // 親が設定されているか
    if (parent.lock() != nullptr) {
        pObj->m_pTransform->set_Parent(parent);
    }


    // リストに追加
    this->add_Object(pObj);   

    return pObj;
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】オブジェクトの追加
//* 引数：1.追加するオブジェクトの共有ポインタ
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::add_Object(std::shared_ptr<GameObject> object)
{
    // 透明度のあるオブジェクト
    if (object->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_TRANSPARENT))
    {
        m_pTransparent_ObjectList.push_back(object);
    }
    // 不透明オブジェクト
    else
    {
        m_pObjectList.push_back(object);
    }

    // 更新順にソート
    m_pObjectList.sort(
        [](const std::shared_ptr<GameObject> &a, const std::shared_ptr<GameObject> &b) {
            return a->get_LayerRank() < b->get_LayerRank();
        });
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】リストからオブジェクトを削除　
//* 引数：1.削除するオブジェクトの共有ポインタ
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::remove_Object(std::shared_ptr<GameObject> object)
{
    // 不透明オブジェクト**********************
    auto begin = m_pObjectList.begin();
    auto end = m_pObjectList.end();

    m_pObjectList.erase(std::remove_if(begin, end,
        [object](const std::shared_ptr<GameObject> &obj)
        {
            return (obj == object);
        }),
        end
    );    
    
    // 透明度のあるオブジェクト**********************
    begin = m_pTransparent_ObjectList.begin();
    end = m_pTransparent_ObjectList.end();

    m_pTransparent_ObjectList.erase(std::remove_if(begin, end,
        [object](const std::shared_ptr<GameObject> &obj)
        {
            return (obj == object);
        }),
        end
    );
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】リストからオブジェクトを削除　※タグ指定
//* 引数：1.削除するオブジェクトの共有ポインタ
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::remove_ObjectByTag(const std::string &tag)
{
    // 不透明オブジェクト**********************
    auto begin = m_pObjectList.begin();
    auto end = m_pObjectList.end();

    m_pObjectList.erase(std::remove_if(begin, end,
        [tag](const std::shared_ptr<GameObject> &obj)
        {
            return (obj->get_Tag() == tag);
        }),
        end
    );    
    
    // 透明度のあるオブジェクト**********************
    begin = m_pTransparent_ObjectList.begin();
    end = m_pTransparent_ObjectList.end();

    m_pTransparent_ObjectList.erase(std::remove_if(begin, end,
        [tag](const std::shared_ptr<GameObject> &obj)
        {
            return (obj->get_Tag() == tag);
        }),
        end
    );
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】指定タグのオブジェクトを一つ取得
//* 引数：1.タグ
//* 返値：オブジェクトの参照ポインタ
//*----------------------------------------------------------------------------------------
std::shared_ptr<GameObject> GameObjectManager::get_ObjectByTag(const std::string& tag)
{
    // 最初に不透明オブジェクトを走査**********************
    auto begin = m_pObjectList.begin();
    auto end = m_pObjectList.end();

    auto it = std::find_if(begin, end,
        [tag](const std::shared_ptr<GameObject>& obj)
        {
            return (obj->get_Tag() == tag);
        }
    );

    if (it != end)
    {
        return (*it);
    }
    // 見つからなければ透明度のあるオブジェクトの方も走査**********************
    else
    {
        begin = m_pTransparent_ObjectList.begin();
        end = m_pTransparent_ObjectList.end();

        it = std::find_if(begin, end,
            [tag](const std::shared_ptr<GameObject>& obj)
            {
                return (obj->get_Tag() == tag);
            }
        );
        if (it != end)
        {
            return (*it);
        }
    }

    return {};  // 最終的に見つからなければ空を返す
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】指定タグのオブジェクトをすべてリストにして取得
///     ※ 透明/不透明両方
//* 引数：1.タグ
//* 返値：オブジェクトの参照ポインタリスト
//*----------------------------------------------------------------------------------------
std::list<std::shared_ptr<GameObject>> GameObjectManager::get_ObjectListByTag(const std::string &tag)
{
    std::list<std::shared_ptr<GameObject>> resList;

    // 不透明オブジェクト**********************
    for (auto &obj : m_pObjectList)
    {
        if (obj->get_Tag() == tag)
        {
            resList.push_back(obj);
        }
    }

    // 透明度のあるオブジェクト**********************
    for (auto &obj : m_pTransparent_ObjectList)
    {
        if (obj->get_Tag() == tag)
        {
            resList.push_back(obj);
        }
    }

    return resList;
}

//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】登録されているオブジェクト数取得
///     ※ 透明/不透明両方
//* 引数：なし
//* 返値：オブジェクト数
//*----------------------------------------------------------------------------------------
size_t GameObjectManager::get_ObjectNum()const
{
    size_t res = m_pObjectList.size() + m_pTransparent_ObjectList.size();
    return res;
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】不透明オブジェクトリストを取得
//* 引数：なし
//* 返値：オブジェクトのリスト
//*----------------------------------------------------------------------------------------
std::list<std::shared_ptr<GameObject>> GameObjectManager::get_Opaque_ObjectList()
{
    return m_pObjectList;
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】透明度のあるオブジェクトリスト取得
//* 引数：なし
//* 返値：オブジェクトのリスト
//*----------------------------------------------------------------------------------------
std::list<std::shared_ptr<GameObject>> GameObjectManager::get_Transparent_ObjectList()
{
    return m_pTransparent_ObjectList;
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】全てのオブジェクトを削除
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::clear_AllObject()
{
    // スマートポインタの場合、
    // デストラクタで破棄されるのでdelete要らない（便利だね！）
    m_pObjectList.clear();  
    m_pTransparent_ObjectList.clear();
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】dontDestroyフラグが立っていないオブジェクトを全て削除
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::clear_NotIsDontDestroyObject()
{
    m_pObjectList.remove_if(
        [](const std::shared_ptr<GameObject> &obj)
        {
            return (obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_DONT_DESTROY) == false);
        }
    );
    m_pTransparent_ObjectList.remove_if(
        [](const std::shared_ptr<GameObject> &obj)
        {
            return (obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_DONT_DESTROY) == false);
        }
    );
}

namespace GIGA_Engine
{
    std::shared_ptr<GameObject>Instantiate(std::shared_ptr<GameObject> pObj,bool isTransparent, VECTOR3::VEC3 pos , VECTOR3::VEC3 rot , std::weak_ptr<MyTransform> parent )
    {
        return Master::m_pGameObjectManager->Internal_Instantiate(pObj, isTransparent, pos, rot, parent);
    }
};


