#include "stdafx.h"
#include "Hel_BackBlock.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_BackBlock::CHel_BackBlock(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::BACK_BLOCK , pOwner }
{

}

HRESULT CHel_BackBlock::Initialize()
{
	return S_OK;
}

HRESULT CHel_BackBlock::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_BackBlock::Update(_float fTimeDelta)
{
	

}



void CHel_BackBlock::End_State()
{

}


CHel_BackBlock* CHel_BackBlock::Create(class CGameObject* pOwner)
{
	CHel_BackBlock* pInstance = new CHel_BackBlock(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_BackBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_BackBlock::Free()
{
	__super::Free();
}
