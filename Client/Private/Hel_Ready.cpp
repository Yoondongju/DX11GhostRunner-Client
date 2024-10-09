#include "stdafx.h"
#include "Hel_Ready.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Ready::CHel_Ready(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::READY , pOwner }
{

}

HRESULT CHel_Ready::Initialize()
{
	return S_OK;
}

HRESULT CHel_Ready::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_Ready::Update(_float fTimeDelta)
{
	
}



void CHel_Ready::End_State()
{

}


CHel_Ready* CHel_Ready::Create(class CGameObject* pOwner)
{
	CHel_Ready* pInstance = new CHel_Ready(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Ready"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Ready::Free()
{
	__super::Free();
}
