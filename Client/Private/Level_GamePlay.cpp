#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "FreeCamera.h"
#include "GameInstance.h"

#include "Level_Loading.h"

#include "GrapplingPointUI.h"
#include "IconUI.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize(void *pArg)
{

	if (FAILED(Ready_Lights()))
		return E_FAIL;


	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;


	if (FAILED(Ready_Layer_BackGround(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect()))
		return E_FAIL;



	if (FAILED(Ready_Layer_MapObject(pArg)))			// 그랩UI가 만들어져 있어야 플레이어를 만들수있는데 그랩UI는 크래인이 만들고
		return E_FAIL;									// 플레이어를 만들고 나서 몬스터들을 만들어야하기에 여기서 나눠야겟다


	if (FAILED(Ready_Layer_Player(pArg)))			
		return E_FAIL;


	if (FAILED(Ready_Layer_Monster(pArg)))
		return E_FAIL;
	

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;


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


HRESULT CLevel_GamePlay::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);	// 광원이 쏘는 방향 
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
	CFreeCamera::CAMERA_FREE_DESC		Desc{};

	Desc.fSensor = 0.2f;
	Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(90.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), TEXT("No Model Prototype") ,&Desc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_BackGround(void* pArg)
{

	CLevel_Loading::LOAD_DATA_DESC* pDesc = static_cast<CLevel_Loading::LOAD_DATA_DESC*>(pArg);

	_uint iTerrainDataSize = pDesc->iTerrainDataSize;
	const CLoader::LOADING_TERRAIN_INFO* pData = pDesc->pTerrainData;

	for (size_t i = 0; i < iTerrainDataSize; i++)
	{
		static CGameObject::GAMEOBJECT_DESC Desc = {};

		Desc.fRotationPerSec = 20.f;
		Desc.fSpeedPerSec = 20.f;
		Desc.InitWorldMatrix = XMLoadFloat4x4(&pData[i].vWorldMatrix);

 		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"),L"No Model")))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_WaterDrop"), TEXT("No Model"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObject(void* pArg)
{
	CLevel_Loading::LOAD_DATA_DESC* pDesc = static_cast<CLevel_Loading::LOAD_DATA_DESC*>(pArg);
	
	_uint iDecorativeDataSize = pDesc->iDecorativeDataSize;
	const CLoader::LOADING_OBJECT_INFO* pData = pDesc->pDecorativeData;

	for (size_t i = 0; i < iDecorativeDataSize; i++)		// Non Anim Object
	{
		CGameObject::GAMEOBJECT_DESC Desc = {};

		Desc.fRotationPerSec = 20.f;
		Desc.fSpeedPerSec = 20.f;
		Desc.InitWorldMatrix = XMLoadFloat4x4(&pData[i].vWorldMatrix);
		Desc.iObjectType = pData[i].eModelType;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, pData[i].strLayerName, pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}

	_uint iStaticDataSize = pDesc->iStaticDataSize;
	pData = pDesc->pStaticData;

	for (size_t i = 0; i < iStaticDataSize; i++)		// Non Anim Object
	{
		CGameObject::GAMEOBJECT_DESC Desc = {};

		Desc.fRotationPerSec = 20.f;
		Desc.fSpeedPerSec = 20.f;
		Desc.InitWorldMatrix = XMLoadFloat4x4(&pData[i].vWorldMatrix);
		Desc.iObjectType = pData[i].eModelType;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, pData[i].strLayerName, pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}


	_uint iDynamicDataSize = pDesc->iDynamicDataSize;
	pData = pDesc->pDynamicData;

	for (size_t i = 0; i < iDynamicDataSize; i++)		// Non Anim Object
	{
		CGameObject::GAMEOBJECT_DESC Desc = {};

		Desc.fRotationPerSec = 20.f;
		Desc.fSpeedPerSec = 20.f;
		Desc.InitWorldMatrix = XMLoadFloat4x4(&pData[i].vWorldMatrix);
		Desc.iObjectType = pData[i].eModelType;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, pData[i].strLayerName, pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}


	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(void* pArg)
{
	CLevel_Loading::LOAD_DATA_DESC* pDesc = static_cast<CLevel_Loading::LOAD_DATA_DESC*>(pArg);

	_uint iAnimDataSize = pDesc->iAnimDataSize;
	const CLoader::LOADING_OBJECT_INFO* pAnimData = pDesc->pAnimData;

	for (size_t i = 0; i < iAnimDataSize; i++)		// Non Anim Object
	{
		CGameObject::GAMEOBJECT_DESC Desc = {};

		Desc.fRotationPerSec = 20.f;
		Desc.fSpeedPerSec = 20.f;
		Desc.InitWorldMatrix = XMLoadFloat4x4(&pAnimData[i].vWorldMatrix);
		Desc.iObjectType = pAnimData[i].eModelType;


		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, pAnimData[i].strLayerName, pAnimData[i].strPrototypeName, pAnimData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(void* pArg)
{
	CLevel_Loading::LOAD_DATA_DESC* pDesc = static_cast<CLevel_Loading::LOAD_DATA_DESC*>(pArg);

	const CLoader::LOADING_OBJECT_INFO* pPlayerData = pDesc->pPlayerData;

	CGameObject::GAMEOBJECT_DESC Desc = {};
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fSpeedPerSec = 80.f;
	Desc.InitWorldMatrix = XMLoadFloat4x4(&pPlayerData[0].vWorldMatrix);	// 바디의 월드행렬이아니라 플레이어 본체의 월드행렬이여야 하잖아

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, pPlayerData[0].strLayerName, pPlayerData[0].strPrototypeName, L"Model Exist Parts", &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI()
{
	CIconUI::ICON_DESC Desc = {};

	Desc.eType = CIconUI::ICON_TYPE::CUTALL;
	Desc.fX = 80;
	Desc.fY = g_iWinSizeY - 100;
	Desc.fSizeX = 70;
	Desc.fSizeY = 70;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, L"Layer_UI", L"Prototype_GameObject_IconUI", L"No Model", &Desc)))
		return E_FAIL;


	Desc.eType = CIconUI::ICON_TYPE::BLOCK;
	Desc.fX = Desc.fX + 100;
	Desc.fY = g_iWinSizeY - 60;
	Desc.fSizeX = 70;
	Desc.fSizeY = 70;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, L"Layer_UI", L"Prototype_GameObject_IconUI", L"No Model", &Desc)))
		return E_FAIL;


	Desc.eType = CIconUI::ICON_TYPE::DASH;
	Desc.fX = Desc.fX + 100;
	Desc.fY = g_iWinSizeY - 100;
	Desc.fSizeX = 70;
	Desc.fSizeY = 70;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, L"Layer_UI", L"Prototype_GameObject_IconUI", L"No Model", &Desc)))
		return E_FAIL;


	// up UI
	Desc.eType = CIconUI::ICON_TYPE::MINDCONTROL;
	Desc.fX = Desc.fX;
	Desc.fY = g_iWinSizeY - 180;
	Desc.fSizeX = 70;
	Desc.fSizeY = 70;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, L"Layer_UI", L"Prototype_GameObject_IconUI", L"No Model", &Desc)))
		return E_FAIL;


	Desc.eType = CIconUI::ICON_TYPE::TIMESTOP;
	Desc.fX = Desc.fX - 100;
	Desc.fY = g_iWinSizeY - 220;
	Desc.fSizeX = 70;
	Desc.fSizeY = 70;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, L"Layer_UI", L"Prototype_GameObject_IconUI", L"No Model", &Desc)))
		return E_FAIL;


	Desc.eType = CIconUI::ICON_TYPE::NAMI;
	Desc.fX = Desc.fX - 100;
	Desc.fY = g_iWinSizeY - 180;
	Desc.fSizeX = 70;
	Desc.fSizeY = 70;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, L"Layer_UI", L"Prototype_GameObject_IconUI", L"No Model", &Desc)))
		return E_FAIL;


	// 마지막
	Desc.eType = CIconUI::ICON_TYPE::SHURIKEN;
	Desc.fX = Desc.fX + 100;
	Desc.fY = g_iWinSizeY - 140;
	Desc.fSizeX = 80;
	Desc.fSizeY = 80;
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, L"Layer_UI", L"Prototype_GameObject_IconUI", L"No Model", &Desc)))
		return E_FAIL;




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
