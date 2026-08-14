#pragma once

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:ObjectFactory  --- */
//
// 【?】オブジェクトの種類を選択して、ある程度簡潔に外から生成できるようにする
//
// ***************************************************************************************
namespace ObjectFactory {

	// 共有ポインタの型定義
	using ObjectPtr = std::shared_ptr<GameObject>;

    //=========================================================================================
    //
	//						生成時に指定する情報
    //
    //=========================================================================================
    /// <summary>
    /// 生成するオブジェクトの情報
    /// </summary>
    struct BaseCreateObjectInfo
    {
        std::string _tag = "Empty";
    };


    //=========================================================================================
    //
    //						生成関数群
    //
    //=========================================================================================

    static ObjectPtr CreateEmpty(const BaseCreateObjectInfo& _info);   // 空オブジェクト

    /// <summary>
    /// 3Dオブジェクト生成関数群
    /// </summary>
    namespace Primitive3D
    {
        static ObjectPtr CreateCube();      // キューブ
        static ObjectPtr CreatePlane();     // プレーン
        static ObjectPtr CreateQuad();      // クアッド
        static ObjectPtr CreateModel();     // モデル
    };

    /// <summary>
    /// ライト生成関数群
    /// </summary>
    namespace Light
    {
        static ObjectPtr CreatePointLight();        // ポイントライト
		static ObjectPtr CreateDirectionalLight();  // ディレクショナルライト
    };

    /// <summary>
    /// UI生成関数群
    /// </summary>
    namespace UI
    {
        static ObjectPtr CreateSprite();            // スプライト
        static ObjectPtr CreateButton();            // ボタン
    };

    /// <summary>
    /// その他
    /// </summary>
    namespace Other
    {
		static ObjectPtr CreateSkyBox(const BaseCreateObjectInfo& _info);   // スカイボックス
    };
}
