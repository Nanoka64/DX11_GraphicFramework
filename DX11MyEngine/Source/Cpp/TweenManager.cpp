#include "pch.h"
#include "TweenManager.h"
#include "Tween.h"

using namespace Tool;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//*----------------------------------------------------------------------------------------
TweenManager::TweenManager()
{
}
//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
TweenManager::~TweenManager()
{
}

//*---------------------------------------------------------------------------------------
//*【?】初期化処理
//*
//* [引数] なし
//* [返値] 
//* true : 成功
//* false: 失敗
//*----------------------------------------------------------------------------------------
bool TweenManager::Init()
{
	return true;
}

//*---------------------------------------------------------------------------------------
//*【?】更新処理
//*
//* [引数] 
//* deltaTime : デルタタイム
//* 
//* [返値] なし
//*----------------------------------------------------------------------------------------
void TweenManager::Update(float deltaTime)
{
	//
	// Tweenの更新
	//
	for (auto& tweenVariant : m_Tweens)
	{
		std::visit(
			[deltaTime](auto& tween)
			{
				tween.Update(deltaTime);
			},
			tweenVariant);
	}

	//
	// 終了したTweenを削除
	//
	RemoveFinishedTweens();
}
//*---------------------------------------------------------------------------------------
//*【?】完了したTweenの削除
//*
//* [引数] なし
//* [返値] なし
//*----------------------------------------------------------------------------------------
void TweenManager::RemoveFinishedTweens()
{
	std::erase_if(
		m_Tweens,
		[](const TweenVariant& tweenVariant)
		{
			return std::visit(
				[](const auto& tween)
				{
					return tween.IsFinished();
				},
				tweenVariant);
		});
}

////*---------------------------------------------------------------------------------------
////*【?】Tweenの追加
////*
////* [引数] 
////* * _pTarget : Tweenの参照
////* _start     : 開始値
////* _end       : 終了値
////* _duration  : 継続時間
////* _tweenType : Tweenの種類
////* 
////* [返値] なし
////*----------------------------------------------------------------------------------------
//void TweenManager::AddTween(float* _pTarget, float _start, float _end, float _duration, Tool::TweenType _tweenType)
//{
//	Tween tween(_pTarget, _start, _end, _duration, _tweenType);
//	m_Tweens.push_back(tween);
//}
