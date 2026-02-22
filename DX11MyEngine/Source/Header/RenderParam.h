#pragma once


//
/* 描画用のパラメータ管理 */
//
struct RenderParam
{
	CB_TRANSFORM_SET  cbTransformSet;	// ワールド変換用
	CB_VIEW_SET       cbViewSet;		// ビュー変換用
	CB_PROJECTION_SET cbProjectionSet;	// 投影変換用

	bool Init(RendererEngine& render);
	void Term(RendererEngine& render);

private:
	bool InitConstantBuffer(RendererEngine& render);
};

