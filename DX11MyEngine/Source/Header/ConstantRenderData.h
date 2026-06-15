#pragma once

/// <summary>
/// 描画用データ
/// </summary>
namespace RenderData
{
	/// <summary> 描画パス </summary>
	enum class RENDER_PASS
	{
		MAIN,   // 通常
		SHADOW, // シャドウ
	};

	/// <summary> カリングモード </summary>
	enum class CULL_MODE
	{
		NONE,
		FRONT,
		BACK,

		NUM,
	};

	/// <summary> レンダリングパイプラインの種類 </summary>
	enum class RENDER_PIPELINE_STATE
	{
		NONE,
		DEFAULT,
		DEFAULT_2D,

		NUM,
	};

	/// <summary> 描画コマンドタイプ </summary>
	enum class RENDER_COMMAND_TYPE
	{
		STATIC_MESH,
		SKINNED_MESH,
		SPRITE,
		BILLBORAD,
		DECAL,
	};

};