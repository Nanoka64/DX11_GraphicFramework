#include "pch.h"
#include "WeaponDataManager.h"


#include <iostream>
#include <fstream>

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
    if (LoadGunWeaponData("Resource/WeaponsData/Flamethrower01.json", gunData) == false){
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
    
    // アリ
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
    
    // 八面体
    if (LoadGunWeaponData("Resource/EnemysData/OctahedronLaser01_EASY.json", gunData) == false) {     // インポッシブル用
        assert(false);
    }
    m_EnemyWeaponsDataMap[5] = std::make_unique<GunWeaponData>(gunData);





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
    _outData._level                 = j.value("level", -1);
    _outData._name                  = Tool::StringToWstring(j.value("name", "Unknown"));
    _outData._bulletMaxNum          = j.value("bulletMaxNum", 0);
    _outData._bulletSimultaneousNum = j.value("bulletSimultaneousNum", 1);
    _outData._fireRate              = j.value("fireRate", 0.0f);
    _outData._reloadTime            = j.value("reloadTime", 0.0f);
    _outData._accuracy              = j.value("accuracy", 0.0f);
    _outData._zoomLength            = j.value("zoomLength", 0.0f);
    _outData._isLaserSight          = j.value("isLaserSight", false);
    _outData._soundID               = j.value("soundID", -1);
    _outData._muzzleFlashEffectTag  = j.value("muzzleFlashEffectTag", "");

    if (j["muzzleFlashEffectScale"].is_array()) {                                                         // 大きさ
        _outData._muzzleFlashEffectScale.x = j["muzzleFlashEffectScale"][0].get<float>();
        _outData._muzzleFlashEffectScale.y = j["muzzleFlashEffectScale"][1].get<float>();
        _outData._muzzleFlashEffectScale.z = j["muzzleFlashEffectScale"][2].get<float>();
    }


    //*****************************************************************************************
    //	ここから↓					弾情報自体の読み取り
    //*****************************************************************************************
    // 弾の種類を文字列から判定
    std::string typeStr = j.value("bulletType", "NORMAL");
    if (!j.contains("bulletParam") || !j["bulletParam"].is_object())
    {
        return false;
    }
    const json& paramJson = j["bulletParam"];
    
    // 通常弾・爆発弾で共通して使うデータ
    BulletDataBase bulletData;
    if (!LoadBulletData(paramJson, bulletData))
    {
        return false;
    }

    // 完成した弾データを武器へ格納
    _outData._bulletData = std::move(bulletData);

    return true;
}


