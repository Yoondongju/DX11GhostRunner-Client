#include "stdafx.h"
#include "Hel_Dash.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Dash::CHel_Dash(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::DASH_BACK , pOwner }
{

}

HRESULT CHel_Dash::Initialize()
{
	return S_OK;
}

HRESULT CHel_Dash::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_Dash::Update(_float fTimeDelta)
{


}



void CHel_Dash::End_State()
{

}


CHel_Dash* CHel_Dash::Create(class CGameObject* pOwner)
{
	CHel_Dash* pInstance = new CHel_Dash(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Dash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Dash::Free()
{
	__super::Free();
}
