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
	T *m_pResource;

public:
	IDXResource() : m_pResource(nullptr) {};
	virtual ~IDXResource() = default;

	virtual bool Setup() = 0; 
	virtual void Term() = 0;

	T* get()const { return m_pResource; };
	void set(T* res) { m_pResource = res; }
};