//*---------------------------------------------------------------------------------------
//*【?】弾データの読み取り
//*
//* [引数] 
//* &_json : json
//* &_outData : 出力先 
//* [返値]
//* true : 読みとり成功
//* false : 読みとり失敗
//*----------------------------------------------------------------------------------------
bool WeaponDataManager::LoadBulletData(const nlohmann::json& _json, BulletData::BulletDataBase& _outData)
{
    //*****************************************************************************************
    //						共通データの読み取り
    //*****************************************************************************************
    _outData._commonData._damage              = _json.value("damage", 0.0f);              // ダメージ
    _outData._commonData._speed               = _json.value("speed", 0.0f);               // 速度
    _outData._commonData._acceleration        = _json.value("acceleration", 0.0f);        // 加速度
    _outData._commonData._range               = _json.value("range", 0.0f);               // 射程
    _outData._commonData._penetrationsCount   = _json.value("penetrationsCount", 0);      // 貫通可能回数
    _outData._commonData._collisionSize       = _json.value("collisionSize", 0.0f);       // 当たり判定
    _outData._commonData._gravityScale        = _json.value("gravityScale", 0.0f);        // 重力
    _outData._commonData._knockbackForce      = _json.value("knockbackForce", 0.0f);      // 吹っ飛び力

    //bulletData._moveType = BULLET_MOVE_TYPE::LINEAR;    // 一旦直進のみ

    // 衝突マスク
    _outData._commonData._collisionMask = 0;
    if (_json.contains("collisionMask") &&
        _json["collisionMask"].is_array())
    {
        // 配列を回してOR演算
        for (const auto& maskJson : _json["collisionMask"])
        {
            const std::string maskName = maskJson.get<std::string>();
            const auto categoryIt = g_CollisionCategoryMap.find(maskName);

            // 未定義衝突カテゴリ
            if (categoryIt == g_CollisionCategoryMap.end())
            {
                return false;
            }

            _outData._commonData._collisionMask |= UINT_CAST(categoryIt->second);
        }
    }

    //*****************************************************************************************
    //						移動データの読み取り
    //*****************************************************************************************
    if (!LoadMovementData(_json, _outData))
    {
        return false;
    }

    //*****************************************************************************************
    //						ヒットデータの読み取り
    //*****************************************************************************************
    if (!LoadHitData(_json, _outData))
    {
        return false;
    }

    //*****************************************************************************************
    //						見た目データの読み取り
    //*****************************************************************************************
    if (!LoadVisualData(_json, _outData))
    {
        return false;
    }

    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】[移動データ] の読み取り
//*     
//* [引数] 
//* &_json : json
//* &_outData : 出力先 
//* [返値]
//* true : 読みとり成功
//* false : 読みとり失敗
//*----------------------------------------------------------------------------------------
bool WeaponDataManager::LoadMovementData(const nlohmann::json& _json, BulletData::BulletDataBase& _outData)
{
    const std::string type =
        _json.value("type", "LINEAR");

    if (type == "LINEAR")
    {
        _outData._moveData.emplace<LinearMoveData>();
    }
    else if (type == "HOMING")
    {
        auto& homing                = _outData._moveData.emplace<HomingMoveData>();
        homing._turnSpeed           = _json.value("turnSpeed", 0.0f);
        homing._targetingDuration   = _json.value("targetingDuration", 0.0f);
        homing._targetingStartDelay = _json.value("targetingStartDelay", 0.0f);
    }
    else
    {
        return false;
    }
    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】[見た目データ] の読み取り
//*
//* [引数] 
//* &_json : json
//* &_outData : 出力先 
//* [返値]
//* true : 読みとり成功
//* false : 読みとり失敗
//*----------------------------------------------------------------------------------------
bool WeaponDataManager::LoadVisualData(const nlohmann::json& _json, BulletData::BulletDataBase& _outData)
{
    using namespace BulletData;

    const std::string archetype = _json.value("archetype", "BILLBOARD");

    if (archetype == "BILLBOARD")
    {
        _outData._visualData._visualArchetype =  BULLET_VISUAL_ARCHETYPE::BILLBOARD;
    }
    else if (archetype == "MODEL")
    {
        _outData._visualData._visualArchetype =   BULLET_VISUAL_ARCHETYPE::MODEL;
    }
    else
    {
        return false;
    }

    _outData._visualData._bulletMaterialTag = _json.value("materialTag", "");           // マテリアルタグ
    _outData._visualData._enableTrail       = _json.value("enableTrail", false);        // トレイルの有無
    _outData._visualData._enableFlightSmoke = _json.value("enableFlightSmoke", false);  // 飛行煙の有無
    _outData._visualData._smokeInterval     = _json.value("smokeInterval", 0.05f);      // 飛行煙の間隔
    _outData._visualData._smokeEffectTag    = _json.value("smokeEffectTag", "");        // 煙エフェクトのタグ
    _outData._visualData._bulletMaterialTag = _json.value("bulletMaterialTag", "");     // 弾そのもののマテリアル
    LoadVEC3Data(_json, "trailColor", _outData._visualData._trailColor);                // トレイルカラー
    LoadVEC3Data(_json, "scale", _outData._visualData._scale);                          // 弾の大きさ
}

//*---------------------------------------------------------------------------------------
//*【?】[ヒットデータ] の読み取り
//*
//* [引数] 
//* &_json : json
//* &_outData : 出力先 
//* [返値]
//* true : 読みとり成功
//* false : 読みとり失敗
//*----------------------------------------------------------------------------------------
bool WeaponDataManager::LoadHitData(const nlohmann::json& _json, BulletData::BulletDataBase& _outData)
{
    using namespace BulletData;

    const std::string type = _json.value("type", "DIRECT");

    if (type == "DIRECT")
    {
        auto& direct                = _outData._hitData.emplace<DirectHitData>();
        direct._decalMaterialTag    = _json.value("decalMaterialTag", "");          // デカールタグ
        direct._hitEffectTag        = _json.value("hitEffectTag", "");              // ヒットエフェクトタグ
    }
    else if (type == "EXPLOSION")
    {
        auto& explosion                     = _outData._hitData.emplace<ExplosionHitData>();
        explosion._decalMaterialTag         = _json.value("decalMaterialTag", "");             // デカールタグ
        explosion._hitEffectTag             = _json.value("hitEffectTag", "");                 // ヒットエフェクトタグ
        explosion._explosionRadius          = _json.value("explosionRadius", 0.0f);            // 爆発半径 
        explosion._explosionEffectHandleTag = _json.value("explosionEffectTag", "");           // 爆発エフェクトタグ
        explosion._explosionEffectAliveTime = _json.value("explosionEffectAliveTime", 1.0f);   // 爆発エフェクトの生存時間（1.0でそのまま）
        explosion._isSmoke                  = _json.value("isSmoke", false);                   // 煙が出るか
    }
    else
    {
        return false;
    }

    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】VEC3 型の読み取り
//*
//* [引数] 
//* &_json : json
//* &_tag : タグ
//* &_outData : 出力先 
//* [返値]
//* true : 読みとり成功
//* false : 読みとり失敗
//*----------------------------------------------------------------------------------------
void WeaponDataManager::LoadVEC3Data(const nlohmann::json& _json, const std::string& _tag,VECTOR3::VEC3& _outData)
{
    if (_json.contains(_tag) &&                                                      // 弾の大きさ
        _json[_tag].is_array() &&
        _json[_tag].size() == 3)
    {
        _outData.x = _json[_tag][0].get<float>();
        _outData.y = _json[_tag][1].get<float>();
        _outData.z = _json[_tag][2].get<float>();
    }
}