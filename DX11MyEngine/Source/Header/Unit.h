#pragma once

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Grid3D Class --- */
//
// 【?】セルに登録される
//		双方向リストとして機能する
// 
// 参考サイト：
//	https://gameprogrammingpatterns.com/spatial-partition.html#drawing-battle-lines
//
// ***************************************************************************************
class Unit
{
	friend class Grid3D;
private:
	VECTOR3::VEC3 m_Pos;
	class Grid3D* m_pGrid;
	Unit* m_pPrev;
	Unit* m_pNext;

public:
	Unit(class Grid3D *_pGrid, const VECTOR3::VEC3& _pos);
	~Unit();

	void Move(const VECTOR3::VEC3& _pos);
};

