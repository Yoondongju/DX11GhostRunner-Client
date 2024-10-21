#include "stdafx.h"
#include "..\Public\Level_Stage2_Boss.h"

#include "FreeCamera.h"
#include "GameInstance.h"

#include "Level_Loading.h"

#include "GrapplingPointUI.h"
#include "IconUI.h"

#include "Particle_ShurikenEffect.h"


#include "Player.h"
#include "SubShuriken.h"

#include "Hel.h"

CLevel_Stage2_Boss::CLevel_Stage2_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Stage2_Boss::Initialize(void* pArg)
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
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE2_BOSS, L"Layer_Map", L"Prototype_GameObject_Static_Object", L"Prototype_Component_MapModel", &Desc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE2_BOSS, L"Layer_BackGroundParticle", L"Prototype_GameObject_Particle_Rain", L"NoModel")))
		return E_FAIL;


	if (FAILED(Ready_Layer_Monster(pArg)))
		return E_FAIL;



	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));

	CTransform* pPlayerTransform = pPlayer->Get_Transform();


	//_vector vInitPos = { -5293.03174, 500.00000, 250.670639, 1.00000000 };
	//pPlayerTransform->Set_State(CTransform::STATE_POSITION, vInitPos);
	
	_float4x4 InitWorldMatrix =
	{
		0.0506078489, -4.36557457e-10, -0.998718739, 0.00000000,
		-0.0942385346, 0.995538235, -0.00477532251, 0.00000000,
		0.994262755, 0.0943594202, 0.0503820814, 0.00000000,
		-5293.03174,  500.00000,  250.670639, 1.00000000
	};
	
	pPlayerTransform->Set_WorldMatrix(InitWorldMatrix);
	
	*(_float3*)InitWorldMatrix.m[3] = _float3(0.f, 0.f, 0.f);
	
	_float4x4* pRotationMatrix = pPlayer->Get_RotationMatrixPtr();
	
	*pRotationMatrix = InitWorldMatrix;

	return S_OK;
}


void CLevel_Stage2_Boss::Update(_float fTimeDelta)
{
	
}


HRESULT CLevel_Stage2_Boss::Render()
{
	return S_OK;
}


HRESULT CLevel_Stage2_Boss::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */

	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);	// 광원이 쏘는 방향 
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;


	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	pPlayer->Set_BlockCoolTime(0.3f);


	
	return S_OK;
}




HRESULT CLevel_Stage2_Boss::Ready_Layer_BackGround(void* pArg)
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

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE2_BOSS, TEXT("Layer_BackGround"), pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE2_BOSS, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"), L"No Model")))
		return E_FAIL;


	return S_OK;
}



HRESULT CLevel_Stage2_Boss::Ready_Layer_MapObject(void* pArg)
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

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE2_BOSS, pData[i].strLayerName, pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
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

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE2_BOSS, pData[i].strLayerName, pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
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

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE2_BOSS, pData[i].strLayerName, pData[i].strPrototypeName, pData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CLevel_Stage2_Boss::Ready_Layer_Monster(void* pArg)
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


		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE2_BOSS, pAnimData[i].strLayerName, pAnimData[i].strPrototypeName, pAnimData[i].strModelPrototypeName, &Desc)))
			return E_FAIL;
	}

	return S_OK;
}




CLevel_Stage2_Boss* CLevel_Stage2_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CLevel_Stage2_Boss* pInstance = new CLevel_Stage2_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Stage2_Boss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage2_Boss::Free()
{
	__super::Free();

}
