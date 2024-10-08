#include "stdafx.h"
#include "Hel_Idle.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Idle::CHel_Idle(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::IDLE , pOwner }
{

}

HRESULT CHel_Idle::Initialize()
{
	return S_OK;
}

HRESULT CHel_Idle::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_Idle::Update(_float fTimeDelta)
{


}


void CHel_Idle::End_State()
{

}


CHel_Idle* CHel_Idle::Create(class CGameObject* pOwner)
{
	CHel_Idle* pInstance = new CHel_Idle(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Idle::Free()
{
	__super::Free();
}
