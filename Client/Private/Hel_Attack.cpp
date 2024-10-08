#include "stdafx.h"
#include "Hel_Attack.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Attack::CHel_Attack(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::ATTACK1 , pOwner }
{

}

HRESULT CHel_Attack::Initialize()
{
	return S_OK;
}

HRESULT CHel_Attack::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_Attack::Update(_float fTimeDelta)
{


}



void CHel_Attack::End_State()
{

}


CHel_Attack* CHel_Attack::Create(class CGameObject* pOwner)
{
	CHel_Attack* pInstance = new CHel_Attack(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Attack::Free()
{
	__super::Free();
}
