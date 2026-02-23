#pragma once
#include <iostream>
#include <type_traits>

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


		/* 加藤君のコードだよ */
		template<class bitType>
		struct BIT_FLAG_STRUCT
		{
			bitType bit;

			operator bitType() { return &bit; }

			bool Bool() { return bit != (bitType)0; }

			bool IsFlag(bitType flag) { return (bit & flag) != (bitType)0; }
		};
		// ビットフラグ用構造体
		template<typename T, typename = typename std::enable_if<std::is_integral<T>::value &&std::is_unsigned<T>::value>::type>
		struct BIT_FLAG
		{
		public:
			T flags;// フラグ

			BIT_FLAG() : flags((T)0) {}
			BIT_FLAG(T flag) : flags(flag) {}

			operator T &() {
				return this->flags;
			}

			bool operator &(T flag) const {
				return ((this->flags & flag) != 0);
			}

			/*初期化*/
			inline void Init() { flags = (T)0; }
			/*全ビット有効*/
			inline void SetAllBit() { flags = ~((T)0); }

			/*1のビットがあれば「true」を返す*/
			inline bool Bool() const {
				return this->flags != 0;
			}

			inline unsigned long long BitSize()
			{
				return sizeof(T) * 8ULL;
			}

			/*-----【指定ビット設定】-----*/
			inline void SetFlag(bool flag, int number) {
				if (flag) {
					EnableFlag(number);
				}
				else
				{
					DisableFlag(number);
				}
			}
			/*【指定ビット設定】*/
			template<typename ENUM_T>
			inline void SetFlag(bool flag, ENUM_T number) { SetFlag(flag, (int)number); }
			/*----------------------------*/


			/*-----【指定ビット有効化】-----*/
			inline void EnableFlag(int number) {
				if (CheckNumber(number)) {
					flags |= ((T)1 << number);
				}
			}
			/*【指定ビット有効化】*/
			template<typename ENUM_T>
			inline void EnableFlag(ENUM_T number) { EnableFlag((int)number); }
			/*------------------------------*/


			/*-----【指定ビット無効化】-----*/
			inline void DisableFlag(int number) {
				if (CheckNumber(number)) {
					flags &= ~((T)1 << number);
				}
			}
			/*【指定ビット無効化】*/
			template<typename ENUM_T>
			inline void DisableFlag(ENUM_T number) { DisableFlag((int)number); }
			/*------------------------------*/


			/*-----【指定ビット反転】-----*/
			inline void InvertFlag(int number) {
				if (CheckNumber(number)) {
					flags ^= ((T)1 << number);
				}
			}
			/*【指定ビット反転】*/
			template<typename ENUM_T>
			inline void InvertFlag(ENUM_T number) { InvertFlag((int)number); }
			/*----------------------------*/


			/*-----【指定ビット取得】-----*/
			inline bool GetFlag(int number) const {
				if (CheckNumber(number)) {
					return ((flags & ((T)1 << number)) != 0);
				}
				return false;
			}
			/*【指定ビット取得】*/
			template<typename ENUM_T>
			inline bool GetFlag(ENUM_T number) const { return GetFlag((int)number); }
			/*----------------------------*/

			/*-----【指定ビット数から数字を設定する】-----*/
			inline void SetNumber(T setNumber, T numberZone, int number)
			{
				if (CheckNumber(number)) {
					flags &= (setNumber << number) | ~(numberZone << number);
				}
			}
			/*【指定ビット数から数字を設定する】*/
			template<typename ENUM_T>
			inline void SetNumber(T setNumber, T numberZone, ENUM_T number) const { SetNumber(setNumber, numberZone, (int)number); }
			/*--------------------------------------------*/

			/*-----【指定ビット数から数字を取得する】-----*/
			inline T GetNumber(T numberZone, int number, int rightBitNumber = -1)
			{
				if (rightBitNumber == -1)
				{
					rightBitNumber = number;
				}

				if (CheckNumber(number)) {
					return (flags & (numberZone << number)) >> rightBitNumber;
				}
				return T(0);
			}
			/*【指定ビット数から数字を設定する】*/
			template<typename ENUM_T>
			inline T GetNumber(T numberZone, ENUM_T number, ENUM_T rightBitNumber = (ENUM_T)-1) const { return GetNumber(numberZone, (int)number, (int)rightBitNumber); }
			/*--------------------------------------------*/

			/*Tのビット数を超えてないかを確認する(超えていれば「false」を返す)*/
			static inline bool CheckNumber(int number) {
				if (((sizeof(T) * 8) > number) && (number >= 0)) {
					return true;
				}
				return false;
			}

			/*指定のビットだけ1の整数を取得*/
			inline static BIT_FLAG BIT_GET(int number) { return BIT_FLAG((T)1 << number); }
		};
	};
};