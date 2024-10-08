#include "stdafx.h"
#include "Hel_Stun.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Stun::CHel_Stun(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::STUN_HIT , pOwner }
{

}

HRESULT CHel_Stun::Initialize()
{
	return S_OK;
}

HRESULT CHel_Stun::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_Stun::Update(_float fTimeDelta)
{


}



void CHel_Stun::End_State()
{

}


CHel_Stun* CHel_Stun::Create(class CGameObject* pOwner)
{
	CHel_Stun* pInstance = new CHel_Stun(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Stun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Stun::Free()
{
	__super::Free();
}
