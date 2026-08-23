#include "pch.h"
#include "Grid3D.h"
#include "Unit.h"

using namespace VECTOR3;

//*---------------------------------------------------------------------------------------
//*【?】コンストラクタ
//* [引数]
//* _pGrid : 所属するグリッド
//* _pos   : 位置
//*----------------------------------------------------------------------------------------
Grid3D::Grid3D()
{
    for (int x = 0; x < NUM_CELLS; x++)
    {
        for (int y = 0; y < NUM_CELLS; y++)
        {
            for (int z = 0; z < NUM_CELLS; z++)
            {
                m_pCells[x][y][z] = nullptr;
            }
        }
    }
}

//*---------------------------------------------------------------------------------------
//*【?】デストラクタ
//*----------------------------------------------------------------------------------------
Grid3D::~Grid3D()
{

}

//*---------------------------------------------------------------------------------------
//*【?】セルに追加
//*
//* [引数]
//* *_pUnit : 追加するユニット
//*
//* [返値]
//* なし 
//*----------------------------------------------------------------------------------------
void Grid3D::Add(class Unit* _pUnit)
{
    VEC3 pos = _pUnit->m_Pos;

    // セルの位置を求める
    int cellX = (int)(pos.x / Grid3D::CELL_SIZE);
    int cellY = (int)(pos.y / Grid3D::CELL_SIZE);
    int cellZ = (int)(pos.z / Grid3D::CELL_SIZE);

    // 範囲外のセルを指していたら終了
    if (NUM_CELLS <= cellX || cellX < 0 ||
        NUM_CELLS <= cellY || cellY < 0 ||
        NUM_CELLS <= cellZ || cellZ < 0)
    {
        assert(false);
        return;
    }

    // そのセルが属するリストの先頭に追加
    _pUnit->m_pPrev = nullptr;
    _pUnit->m_pNext = m_pCells[cellX][cellY][cellZ];
    m_pCells[cellX][cellY][cellZ] = _pUnit;

    if (_pUnit->m_pNext != nullptr)
    {
        _pUnit->m_pNext->m_pPrev = _pUnit;
    }
}

//*---------------------------------------------------------------------------------------
//*【?】移動
//*
//* [引数]
//* *_pUnit : 移動対象のユニット
//* _pos    : 設定する位置
//*
//* [返値]
//* なし 
//*----------------------------------------------------------------------------------------
void Grid3D::Move(class Unit* _pUnit, const VECTOR3::VEC3& _pos)
{
    VEC3 oldPos = _pUnit->m_Pos;

    // どのセルにあるか
    int oldCellX = (int)(oldPos.x / Grid3D::CELL_SIZE);
    int oldCellY = (int)(oldPos.y / Grid3D::CELL_SIZE);
    int oldCellZ = (int)(oldPos.z / Grid3D::CELL_SIZE);

    // どのセルに移動するか
    int newCellX = (int)(_pos.x / Grid3D::CELL_SIZE);
    int newCellY = (int)(_pos.y / Grid3D::CELL_SIZE);
    int newCellZ = (int)(_pos.z / Grid3D::CELL_SIZE);

    _pUnit->m_Pos = _pos;

    // セルの位置が変わっていないなら、終了
    if (oldCellX == newCellX &&
        oldCellY == newCellY &&
        oldCellZ == newCellZ){
        return;
    }

    // 以前のセルのリストからリンクを解除する
    if (_pUnit->m_pPrev != nullptr)
    {
        _pUnit->m_pPrev->m_pNext = _pUnit->m_pNext;
    }
    
    if (_pUnit->m_pNext != nullptr)
    {
        _pUnit->m_pNext->m_pPrev = _pUnit->m_pPrev;
    }

    // 先頭にいるなら、次のユニットと入れ替える
    if (m_pCells[oldCellX][oldCellY][oldCellZ] == _pUnit)
    {
        m_pCells[oldCellX][oldCellY][oldCellZ] = _pUnit->m_pNext;
    }

    // 新しいセルに追加
    Add(_pUnit);
}

//*---------------------------------------------------------------------------------------
//*【?】ユニット同士の距離を調べる
//*
//* [引数]
//* *_pUnitA : 対象ユニットA
//* *_pUnitB : 対象ユニットB
//*
//* [返値]
//* なし 
//*----------------------------------------------------------------------------------------
float Grid3D::UnitDistanceSq(class Unit* _pUnitA, class Unit* _pUnitB)
{
    return VEC3::Distance(_pUnitA->m_Pos, _pUnitB->m_Pos);
}
