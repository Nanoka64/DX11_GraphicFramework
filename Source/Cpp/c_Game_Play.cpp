#include "pch.h"
#include "c_Game_Play.h"
#include "ResourceManager.h"
#include "SceneStateEnums.h"
#include "InputFactory.h"
#include "GameObject.h"

using namespace SceneStateEnums;
using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//* @:c_Game_Play Class 
//*【?】開始
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Game_Play::OnEnter(SceneManager* pOwner)
{
    m_pBomber[0] = Master::m_pGameObjectManager->get_ObjectByTag("B-2_1");
    m_pBomber[1] = Master::m_pGameObjectManager->get_ObjectByTag("B-2_2");
    m_pBomber[2] = Master::m_pGameObjectManager->get_ObjectByTag("B-2_3");
}


//*---------------------------------------------------------------------------------------
//* @:c_Game_Play Class 
//*【?】終了
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Game_Play::OnExit(SceneManager* pOwner)
{

}


//*---------------------------------------------------------------------------------------
//* @:c_Game_Play Class 
//*【?】更新
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
int c_Game_Play::Update(SceneManager *pOwner)
{
    static float counter = 0.0f;
    counter += 0.01f;

    auto antList = Master::m_pGameObjectManager->get_ObjectListByTag("Ant");
    m_EnemyNum = antList.size();
    // 敵が居なくなったらリザルトへ
    if (m_EnemyNum == 0)
    {
        return c_GAME::c_GO_RESULT_SCENE;
    }


    //-----------------------------------------------------------------------------
    // ■ 爆撃機の移動処理
    //-----------------------------------------------------------------------------
    static VEC3 B2PrevPos[3]{};
    VEC3 center = VEC3(0.0f, 0.0f, 0.0f);
    float radius = 700.0f;
    for (int i = 0; i < 3; i++)
    {
        auto B2Transform = m_pBomber[i]->get_Transform().lock();
        if (B2Transform)
        {
            float intervalDist = 200.0f * i;

            VEC3 pos = VEC3(0.0f, 0.0f, 0.0f);
            pos.x = center.x + (radius - intervalDist) * cos(counter);
            pos.y = 300.0f + i * 100.0f;
            pos.z = center.z + (radius - intervalDist) * sin(counter);

            // 速度 = 今の座標 - 前の座標
            VEC3 velocity = pos - B2PrevPos[i];

            // 移動ベクトルから、Y軸周りの回転角度を求める
            float targetAngle = atan2(velocity.x, velocity.z);

            // 前の座標として保持
            B2PrevPos[i] = pos;

            B2Transform->set_Pos(pos);
            B2Transform->set_RotateToRad(0.0f, targetAngle, 0.0f);
        }
    }
    return c_GAME::c_GAME_PLAY;
}


//*---------------------------------------------------------------------------------------
//* @:c_Game_Play Class 
//*【?】描画
//* 引数：1.SceneManager
//* 返値：void
//*----------------------------------------------------------------------------------------
void c_Game_Play::Draw(SceneManager* pOwner)
{
    Master::m_pDirectWriteManager->DrawFormatString("残りの敵数：{:d}", VECTOR2::VEC2(0, 540), "White_40_STD",m_EnemyNum);
}
