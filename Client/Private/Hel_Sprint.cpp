#include "stdafx.h"
#include "Hel_Sprint.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Sprint::CHel_Sprint(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::SPRINT , pOwner }
{

}

HRESULT CHel_Sprint::Initialize()
{
	return S_OK;
}

HRESULT CHel_Sprint::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_Sprint::Update(_float fTimeDelta)
{


}



void CHel_Sprint::End_State()
{

}


CHel_Sprint* CHel_Sprint::Create(class CGameObject* pOwner)
{
	CHel_Sprint* pInstance = new CHel_Sprint(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Sprint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Sprint::Free()
{
	__super::Free();
}
