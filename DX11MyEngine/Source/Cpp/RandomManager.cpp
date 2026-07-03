#include "pch.h"
#include "RandomManager.h"

using namespace VECTOR3;
using namespace VECTOR2;
using namespace VECTOR4;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
RandomManager::RandomManager()
{

}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
RandomManager::~RandomManager()
{

}

//*---------------------------------------------------------------------------------------
//*【?】初期化
//*
//* [引数] 
//* なし
//* [返値]
//* true : 成功
//* false : 失敗
//*----------------------------------------------------------------------------------------
bool RandomManager::Init()
{
    // シード値生成
    std::random_device seed_gen;

    // ランダム生成エンジンの初期化
    m_RandomEngine.seed(seed_gen());

    return true;
}

//*---------------------------------------------------------------------------------------
//*【?】Int型のランダム値取得
//*
//* [引数] 
//* _min : 最小値
//* _max : 最大値
//* [返値]
//* int : ランダム値
//*----------------------------------------------------------------------------------------
int RandomManager::GetIntRandom(int _min, int _max)
{
    std::uniform_int_distribution<int>dist(_min, _max);
    return dist(m_RandomEngine);
}

//*---------------------------------------------------------------------------------------
//*【?】float型のランダム値取得
//*
//* [引数] 
//* _min : 最小値
//* _max : 最大値
//* [返値]
//* float : ランダム値
//*----------------------------------------------------------------------------------------
float RandomManager::GetFloatRandom(float _min, float _max)
{
    std::uniform_real_distribution<float>dist(_min, _max);
    return dist(m_RandomEngine);
}


//*---------------------------------------------------------------------------------------
//*【?】VEC3型のランダム値取得
//*
//* [引数] 
//* _min : 最小値
//* _max : 最大値
//* [返値]
//* float : ランダム値
//*----------------------------------------------------------------------------------------
VECTOR3::VEC3 RandomManager::GetVEC3Random(const VECTOR3::VEC3& _min, const VECTOR3::VEC3& _max)
{
    std::uniform_real_distribution<float>distX(_min.x, _max.x);
    std::uniform_real_distribution<float>distY(_min.y, _max.y);
    std::uniform_real_distribution<float>distZ(_min.z, _max.z);
    VEC3 result;
    
    result.x = distX(m_RandomEngine);
    result.y = distY(m_RandomEngine);
    result.z = distZ(m_RandomEngine);

    return result;
}

//*---------------------------------------------------------------------------------------
//*【?】trueかfalseをランダム取得
//*
//* [引数]
//* _trueProbability : trueの確率（0.0～1.0） 
//* [返値]
//* float : ランダム値
//*----------------------------------------------------------------------------------------
bool RandomManager::GetBoolRandom(float _trueProbability)
{
	std::bernoulli_distribution dist(_trueProbability);
	return dist(m_RandomEngine);
}