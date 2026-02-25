#pragma once
#include "IMoveBehaviour.h"


// ***************************************************************************************
// ---------------------------------------------------------------------------------------
/* --- @:LinearMove_Behaviour Class --- */
//
//  šŒp³FIMoveBehaviour š
//
// y?z’¼üˆÚ“®‚Ì‹““®ƒNƒ‰ƒX
//
// ***************************************************************************************
class LinearMove_Behaviour : public IMoveBehaviour
{
private:



public:
	LinearMove_Behaviour();
	~LinearMove_Behaviour();

	ResultMove MoveCalculate(float _deltaTime, const MoveParam& _param, const class MyTransform& _transform) override;	// ˆÚ“®ŒvZ

};

