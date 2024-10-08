#include "stdafx.h"
#include "Hel_Raise.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Raise::CHel_Raise(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::RAISE , pOwner }
{

}

HRESULT CHel_Raise::Initialize()
{
	return S_OK;
}

HRESULT CHel_Raise::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_Raise::Update(_float fTimeDelta)
{


}

void CHel_Raise::End_State()
{

}


CHel_Raise* CHel_Raise::Create(class CGameObject* pOwner)
{
	CHel_Raise* pInstance = new CHel_Raise(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Raise"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Raise::Free()
{
	__super::Free();
}
