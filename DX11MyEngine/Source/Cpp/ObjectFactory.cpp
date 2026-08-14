#include "pch.h"
#include "GameObject.h"
#include "ObjectFactory.h"
#include "Component_SkyRenderer.h"

//*---------------------------------------------------------------------------------------
//*【?】空オブジェクトの生成
//*
//* [引数]
//* ○○ : 
//*
//* [返値]
//* 生成したオブジェクトの共有ポインタ 
//*----------------------------------------------------------------------------------------
ObjectFactory::ObjectPtr ObjectFactory::CreateEmpty(const BaseCreateObjectInfo& _info)
{

}

//*---------------------------------------------------------------------------------------
//*【?】スカイボックスオブジェクトの作成
//*
//* [引数]
//* ○○ : 
//*
//* [返値]
//* 生成したオブジェクトの共有ポインタ 
//*----------------------------------------------------------------------------------------
ObjectFactory::ObjectPtr ObjectFactory::Other::CreateSkyBox(const BaseCreateObjectInfo& _info)
{

}
