#include "pch.h"
#include "Shader.h"

//--------------------------------------------------------------------------------------
//      * ShaderManager Struct - コンストラクタ - *
//--------------------------------------------------------------------------------------
SHADER_DATA::SHADER_DATA()
{
	m_pVertexShader = nullptr;
	m_pPixelShader  = nullptr;
	m_pVertexLayout = nullptr;
}


//--------------------------------------------------------------------------------------
//      * ShaderManager Struct - デストラクタ - *
//--------------------------------------------------------------------------------------
SHADER_DATA::~SHADER_DATA()
{
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
}