#include "stdafx.h"
#include "Hel_Jump.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Jump::CHel_Jump(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::IDLE_TO_JUMP , pOwner }
{

}

HRESULT CHel_Jump::Initialize()
{
	return S_OK;
}

HRESULT CHel_Jump::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_Jump::Update(_float fTimeDelta)
{


}



void CHel_Jump::End_State()
{

}


CHel_Jump* CHel_Jump::Create(class CGameObject* pOwner)
{
	CHel_Jump* pInstance = new CHel_Jump(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Jump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Jump::Free()
{
	__super::Free();
}
