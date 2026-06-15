#include "pch.h"
#include "WeaponDataManager.h"


#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace WeaponData;
using namespace UtilityData;
using namespace BulletData;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
WeaponDataManager::WeaponDataManager()
{
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
WeaponDataManager::~WeaponDataManager()
{
    m_AllWeaponsDataMap.clear();
}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数] なし
//*
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool WeaponDataManager::Init()
{
    GunWeaponData gunData;

    // スタンダード ************************************************************************************
    if (LoadGunWeaponData("Resource/WeaponsData/AssultRifle01.json", gunData) == false){
        assert(false);
    }
    m_AllWeaponsDataMap[0] = std::make_unique<GunWeaponData>(gunData);

    if (LoadGunWeaponData("Resource/WeaponsData/Launcher01.json", gunData) == false){
        assert(false);
    }
    m_AllWeaponsDataMap[1] = std::make_unique<GunWeaponData>(gunData);
    
    // ラピッド ************************************************************************************
    if (LoadGunWeaponData("Resource/WeaponsData/Flamethrower02.json", gunData) == false){
        assert(false);
    }
    m_AllWeaponsDataMap[2] = std::make_unique<GunWeaponData>(gunData);
    
    if (LoadGunWeaponData("Resource/WeaponsData/Launcher02.json", gunData) == false){
        assert(false);
    }
    m_AllWeaponsDataMap[3] = std::make_unique<GunWeaponData>(gunData);

    // スカウト ************************************************************************************
    if (LoadGunWeaponData("Resource/WeaponsData/AssultRifle02.json", gunData) == false) {
        assert(false);
    }
    m_AllWeaponsDataMap[4] = std::make_unique<GunWeaponData>(gunData);

    if (LoadGunWeaponData("Resource/WeaponsData/Sniper01.json", gunData) == false) {
        assert(false);
    }
    m_AllWeaponsDataMap[5] = std::make_unique<GunWeaponData>(gunData);
     
    // ヘビー ************************************************************************************
    if (LoadGunWeaponData("Resource/WeaponsData/Shotgun01.json", gunData) == false) {
        assert(false);
    }
    m_AllWeaponsDataMap[6] = std::make_unique<GunWeaponData>(gunData);

    if (LoadGunWeaponData("Resource/WeaponsData/Launcher03.json", gunData) == false) {
        assert(false);
    }
    m_AllWeaponsDataMap[7] = std::make_unique<GunWeaponData>(gunData);


    // 敵の武器 ************************************************************************************
    if (LoadGunWeaponData("Resource/EnemysData/AntAcid01_EASY.json", gunData) == false) {           // イージー用
        assert(false);
    }
    m_EnemyWeaponsDataMap[0] = std::make_unique<GunWeaponData>(gunData);
    if (LoadGunWeaponData("Resource/EnemysData/AntAcid01_NORMAL.json", gunData) == false) {         // ノーマル用
        assert(false);
    }
    m_EnemyWeaponsDataMap[1] = std::make_unique<GunWeaponData>(gunData);
    if (LoadGunWeaponData("Resource/EnemysData/AntAcid01_HARD.json", gunData) == false) {           // ハード用
        assert(false);
    }
    m_EnemyWeaponsDataMap[2] = std::make_unique<GunWeaponData>(gunData);
    if (LoadGunWeaponData("Resource/EnemysData/AntAcid01_DISASTER.json", gunData) == false) {       // ディザスター用
        assert(false);
    }
    m_EnemyWeaponsDataMap[3] = std::make_unique<GunWeaponData>(gunData);
    if (LoadGunWeaponData("Resource/EnemysData/AntAcid01_IMPOSSIBLE.json", gunData) == false) {     // インポッシブル用
        assert(false);
    }
    m_EnemyWeaponsDataMap[4] = std::make_unique<GunWeaponData>(gunData);





    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】指定IDの武器データを検索・取得する
//*
//* [引数] 武器ID
//*
//* [返値]
//* 読み取り専用武器データ
//* nullptr:見つからなかった 
//*----------------------------------------------------------------------------------------
const WeaponData::BaseWeaponData* WeaponDataManager::FindWeaponData(int _id)const
{
    auto it = m_AllWeaponsDataMap.find(_id);
    if (it != m_AllWeaponsDataMap.end())
    {
        // unique_ptr の中身の生ポインタ（const）を返す
        return it->second.get();
    }

    // 見つからなかった場合はnullptr
    return nullptr;
}

//*---------------------------------------------------------------------------------------
//*【?】指定IDの敵の武器データを検索・取得する
//*
//* [引数] 武器ID
//*
//* [返値]
//* 読み取り専用武器データ
//* nullptr:見つからなかった 
//*----------------------------------------------------------------------------------------
const WeaponData::BaseWeaponData* WeaponDataManager::FindEnemysWeaponData(int _id)const
{
    auto it = m_EnemyWeaponsDataMap.find(_id);
    if (it != m_EnemyWeaponsDataMap.end())
    {
        // unique_ptr の中身の生ポインタ（const）を返す
        return it->second.get();
    }

    // 見つからなかった場合はnullptr
    return nullptr;
}

//*---------------------------------------------------------------------------------------
//*【?】武器のデータ読み込み（json）
//*
//* [引数] 
//* filepath : jsonのファイルパス
//* &outData : 出力先 
//* [返値]
//* 読み取り専用武器データ
//* nullptr:見つからなかった 
//*----------------------------------------------------------------------------------------
bool WeaponDataManager::LoadGunWeaponData(const std::string& _filepath, WeaponData::GunWeaponData& _outData)
{
    using json = nlohmann::json;
    std::ifstream ifs(_filepath);
    if (!ifs.is_open()) return false;

    json j;
    ifs >> j;

    _outData.Reset();

    // ベースデータの読み込み
    // .value("キー", 初期値) を使うことで、キーがない場合の安全策をとる
    _outData._level = j.value("level", -1);
    _outData._name = Tool::StringToWstring(j.value("name", "Unknown"));
    _outData._bulletMaxNum = j.value("bulletMaxNum", 0);
    _outData._bulletSimultaneousNum = j.value("bulletSimultaneousNum", 1);
    _outData._fireRate = j.value("fireRate", 0.0f);
    _outData._reloadTime = j.value("reloadTime", 0.0f);
    _outData._accuracy = j.value("accuracy", 0.0f);
    _outData._zoomLength = j.value("zoomLength", 0.0f);
    _outData._isLaserSight = j.value("isLaserSight", false);
    _outData._soundID = j.value("soundID", -1);
    _outData._muzzleFlashEffectTag = j.value("muzzleFlashEffectTag", "");

    if (j["muzzleFlashEffectScale"].is_array()) {                                                         // 大きさ
        _outData._muzzleFlashEffectScale.x = j["muzzleFlashEffectScale"][0].get<float>();
        _outData._muzzleFlashEffectScale.y = j["muzzleFlashEffectScale"][1].get<float>();
        _outData._muzzleFlashEffectScale.z = j["muzzleFlashEffectScale"][2].get<float>();
    }

    // 弾の種類を文字列から判定
    std::string typeStr = j.value("bulletType", "NORMAL");
    const json& paramJson = j["bulletParam"];
    
    //////////////////////////////////////////////////////////////////////////////////////////
    //
    //						通常弾
    // 
    //////////////////////////////////////////////////////////////////////////////////////////
    if (typeStr == "NORMAL")
    {
        _outData._bulletType = BULLET_TYPE::NORMAL;

        NormalBulletData normalData;
        normalData._damage = paramJson.value("damage", 0.0f);                                       // ダメージ値
        normalData._damageDistAttenuationRate = paramJson.value("damageDistAttenuationRate", 0.0f); // ダメージ減衰
        normalData._speed = paramJson.value("speed", 0.0f);                                         // 速度
        normalData._acceleration = paramJson.value("acceleration", 0.0f);                           // 加速度
        normalData._range = paramJson.value("range", 0.0f);                                         // 射程
        normalData._penetrationsCount = paramJson.value("penetrationsCount", 0);                    // 貫通可能回数
        normalData._collisionSize = paramJson.value("collisionSize", 0.0f);                         // 当たり判定
        normalData._gravityScale = paramJson.value("gravityScale", 0.0f);                           // 重力
        normalData._knockbackForce = paramJson.value("knockbackForce", 0.0f);                       // 吹っ飛び力

        if (paramJson.contains("collisionMask") && paramJson["collisionMask"].is_array()) {         // 衝突マスク
            // 衝突マスク（配列を回してOR演算）
            for (const auto& maskStr : paramJson["collisionMask"]) {
                normalData._collisionMask |= UINT_CAST(g_CollisionCategoryMap[maskStr.get<std::string>()]);
            }
        }
        normalData._bulletMaterialTag = paramJson.value("bulletMaterialTag", "");                   // 弾そのもののマテリアル
        normalData._decalMaterialTag = paramJson.value("decalMaterialTag", "");                     // デカールに使うマテリアル
        normalData._hitEffectTag = paramJson.value("hitEffectTag", "");                             // ヒットエフェクト

        if (paramJson["scale"].is_array()){                                                         // 大きさ
            normalData._scale.x = paramJson["scale"][0].get<float>();
            normalData._scale.y = paramJson["scale"][1].get<float>();
            normalData._scale.z = paramJson["scale"][2].get<float>();
        }

        // variantに代入
        _outData._bulletParam = normalData;
    }
    //////////////////////////////////////////////////////////////////////////////////////////
    //
    //						爆発弾
    // 
    //////////////////////////////////////////////////////////////////////////////////////////
    else if (typeStr == "EXPLOSION")
    {
        _outData._bulletType = BULLET_TYPE::EXPLOSION;

        ExplosionBulletData expData;
        expData._damage = paramJson.value("damage", 0.0f);
        expData._damageDistAttenuationRate = paramJson.value("damageDistAttenuationRate", 0.0f);
        expData._speed = paramJson.value("speed", 0.0f);
        expData._acceleration = paramJson.value("acceleration", 0.0f);
        expData._range = paramJson.value("range", 0.0f);
        expData._penetrationsCount = paramJson.value("penetrationsCount", 0);
        expData._collisionSize = paramJson.value("collisionSize", 0.0f);
        expData._gravityScale = paramJson.value("gravityScale", 0.0f);
        expData._knockbackForce = paramJson.value("knockbackForce", 0.0f);                       // 吹っ飛び力

        if (paramJson.contains("collisionMask") && paramJson["collisionMask"].is_array()) {
            // 衝突マスク（配列を回してOR演算）
            for (const auto& maskStr : paramJson["collisionMask"]) {
                expData._collisionMask |= UINT_CAST(g_CollisionCategoryMap[maskStr.get<std::string>()]);
            }
        }

        expData._bulletMaterialTag = paramJson.value("bulletMaterialTag", "");                  // 弾そのもののマテリアル
        expData._decalMaterialTag = paramJson.value("decalMaterialTag", "");                    // デカールマテリアル
        expData._hitEffectTag = paramJson.value("hitEffectTag", "");                            // ヒットエフェクト

        if (paramJson["scale"].is_array()) {                                                         // 大きさ
            expData._scale.x = paramJson["scale"][0].get<float>();
            expData._scale.y = paramJson["scale"][1].get<float>();
            expData._scale.z = paramJson["scale"][2].get<float>();
        }

        // 派生クラス独自のパラメータを読み込み
        expData._explosionRadius = paramJson.value("explosionRadius", 0.0f);                        // 爆発半径
        expData._explosionEffectHandleTag = paramJson.value("explosionEffectHandleTag", "");        // 爆発エフェクトタグ
        expData._explosionEffectAliveTime = paramJson.value("explosionEffectAliveTime", 1.0f);      // 爆発エフェクトの生存時間（1.0でそのまま）
        expData._isSmoke = paramJson.value("isSmoke", false);                                       // 煙が出るか

        // variantに代入
        _outData._bulletParam = expData;
    }
    // 他の弾タイプ(EXPLOSION_DELAY, HORMING等)も同様に分岐を追加...

    return true;
}