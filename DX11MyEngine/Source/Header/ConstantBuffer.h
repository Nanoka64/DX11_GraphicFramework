#pragma once
#include "IConstantBuffer.h"

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:ConstantBuffer Class --- */
//
//  ★継承：IComponent ★
//
// 【?】定数バッファのラップクラス
//		
// ***************************************************************************************
template<typename T>
class ConstantBuffer : public IConstantBuffer
{
private:
	D3D11_USAGE m_Usage;							// 使用方法

public:
	// コンパイル時に16バイトアライメントをチェック
	static_assert((sizeof(T) % 16) == 0, "Constant Buffer struct must be 16-byte aligned.");

	//*---------------------------------------------------------------------------------------
	//*【?】コンストラクタ
	//*----------------------------------------------------------------------------------------
	ConstantBuffer() : 
		IConstantBuffer(),
		m_Usage(D3D11_USAGE_DEFAULT)
	{
	}

	//*---------------------------------------------------------------------------------------
	//*【?】デストラクタ
	//*----------------------------------------------------------------------------------------
	~ConstantBuffer() {
		Release();
	}

	/// <summary>
	/// 解放
	/// </summary>
	void Release() override
	{
		if (m_pResource != nullptr) {
			m_pResource->Release();
			//m_pResource = nullptr;
		}
	}
	
	/// <summary>
	/// セットアップ
	/// </summary>
	/// <param name="_device">デバイス</param>
	/// <param name="_usage">使用方法</param>
	/// <returns></returns>
	bool Setup(ID3D11Device* _device, const D3D11_USAGE _usage = D3D11_USAGE_DEFAULT)override
	{
		m_Usage = _usage;

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = m_Usage;
		bd.ByteWidth = sizeof(T);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = m_Usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;	// 動的変更する場合はCPUアクセスフラグをセット

		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuff;

		// バッファの作成
		HRESULT hr = _device->CreateBuffer(&bd, nullptr, pBuff.GetAddressOf());

		// 作成に成功したら、メンバ変数にセット
		this->set(pBuff);
		
		if (FAILED(hr)) {
			return false;
		}
		return true;
	}


	/// <summary>
	/// 実際に外から呼び出される更新関数
	/// </summary>
	/// <param name="context">コンテキスト</param>
	/// <param name="data">データ</param>
	/// <param name="size">サイズ</param>
	void UpdateRaw(ID3D11DeviceContext* _context, const void* _data, size_t _size) override
	{
		// サイズチェック
		assert(_size == sizeof(T));

		this->Update(_context, *(static_cast<const T*>(_data)));
	}

	/// <summary>
	/// 定数バッファをシェーダにセット
	/// </summary>
	/// <param name="context">コンテキスト</param>
	/// <param name="slot">スロット</param>
	void Bind(ID3D11DeviceContext* _context, UINT _slot) override {
		ID3D11Buffer* buffers[] = { m_pResource.Get() };
		_context->VSSetConstantBuffers(_slot, 1, buffers);
		_context->PSSetConstantBuffers(_slot, 1, buffers);
	}

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="_context">コンテキスト</param>
	/// <param name="_data">データ</param>
	void Update(ID3D11DeviceContext* _context, const T& _data)
	{
		switch (m_Usage)
		{
			/* デフォルト */
		case D3D11_USAGE_DEFAULT:
			_context->UpdateSubresource(m_pResource.Get(), 0, nullptr, &_data, 0, 0);
			break;
		
		case D3D11_USAGE_IMMUTABLE:
			break;

			/* 動的書き込み */
		case D3D11_USAGE_DYNAMIC:
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if (SUCCEEDED(_context->Map(m_pResource.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
			{
				memcpy(mappedResource.pData, &_data, sizeof(T));
				_context->Unmap(m_pResource.Get(), 0);
			}
		}
			break;
		
		case D3D11_USAGE_STAGING:
			break;
		
		default:
			break;
		}
	}
};

