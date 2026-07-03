#pragma once


// ---------------------------------------------------------------------------------------
/* --- @:RandomManager Class --- */
//
//  ★★★シングルトン★★★
//
// 【?】ランダムの管理
//		乱数生成にrand()ではなく、標準ライブラリのstd::randomを使う
// 
// 参考サイト：https://runebook.dev/ja/docs/cpp/numeric/random/rand
//
// ***************************************************************************************
class RandomManager
{
private:
	std::mt19937 m_RandomEngine; // 乱数生成エンジン（メルセンヌ・ツイスタ）

public:
	RandomManager();
	~RandomManager();

	bool Init();

	int GetIntRandom(int _min, int _max);
	float GetFloatRandom(float _min, float _max);
    VECTOR3::VEC3 GetVEC3Random(const VECTOR3::VEC3& _min, const VECTOR3::VEC3& _max);
	bool GetBoolRandom(float _trueProbability = 0.5f); // trueの確率を指定してboolを返す

private:
	// コピー禁止
	RandomManager(const RandomManager&) = delete;
	RandomManager& operator=(const RandomManager&) = delete;
	// ------------------------------------------------------
};

