#include "stdafx.h"
#include "..\Public\WeaponParticle.h"

#include "GameInstance.h"

CWeaponParticle::CWeaponParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeaponParticle::CWeaponParticle(const CWeaponParticle& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CWeaponParticle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeaponParticle::Initialize(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);
	m_pParentState = pDesc->pParentState;
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;


	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(70.f, 10.f, 70.f, 1.f));


	return S_OK;
}

void CWeaponParticle::Priority_Update(_float fTimeDelta)
{

}

void CWeaponParticle::Update(_float fTimeDelta)
{
	//_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
	//
	//for (size_t i = 0; i < 3; i++)
	//{
	//	SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	//}
	//XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));

	m_pVIBufferCom->Spread(fTimeDelta);

}

void CWeaponParticle::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CWeaponParticle::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeaponParticle::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CWeaponParticle* CWeaponParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeaponParticle* pInstance = new CWeaponParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeaponParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeaponParticle::Clone(void* pArg)
{
	CWeaponParticle* pInstance = new CWeaponParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeaponParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeaponParticle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
