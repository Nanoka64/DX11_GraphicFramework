#pragma once
#include "IComponent.h"
#include "ConstantBulletData.h"




// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Bullet Class --- */
//
//  ★継承：IComponent ★
// 
// 【?】振る舞いを付け替えることで、弾の種類を変えられるようにする
//		
// ***************************************************************************************
class Bullet : public IComponent
{
private:
	VECTOR3::VEC3 m_StartPos;	// 発射開始位置
	VECTOR3::VEC3 m_PrevPos;	// 前の座標
	VECTOR3::VEC3 m_MoveDir;
	int m_CrntPenetrationCount;	// 現在の貫通数
	const BulletData::BulletDataBase* m_pBulletData;							// 読みとり専用の弾データを持つ

    std::array<std::unique_ptr<class IBulletBehaviour>,
        static_cast<size_t>(BulletData::BULLET_BEHAVIOUR_TYPE::NUM)> m_Behaviours;
    class IBulletBehaviour* m_pCurrentBehaviour = nullptr;

public:
	Bullet(std::weak_ptr<GameObject> pOwner, int updateRank = 100);
	virtual ~Bullet() = default;

    void Start(RendererEngine& renderer) override;
    void Update(RendererEngine& renderer) override;
    void LateUpdate(RendererEngine& renderer) override;
    void Setup(
        const BulletData::BulletDataBase* _pData,
        const BulletData::BulletSpawnContext& context);

    void Deactivate(BulletData::BULLET_END_REASON reason);
    void Reset();
    const BulletData::BulletDataBase* get_BulletData()const { return  m_pBulletData; }


    template<typename T>
    const T* get_HitData() const
    {
        if (!m_pBulletData)
        {
            return nullptr;
        }
        return std::get_if<T>(&m_pBulletData->_hitData);
    }
};

