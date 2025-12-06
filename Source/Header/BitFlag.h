#pragma once

namespace GIGA_Engine
{
	namespace BitFlag
	{
		/// <summary>
		/// フラグを立てる
		/// </summary>
		/// <typeparam name="Type"></typeparam>
		/// <param name="_flag">確認するフラグ</param>
		/// <param name="outFlag">出力フラグの参照</param>
		template<typename Type>
		void SetFlag(Type _flag, Type &outFlag)
		{
			outFlag |= _flag;
		};

		/// <summary>
		/// フラグを降ろす
		/// </summary>
		/// <typeparam name="Type">確認するフラグ</typeparam>
		/// <param name="_flag"></param>
		/// <param name="outFlag"> 出力フラグの参照 </param>
		template<typename Type>
		void UnsetFlag(Type _flag, Type& outFlag)
		{
			outFlag &= ~_flag;
		};

		/// <summary>
		/// フラグを反転させる
		/// </summary>
		/// <typeparam name="Type"></typeparam>
		/// <param name="_flag">確認するフラグ</param>
		/// <param name="outFlag"> 出力フラグの参照 </param>
		template<typename Type>
		void ToggleFlag(Type _flag, Type& outFlag)
		{
			outFlag ^= _flag;
		};	

		/// <summary>
		/// フラグが立っているかどうか
		/// </summary>
		/// <typeparam name="Type"></typeparam>
		/// <param name="_flag">確認するフラグ</param>
		/// <param name="serchFlag">確認するフラグの参照</param>
		/// <returns></returns>
		template<typename Type>
		bool IsFlag(Type _flag, Type serchFlag)
		{
			return (serchFlag & _flag) != 0;
		};
	};
};