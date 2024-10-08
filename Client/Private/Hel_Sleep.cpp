#include "stdafx.h"
#include "Hel_Sleep.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Sleep::CHel_Sleep(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::SLEEP , pOwner }
{

}

HRESULT CHel_Sleep::Initialize()
{
	return S_OK;
}

HRESULT CHel_Sleep::Start_State(void* pArg)
{


	return S_OK;
}



void CHel_Sleep::Update(_float fTimeDelta)
{

}



void CHel_Sleep::End_State()
{

}


CHel_Sleep* CHel_Sleep::Create(class CGameObject* pOwner)
{
	CHel_Sleep* pInstance = new CHel_Sleep(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Sleep"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Sleep::Free()
{
	__super::Free();
}
