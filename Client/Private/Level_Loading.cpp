#include "stdafx.h"
#include "..\Public\Level_Loading.h"


#include "Level_Logo.h"
#include "Level_GamePlay.h"

#include "GameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;	
		

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{	
	if (//(GetKeyState(VK_SPACE) & 0x8000) && 
		true == m_pLoader->isFinished())
	{
		CLevel*			pNewLevel = { nullptr };


		LOAD_DATA_DESC Data;
		Data.iDataSize = m_pLoader->Get_Loading_Object_Size(m_eNextLevelID);
		Data.pData = m_pLoader->Get_Loading_Object_Info(m_eNextLevelID);


		Data.iAnimDataSize = m_pLoader->Get_Loading_AnimObject_Size(m_eNextLevelID);
		Data.pAnimData = m_pLoader->Get_Loading_AnimObject_Info(m_eNextLevelID);


		Data.iTerrainDataSize = m_pLoader->Get_Loading_Terrain_Size(m_eNextLevelID);
		Data.pTerrainData = m_pLoader->Get_Loading_Terrain_Info(m_eNextLevelID);
		
		Data.pPlayerData = m_pLoader->Get_Loading_Player_Info(m_eNextLevelID);

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
			pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext,&Data);
			break;
		}

		if (nullptr == pNewLevel)
			return;

		if (FAILED(m_pGameInstance->Change_Level(m_eNextLevelID, pNewLevel)))
			return;
	}	
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Draw_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_BackGround"),
	//	TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
