#pragma once


//* =========================================================================
//* - @:OBJECT_STATUS_BITFLAG列挙体 - */
//* 【?】オブジェクトの状態ビットフラグ
//* =========================================================================
enum class OBJECT_STATUS_BITFLAG : unsigned
{
	IS_ACTIVE		= 1 << 0,	// 1	アクティブか
	IS_DELETE		= 1 << 1,	// 2	削除するか
	IS_DONT_DESTROY	= 1 << 2,	// 4	削除しないでね
	IS_TRANSPARENT	= 1 << 3,	// 8	透明度があるか
	IS_TIME_DESTROY	= 1 << 4,	// 16	時間で削除するか
};


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Object Class --- */
//
//  ★★★抽象クラス★★★
//
// 【?】全てのオブジェクトの基底クラス

// ***************************************************************************************
class Object
{
private:
	std::string m_Tag;	// タグ
	
	// ↓ビットフラグに置き換える↓
	//bool m_IsActive;     // アクティブ状態フラグ(動いてるかどうか)
	//bool m_IsDeath;      // 死亡フラグ(まだ削除対象ではない)
	//bool m_IsDelete;     // Mngからの削除フラグ
	//bool m_IsDontDestroy;// 削除しないでーっていうやつ（Unityのシーンを跨いで存在するようなやつ）

	int m_LayerRank;       // 描画順を決めるために使うランク
	unsigned int m_ObjectStatusBitFlag;	// 状態をビット管理

public:
	Object();
	virtual ~Object() = default;


	const std::string &get_Tag()const { return m_Tag; }		// タグを取得
	void set_Tag(const std::string &tag) { m_Tag = tag; }   // タグを設定

	// ****************************************************************************************************************************************
	/* フラグ関連 */
	// ****************************************************************************************************************************************
	/*
	void set_IsDelete(const bool &flag)		 { m_IsDelete = flag; }                 // Deleteフラグをtrueにする
	void set_IsDeath(const bool &flag)		 { m_IsDeath = flag; }                  // 死亡フラグオンにする
	void set_IsActive(const bool &flag)		 { m_IsActive = flag; }					// アクティブフラグを設定
	void set_IsDontDestroy(const bool &flag) { m_IsDontDestroy = flag; }			// 削除しないでフラグを設定
	bool get_IsDelete()const	  { return m_IsDelete; }							// 削除フラグ取得
	bool get_IsDeath()const		  { return m_IsDeath; }								// 死亡フラグ取得
	bool get_IsActive()const	  { return m_IsActive; }							// アクティブフラグ取得
	bool get_IsDontDestroy()const { return m_IsDontDestroy; }						// 削除しないでフラグ取得
	*/

	void set_StatusFlag(OBJECT_STATUS_BITFLAG flag);	// 指定の状態フラグを立てる
	void clear_StatusFlag(OBJECT_STATUS_BITFLAG flag);	// 指定の状態フラグを降ろす
	bool get_IsStatusFlag(OBJECT_STATUS_BITFLAG flag);	// 指定の状態フラグが立っているかどうか
	void toggle_StatusFlag(OBJECT_STATUS_BITFLAG flag);	// 指定の状態フラグを反転


	// ****************************************************************************************************************************************
	/* 描画ランク */
	// ****************************************************************************************************************************************
	void set_LayerRank(int rank) { m_LayerRank = rank; }   // 描画ランクの設定
	int get_LayerRank() const { return m_LayerRank; }      // 描画ランクの取得
};




