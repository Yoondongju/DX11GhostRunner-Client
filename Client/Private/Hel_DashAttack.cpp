#include "stdafx.h"
#include "Hel_DashAttack.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_DashAttack::CHel_DashAttack(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::DASH_TO_IDLE_ATTACK , pOwner }
{

}

HRESULT CHel_DashAttack::Initialize()
{
	return S_OK;
}

HRESULT CHel_DashAttack::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_DashAttack::Update(_float fTimeDelta)
{


}


void CHel_DashAttack::End_State()
{

}


CHel_DashAttack* CHel_DashAttack::Create(class CGameObject* pOwner)
{
	CHel_DashAttack* pInstance = new CHel_DashAttack(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_DashAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_DashAttack::Free()
{
	__super::Free();
}
