#pragma once


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:IDXResource Class --- */
//
//  ★★★抽象クラス★★★
//
// 【?】DirectXラップ用クラスの基底テンプレートクラス
//
// ***************************************************************************************
template<class T>
class IDXResource
{
protected:
	Microsoft::WRL::ComPtr<T> m_pResource;

public:
	IDXResource() : m_pResource(nullptr) {};
	virtual ~IDXResource() = default;

	virtual bool Setup() = 0; 
	virtual void Term() = 0;

	T* get()const { return m_pResource.Get(); };
	void set(Microsoft::WRL::ComPtr<T> res) { m_pResource = std::move(res); }
};

