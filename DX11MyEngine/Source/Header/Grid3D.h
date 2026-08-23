#pragma once

// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:Grid3D Class --- */
//
// 【?】セルの管理
// 
// 参考サイト：
//	https://gameprogrammingpatterns.com/spatial-partition.html#drawing-battle-lines
//
// ***************************************************************************************
class Grid3D
{
public:
	static const int NUM_CELLS = 10;
	static const int CELL_SIZE = 20;

private:
	class Unit* m_pCells[NUM_CELLS][NUM_CELLS][NUM_CELLS];


public:
	Grid3D();
	~Grid3D();

	void Add(class Unit* _pUnit);
	void Move(class Unit* _pUnit, const VECTOR3::VEC3& _pos);
	float UnitDistanceSq(class Unit* _pUnitA, class Unit* _pUnitB);
};

