#include "pch.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "Component_Transform.h"
#include "Master.h"
#include "RendererEngine.h"
#include "Component_RectTransform.h"
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
    m_3DOpaqueList.clear();
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
    // ******************************************************************
    //
    // 不透明オブジェクト
    // 
    // ******************************************************************
    for (auto it = m_3DOpaqueList.begin(); it != m_3DOpaqueList.end(); it++)
    {
        if ((*it)->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE) == true) 
        {
            (*it).get()->Update(renderer);
            (*it).get()->ComponentUpdate(renderer);
        }
    }
    // ******************************************************************
    //
    //  透明度有りオブジェクト
    // 
    // ******************************************************************
    for (auto it = m_3DTranslucentList.begin(); it != m_3DTranslucentList.end(); it++)
    {
        if ((*it)->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_ACTIVE) == true) 
        {
            (*it).get()->Update(renderer);
            (*it).get()->ComponentUpdate(renderer);
        }
    }
    // ******************************************************************
    //
    //  2Dオブジェクト
    // 
    // ******************************************************************
    for (auto it = m_2DTranslucentList.begin(); it != m_2DTranslucentList.end(); it++)
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
    for (auto it = m_3DOpaqueList.begin(); it != m_3DOpaqueList.end(); it++)
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
    for (auto it = m_3DTranslucentList.begin(); it != m_3DTranslucentList.end(); it++)
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
    //  2Dオブジェクト
    // 
    // ******************************************************************
    for (auto it = m_2DTranslucentList.begin(); it != m_2DTranslucentList.end(); it++)
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
    Master::m_pDebugger->DG_BulletText("Count : %d", m_3DOpaqueList.size());

    // 描画
    for (auto& obj : m_3DOpaqueList)
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
            for (auto& component : obj->get_ComponentMap())
            {
                Master::m_pDebugger->DG_BulletText(component.second->get_Tag().c_str());
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
    for (auto &obj : m_3DOpaqueList)
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
    auto begin = m_3DTranslucentList.begin();
    auto end = m_3DTranslucentList.end();

    // カメラ座標から遠い順にソートする
    std::sort(begin, end, [camPos](const std::shared_ptr<GameObject> &a, const std::shared_ptr<GameObject> &b)
        {
            float a_CamDist = VEC3::DistanceSq(a->get_Transform().lock()->get_VEC3ToPos(), camPos);
            float b_CamDist = VEC3::DistanceSq(b->get_Transform().lock()->get_VEC3ToPos(), camPos);
            return (a_CamDist > b_CamDist);
        });

    // 描画
    for (auto &obj : m_3DTranslucentList)
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
//*【?】透明度あり2Dオブジェクトの描画パス
//* 引数：1.RendererEngine
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::Alpha_2DObjectRenderPass(RendererEngine &renderer)
{
    VEC3 camPos = renderer.get_CameraPosition();    // カメラ座標
    auto begin = m_2DTranslucentList.begin();
    auto end = m_2DTranslucentList.end();

    // 描画
    for (auto &obj : m_2DTranslucentList)
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
    m_3DOpaqueList.clear();
    m_3DTranslucentList.clear();
    m_2DTranslucentList.clear();
    return true;
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】オブジェクトの生成
//      この関数を通じてオブジェクトを生成する。
//* 引数：1.RendererEngine
//* 返値：生成したオブジェクトの共有ポインタ
//*----------------------------------------------------------------------------------------
std::shared_ptr<GameObject> GameObjectManager::Internal_Instantiate3D(std::shared_ptr<GameObject> pObj, bool isTransparent, VECTOR3::VEC3 pos, VECTOR3::VEC3 rot, std::weak_ptr<MyTransform> parent)
{
    // Transformは全てのオブジェクトに共通するコンポーネントとするため、生成時に追加する
    // (Unity風に)
    pObj->add_Component<MyTransform>();
    pObj->m_pTransform = pObj->get_Component<MyTransform>();
    pObj->m_pTransform->set_Pos(pos);
    pObj->m_pTransform->set_RotateToDeg(rot);

    // 親が設定されているか
    if (parent.lock() != nullptr) {
        pObj->m_pTransform->set_Parent(parent);
    }


    // 透明度があるならフラグ立てる
    if (isTransparent) {
        pObj->set_StatusFlag(OBJECT_STATUS_BITFLAG::IS_TRANSPARENT);
        this->add_Object(OBJECT_TYPE::TRANSLICENT_3D, pObj);
    }
    else
    {
        this->add_Object(OBJECT_TYPE::OPAQUE_3D, pObj);
    }


    return pObj;
}

//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】2Dオブジェクトの生成
//      この関数を通じてオブジェクトを生成する。
//* 引数：1.RendererEngine
//* 返値：生成したオブジェクトの共有ポインタ
//*----------------------------------------------------------------------------------------
std::shared_ptr<GameObject> GameObjectManager::Internal_Instantiate2D(std::shared_ptr<GameObject> pObj, bool isTransparent, VECTOR3::VEC3 pos, VECTOR3::VEC3 rot, std::weak_ptr<RectTransform> parent)
{
    // Transformは全てのオブジェクトに共通するコンポーネントとするため、生成時に追加する
    // (Unity風に)
    pObj->add_Component<RectTransform>();
    pObj->m_pTransform = pObj->get_Component<RectTransform>();
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
    this->add_Object(OBJECT_TYPE::TRANSLICENT_2D, pObj);   

    return pObj;
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】オブジェクトの追加
//* 引数：1.追加するオブジェクトの共有ポインタ
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::add_Object(OBJECT_TYPE _type, std::shared_ptr<GameObject> _object)
{
    switch (_type)
    {
        // 不透明オブジェクト
    case OBJECT_TYPE::OPAQUE_3D:
        m_3DOpaqueList.push_back(_object);
        break;
        // 透明度のあるオブジェクト
    case OBJECT_TYPE::TRANSLICENT_3D:
        m_3DTranslucentList.push_back(_object);
        break;
    case OBJECT_TYPE::TRANSLICENT_2D:
        m_2DTranslucentList.push_back(_object);
        break;
    }

    auto begin = m_3DOpaqueList.begin();
    auto end = m_3DOpaqueList.end();

    // 不透明オブジェクトのみ更新順にソート
    std::sort(begin, end,
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
    std::erase(m_3DOpaqueList, object);
    std::erase(m_3DTranslucentList, object);
    std::erase(m_2DTranslucentList, object);
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】リストからオブジェクトを削除　※タグ指定
//* 引数：1.削除するオブジェクトの共有ポインタ
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::remove_ObjectByTag(const std::string &tag)
{
    // 3D不透明
    std::erase_if(m_3DOpaqueList,
        [&tag](const auto &obj) {
        return obj->get_Tag() == tag;
        }
    );

    // 3D透明
    std::erase_if(m_3DTranslucentList, 
        [&tag](const auto &obj) {
        return obj->get_Tag() == tag;
        }
    );

    // 2D透明
    std::erase_if(m_2DTranslucentList, 
        [&tag](const auto &obj) {
        return obj->get_Tag() == tag;
        }
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
    auto begin = m_3DOpaqueList.begin();
    auto end = m_3DOpaqueList.end();

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
        begin = m_3DTranslucentList.begin();
        end = m_3DTranslucentList.end();

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

        // それでも見つからなければ2Dオブジェクトの方も走査**********************
        begin = m_2DTranslucentList.begin();
        end = m_2DTranslucentList.end();

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

    return nullptr;  // 最終的に見つからなければ空を返す
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】指定タグのオブジェクトをすべてリストにして取得
///     ※ 透明/不透明両方
//* 引数：1.タグ
//* 返値：オブジェクトの参照ポインタリスト
//*----------------------------------------------------------------------------------------
std::vector<std::shared_ptr<GameObject>> GameObjectManager::get_ObjectListByTag(const std::string &tag)
{
    std::vector<std::shared_ptr<GameObject>> resList;

    // 不透明3Dオブジェクト**********************
    for (auto &obj : m_3DOpaqueList)
    {
        if (obj->get_Tag() == tag)
        {
            resList.push_back(obj);
        }
    }

    // 透明度のある3Dオブジェクト**********************
    for (auto &obj : m_3DTranslucentList)
    {
        if (obj->get_Tag() == tag)
        {
            resList.push_back(obj);
        }
    }

    // 透明度のある2Dオブジェクト**********************
    for (auto &obj : m_2DTranslucentList)
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
    size_t res = m_3DOpaqueList.size() + m_3DTranslucentList.size() + m_2DTranslucentList.size();
    return res;
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
    // デストラクタで破棄されるのでdelete要らない（便利！）
    m_3DOpaqueList.clear();  
    m_3DTranslucentList.clear();
    m_2DTranslucentList.clear();
}


//*---------------------------------------------------------------------------------------
//* @:GameObjectManager Class 
//*【?】dontDestroyフラグが立っていないオブジェクトを全て削除
//* 引数：なし
//* 返値：void
//*----------------------------------------------------------------------------------------
void GameObjectManager::clear_NotIsDontDestroyObject()
{
    // 3D不透明
    m_3DOpaqueList.erase(std::remove_if(m_3DOpaqueList.begin(), m_3DOpaqueList.end(),
        [](const std::shared_ptr<GameObject> &obj)
        {
            return (obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_DONT_DESTROY) == false);
        }),
        m_3DOpaqueList.end()
    );

    // 3D透明
    m_3DTranslucentList.erase(std::remove_if(m_3DTranslucentList.begin(), m_3DTranslucentList.end(),
        [](const std::shared_ptr<GameObject> &obj)
        {
            return (obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_DONT_DESTROY) == false);
        }),
        m_3DTranslucentList.end()
    );

    // 2D透明
    m_2DTranslucentList.erase(std::remove_if(m_2DTranslucentList.begin(), m_2DTranslucentList.end(),
        [](const std::shared_ptr<GameObject> &obj)
        {
            return (obj->get_IsStatusFlag(OBJECT_STATUS_BITFLAG::IS_DONT_DESTROY) == false);
        }),
        m_2DTranslucentList.end()
    );
}

namespace GIGA_Engine
{
    std::shared_ptr<GameObject>Instantiate3D(std::shared_ptr<GameObject> pObj,bool isTransparent, VECTOR3::VEC3 pos , VECTOR3::VEC3 rot , std::weak_ptr<MyTransform> parent )
    {
        return Master::m_pGameObjectManager->Internal_Instantiate3D(pObj, isTransparent, pos, rot, parent);
    }
    std::shared_ptr<GameObject>Instantiate2D(std::shared_ptr<GameObject> pObj,bool isTransparent, VECTOR3::VEC3 pos , VECTOR3::VEC3 rot , std::weak_ptr<RectTransform> parent )
    {
        return Master::m_pGameObjectManager->Internal_Instantiate2D(pObj, isTransparent, pos, rot, parent);
    }
};


