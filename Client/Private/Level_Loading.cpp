#include "stdafx.h"
#include "..\Public\Level_Loading.h"


#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Stage1.h"
#include "Level_Stage1_Boss.h"

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
		Data.iDecorativeDataSize = m_pLoader->Get_Loading_DecorativeObject_Size(m_eNextLevelID);
		Data.pDecorativeData = m_pLoader->Get_Loading_DecorativeObject_Info(m_eNextLevelID);

		Data.iStaticDataSize = m_pLoader->Get_Loading_StaticObject_Size(m_eNextLevelID);
		Data.pStaticData = m_pLoader->Get_Loading_StaticObject_Info(m_eNextLevelID);

		Data.iDynamicDataSize = m_pLoader->Get_Loading_DynamicObject_Size(m_eNextLevelID);
		Data.pDynamicData = m_pLoader->Get_Loading_DynamicObject_Info(m_eNextLevelID);


		Data.iAnimDataSize = m_pLoader->Get_Loading_AnimObject_Size(m_eNextLevelID);
		Data.pAnimData = m_pLoader->Get_Loading_AnimObject_Info(m_eNextLevelID);


		Data.iTerrainDataSize = m_pLoader->Get_Loading_Terrain_Size(m_eNextLevelID);
		Data.pTerrainData = m_pLoader->Get_Loading_Terrain_Info(m_eNextLevelID);
		
		Data.pPlayerData = m_pLoader->Get_Loading_Player_Info();


		m_pGameInstance->Phys_Clear();
			
		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_GAMEPLAY:
			pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext,&Data);
			break;
		case LEVEL_STAGE1:
			pNewLevel = CLevel_Stage1::Create(m_pDevice, m_pContext, &Data);
			break;
		case LEVEL_STAGE1_BOSS:
			pNewLevel = CLevel_Stage1_Boss::Create(m_pDevice, m_pContext, &Data);
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
