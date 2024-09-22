#include "stdafx.h"
#include "..\Public\WeaponParticle.h"

#include "GameInstance.h"
#include "Player.h"

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
	
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	


	return S_OK;
}

void CWeaponParticle::Priority_Update(_float fTimeDelta)
{

}



void CWeaponParticle::Update(_float fTimeDelta)
{
	return;

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));





	CWeapon_Player* pWeapon = static_cast<CWeapon_Player*>(static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON));
	_float4x4* pWeaponMatrix = pWeapon->Get_PartObjectComBindWorldMatrixPtr();

	_float3 StartLocal = {};
	_float3 EndLocal = {};
	_matrix WeaponMatrix = {};


	switch (pWeapon->Get_CurType())
	{
	case CWeapon_Player::KATANA:
	{
		m_bActive = true;
		StartLocal = m_KatanaStartLocal;
		EndLocal = m_KatanaEndLocal;

		WeaponMatrix = XMLoadFloat4x4(pWeaponMatrix);

		if (false == m_bActive)
			return;

		m_fAccTime += fTimeDelta;

	

		XMStoreFloat3(&m_CurStart, XMVector3Transform(XMLoadFloat3(&StartLocal), WeaponMatrix));
		XMStoreFloat3(&m_CurEnd, XMVector3Transform(XMLoadFloat3(&EndLocal), WeaponMatrix));

		

		m_pVIBufferCom->Update_Trail(fTimeDelta, XMLoadFloat3(&m_PreStart), XMLoadFloat3(&m_PreEnd), XMLoadFloat3(&m_CurStart), XMLoadFloat3(&m_CurEnd));

		if (m_fAccTime > 0.05f)
		{
			m_PreStart = m_CurStart;
			m_PreEnd = m_CurEnd;

			m_fAccTime = 0.f;
		}	
	}
	break;
	case CWeapon_Player::SHURIKEN:
	{
		if (false == m_bActive)
			return;

		m_bActive = false;
	}
	break;
	}

	


}

void CWeaponParticle::Late_Update(_float fTimeDelta)
{
	return;

	if (false == m_bActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CWeaponParticle::Render()
{
	_float4x4* pWeaponMatrix = static_cast<CWeapon_Player*>(static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_PartObjectComBindWorldMatrixPtr();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", pWeaponMatrix)))
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSwordTrail"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleShuriken"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Trail"),
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
