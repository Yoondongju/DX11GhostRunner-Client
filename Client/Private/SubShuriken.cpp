#include "stdafx.h"
#include "..\Public\SubShuriken.h"

#include "Player.h"

#include "GameInstance.h"

CSubShuriken::CSubShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon_Player{ pDevice, pContext }
{
}

CSubShuriken::CSubShuriken(const CSubShuriken& Prototype)
	: CWeapon_Player{ Prototype }
{
}

HRESULT CSubShuriken::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSubShuriken::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	
	return S_OK;
}

void CSubShuriken::Priority_Update(_float fTimeDelta)
{

}

void CSubShuriken::Update(_float fTimeDelta)
{
	
}

void CSubShuriken::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSubShuriken::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;



	_uint		iNumMeshes = m_pModelCom[m_eCurType]->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom[m_eCurType]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom[m_eCurType]->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pColliderCom[m_eCurType]->Render();
#endif

	return S_OK;
}

HRESULT CSubShuriken::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Shuriken"),
		TEXT("Com_Shuriken"), reinterpret_cast<CComponent**>(&m_pModelCom[WEAPON_TYPE::SHURIKEN]))))
		return E_FAIL;



	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(2.f, 2.f, 11.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vExtents.z);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
		TEXT("Com_Collider_Katana"), reinterpret_cast<CComponent**>(&m_pColliderCom[WEAPON_TYPE::KATANA]), &ColliderDesc)))
		return E_FAIL;


	/* FOR.Com_Collider */
	ColliderDesc.vExtents = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vExtents.z);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
		TEXT("Com_Collider_Shuriken"), reinterpret_cast<CComponent**>(&m_pColliderCom[WEAPON_TYPE::SHURIKEN]), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

CSubShuriken* CSubShuriken::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSubShuriken* pInstance = new CSubShuriken(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSubShuriken"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSubShuriken::Clone(void* pArg)
{
	CSubShuriken* pInstance = new CSubShuriken(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSubShuriken"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSubShuriken::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);

	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
