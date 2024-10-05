#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "FreeCamera.h"
#include "GameInstance.h"

#include "Level_Loading.h"

#include "GrapplingPointUI.h"
#include "IconUI.h"

#include "Particle_ShurikenEffect.h"


#include "Player.h"
#include "SubShuriken.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize(void *pArg)
{

	return S_OK;
}


void CLevel_GamePlay::Update(_float fTimeDelta)
{
	
}


HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	return S_OK;
}


CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
