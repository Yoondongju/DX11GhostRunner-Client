#include "stdafx.h"
#include "Hel_RunJump.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_RunJump::CHel_RunJump(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::RUN_JUMP , pOwner }
{

}

HRESULT CHel_RunJump::Initialize()
{
	return S_OK;
}

HRESULT CHel_RunJump::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_RunJump::Update(_float fTimeDelta)
{


}



void CHel_RunJump::End_State()
{

}


CHel_RunJump* CHel_RunJump::Create(class CGameObject* pOwner)
{
	CHel_RunJump* pInstance = new CHel_RunJump(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_RunJump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_RunJump::Free()
{
	__super::Free();
}
