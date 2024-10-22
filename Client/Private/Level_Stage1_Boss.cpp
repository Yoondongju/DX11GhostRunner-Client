#include "stdafx.h"
#include "..\Public\Level_Stage1_Boss.h"

#include "FreeCamera.h"
#include "GameInstance.h"

#include "Level_Loading.h"

#include "GrapplingPointUI.h"
#include "IconUI.h"

#include "Particle_ShurikenEffect.h"


#include "Player.h"
#include "SubShuriken.h"

#include "Elite.h"

CLevel_Stage1_Boss::CLevel_Stage1_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Stage1_Boss::Initialize(void* pArg)
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Layer_MapObject(pArg)))			// 그랩UI가 만들어져 있어야 플레이어를 만들수있는데 그랩UI는 크래인이 만들고
		return E_FAIL;									// 플레이어를 만들고 나서 몬스터들을 만들어야하기에 여기서 나눠야겟다


	CGameObject::GAMEOBJECT_DESC Desc = {};
	Desc.fRotationPerSec = 20.f;
	Desc.fSpeedPerSec = 20.f;
	Desc.InitWorldMatrix = XMMatrixIdentity();


	Desc.iObjectType = CModel::TYPE::TYPE_NONANIM;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE1_BOSS, L"Layer_Map", L"Prototype_GameObject_Static_Object", L"Prototype_Component_MapModel", &Desc)))
		return E_FAIL;


	if (FAILED(Ready_Layer_Monster(pArg)))
		return E_FAIL;


	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));

	CTransform* pPlayerTransform = pPlayer->Get_Transform();
	_float4x4 InitWorldMatrix =
	{
		-0.999953628, -2.83807822e-08, 0.00937000755, 0.00000000,
		0.000709990622, 0.997123539, 0.0757577717, 0.00000000,
		-0.00934499968, 0.0757606402, -0.997079849, 0.00000000,
		535.953247, 51.9346428, -1229.84497, 1.00000000
	};

	pPlayerTransform->Set_WorldMatrix(InitWorldMatrix);

	*(_float3*)InitWorldMatrix.m[3] = _float3(0.f, 0.f, 0.f);

	_float4x4* pRotationMatrix = pPlayer->Get_RotationMatrixPtr();

	*pRotationMatrix = InitWorldMatrix;


	return S_OK;
}


void CLevel_Stage1_Boss::Update(_float fTimeDelta)
{
	if (false == m_pGameInstance->Check_IsPlaying(SOUND_SECOND_BGM))
	{
		m_pGameInstance->Play_Sound(TEXT("Train.ogg"), SOUND_SECOND_BGM, 0.f);
	}

	m_pGameInstance->SetChannelVolume(SOUND_SECOND_BGM, m_fTrainSoundAmount);

	if (m_fTrainSoundAmountTime >= 0.f &&
		m_fTrainSoundAmountTime < 4.5f)
	{
		m_fTrainSoundAmount += fTimeDelta * 0.25f;
	}
	else if (m_fTrainSoundAmountTime >= 4.5f &&
		m_fTrainSoundAmountTime < 9.f)
	{
		m_fTrainSoundAmount -= fTimeDelta * 0.25f;
	}
	else
	{
		m_fTrainSoundAmountTime = 0.f;
	}

	m_fTrainSoundAmountTime += fTimeDelta;




	if (m_pGameInstance->Get_KeyState(KEY::B) == KEY_STATE::TAP)
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE2_BOSS))))
			return;
	}
}


HRESULT CLevel_Stage1_Boss::Render()
{
	return S_OK;
}


HRESULT CLevel_Stage1_Boss::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};
	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);	// 광원이 쏘는 방향 
	LightDesc.vDiffuse = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(10.f, 3.f, 10.f, 1.f);		//  월드위치네
	LightDesc.fRange = 5.f;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = /*_float4(0.4f, 0.2f, 0.2f, 1.f);*/_float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(306.253815f, 70.0848236f, -3449.53833f, 1.f);		//  월드위치네
	LightDesc.fRange = 200.f;
	LightDesc.vDiffuse = _float4(1.f, 0.784f, 0.314f, 1.f);
	LightDesc.vAmbient = /*_float4(0.4f, 0.2f, 0.2f, 1.f);*/_float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	LIGHT_DESC* pLight = m_pGameInstance->Get_LightDesc(1);
	static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY))->Set_PointLight(pLight);


	return S_OK;
}




HRESULT CLevel_Stage1_Boss::Ready_Layer_BackGround(void* pArg)
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

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE1_BOSS, TEXT("Layer_BackGround"), pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE1_BOSS, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"), L"No Model")))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_MapObject(void* pArg)
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

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE1_BOSS, pData[i].strLayerName, pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
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

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE1_BOSS, pData[i].strLayerName, pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
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

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE1_BOSS, pData[i].strLayerName, pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CLevel_Stage1_Boss::Ready_Layer_Monster(void* pArg)
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


		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE1_BOSS, pAnimData[i].strLayerName, pAnimData[i].strPrototypeName, pAnimData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}

	return S_OK;
}




CLevel_Stage1_Boss* CLevel_Stage1_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CLevel_Stage1_Boss* pInstance = new CLevel_Stage1_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Stage1_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage1_Boss::Free()
{
	__super::Free();

}
