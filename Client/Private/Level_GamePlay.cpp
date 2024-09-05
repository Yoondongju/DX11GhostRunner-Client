#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "FreeCamera.h"
#include "GameInstance.h"

#include "Level_Loading.h"

#include "GrapplingPointUI.h"

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


 	if (FAILED(Ready_Layer_MapObject(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;


	if (FAILED(Ready_Layer_Player(pArg)))			// �÷��̾�� Hook���� Grap�� �˾ƾ��ϱ⿡ �ʰԻ����ؾ��Ѵ�.
		return E_FAIL;

	return S_OK;
}


void CLevel_GamePlay::Update(_float fTimeDelta)
{
	
}


HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("�����÷��̷����Դϴ�."));
	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Lights()
{
	/* �����÷��� ������ �ʿ��� ������ �غ��Ѵ�. */
	
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);	// ������ ��� ���� 
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
	//for (size_t i = 0; i < 50; i++)
	//{
	//	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Effect"))))
	//		return E_FAIL;
	//}
	

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
	Desc.InitWorldMatrix = XMLoadFloat4x4(&pPlayerData[0].vWorldMatrix);	// �ٵ��� ��������̾ƴ϶� �÷��̾� ��ü�� ��������̿��� ���ݾ�

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, pPlayerData[0].strLayerName, pPlayerData[0].strPrototypeName, L"Model Exist Parts", &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI()
{
	

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
