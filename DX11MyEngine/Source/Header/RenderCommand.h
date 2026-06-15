#pragma once

namespace RenderData
{
	/// <summary> 描画に必要なデータ </summary>
	struct RenderCommandData
	{
		// --- 共通の描画リソース ---
		ID3D11Buffer* _vertexBuffer;
		ID3D11Buffer* _indexBuffer;
		UINT _indexCount;

		class Material* _material;
		SHADER_TYPE _shaderType;

		//std::variant<CB_TRANSFORM> _data;
	};
};