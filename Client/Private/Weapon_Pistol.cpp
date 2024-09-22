#include "stdafx.h"
#include "..\Public\Weapon_Pistol.h"

#include "Player.h"

#include "GameInstance.h"
#include "Pistol.h"

CWeapon_Pistol::CWeapon_Pistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{

}

CWeapon_Pistol::CWeapon_Pistol(const CWeapon_Pistol& Prototype)
	: CPartObject{ Prototype }
{

}

HRESULT CWeapon_Pistol::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Pistol::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	//_float4x4 InitWorldMatrix = {
	//	0.181342006, 0.655698836, 0.207122847, 0.00000000,
	//	-0.963511229, 0.238839343, 0.0874769986, 0.00000000,
	//	0.0110353436, -0.301334143, 0.944287121, 0.00000000,
	//	0.0750860050, -0.0355550535, 0.0480655581, 1.00000000
	//};


	//m_pTransformCom->Set_WorldMatrix(InitWorldMatrix);

	

	return S_OK;
}

void CWeapon_Pistol::Priority_Update(_float fTimeDelta)
{

}

void CWeapon_Pistol::Update(_float fTimeDelta)
{

}

void CWeapon_Pistol::Late_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}



	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CWeapon_Pistol::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	

	_uint iPassNum = 0;
	CPistol* pPistol = static_cast<CPistol*>(m_pOwner);

	if (true == pPistol->IsDead())
	{
		iPassNum = 1;

		_float fDiscard = pPistol->Get_Discard();

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveAmount", &fDiscard, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pDeadNoiseTexture->Bind_ShadeResource(m_pShaderCom, "g_NoiseTexture", 0)))
			return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(iPassNum)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon_Pistol::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Pistol_Gun"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_DeadNoiseTexture"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDeadNoiseTexture), nullptr)))
		return E_FAIL;

	return S_OK;
}

CWeapon_Pistol* CWeapon_Pistol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Pistol* pInstance = new CWeapon_Pistol(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Pistol"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Pistol::Clone(void* pArg)
{
	CWeapon_Pistol* pInstance = new CWeapon_Pistol(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Pistol"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Pistol::Free()
{
	__super::Free();

	Safe_Release(m_pDeadNoiseTexture);

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pModelCom);
}
