#pragma once
#include "ConstantWeaponData.h"
#include "ConstantBulletData.h"
#include "ConstantUtilityData.h"
#include <nlohmann/json.hpp>


// ---------------------------------------------------------------------------------------
/* --- @:WeaponDataManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】武器のマスターパラメータを管理する
//      データの所有権 / 保持
//      読み取り専用データの提供
//
// ***************************************************************************************
class WeaponDataManager
{
private:

    /* 全ての武器データを管理 */
    std::unordered_map<int, std::unique_ptr<WeaponData::BaseWeaponData>> m_AllWeaponsDataMap;
    std::unordered_map<int, std::unique_ptr<WeaponData::BaseWeaponData>> m_EnemyWeaponsDataMap;

    /* レンジャーの武器カテゴリごとの検索用 */
    std::map<WeaponData::Ranger::PRIMARY_WEAPON_TYPE, std::vector<const WeaponData::BaseWeaponData*>> m_RangerCategoryViewMap;



public:
    WeaponDataManager();
    ~WeaponDataManager();

    /// <summary>
    /// 初期化
    /// </summary>
    bool Init();

    /// <summary>
    /// 指定IDの武器データを検索・取得する
    /// </summary>
    /// <param name="_id">武器ID</param>
    /// <returns>読み取り専用武器データ</returns>
    const WeaponData::BaseWeaponData* FindWeaponData(int _id)const;


    /// <summary>
    /// 指定IDの敵の武器データを検索・取得する
    /// </summary>
    /// <param name="_id">武器ID</param>
    /// <returns>読み取り専用武器データ</returns>
    const WeaponData::BaseWeaponData* FindEnemysWeaponData(int _id)const;



    bool LoadGunWeaponData(const std::string& filepath, WeaponData::GunWeaponData& outData);


private:
    // コピー禁止
    WeaponDataManager(const WeaponDataManager&) = delete;
    WeaponDataManager& operator=(const WeaponDataManager&) = delete;
    // ------------------------------------------------------

    /// <summary>
    /// 弾データの読み取り
    /// </summary>
    /// <param name="json"></param>
    /// <param name="outData"></param>
    /// <returns></returns>
    bool LoadBulletData(
        const nlohmann::json& json,
        BulletData::BulletDataBase& outData);    
    
    bool LoadMovementData(
        const nlohmann::json& json,
        BulletData::BulletDataBase& outData);
        
    bool LoadHitData(
        const nlohmann::json& json,
        BulletData::BulletDataBase& outData);

    bool LoadVisualData(
        const nlohmann::json& json,
        BulletData::BulletDataBase& outData);

    void LoadVEC3Data(const nlohmann::json& _json, const std::string& _tag, VECTOR3::VEC3& _outData);
};

