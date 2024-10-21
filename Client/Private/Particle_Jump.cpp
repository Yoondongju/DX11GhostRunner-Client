#include "stdafx.h"
#include "..\Public\Particle_Jump.h"

#include "GameInstance.h"

CParticle_Jump::CParticle_Jump(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_Jump::CParticle_Jump(const CParticle_Jump& Prototype)
	: CPartObject{ Prototype }
{
}

void CParticle_Jump::SetActiveMyParticle(_bool b,_bool isOtherFlag)
{
	if (true == m_isActiveMyParticle && 
		true == b &&
		m_fDisableTime != 2.f)
	{	
		m_fDisableTime = 2.f;
		m_pVIBufferCom->ResetTranslation();
	}
	m_isActiveMyParticle = b;
}

HRESULT CParticle_Jump::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Jump::Initialize(void* pArg)
{
	PARTICLE_JUMPEFFECT* pDesc = static_cast<CParticle_Jump::PARTICLE_JUMPEFFECT*>(pArg);
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CParticle_Jump::Priority_Update(_float fTimeDelta)
{


}

void CParticle_Jump::Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
	{
		if (m_fDisableTime <= 0.f)
		{
			m_isActiveMyParticle = false;
			m_fDisableTime = 2.f;
			m_pVIBufferCom->ResetTranslation();
			return;
		}

		m_fDisableTime -= fTimeDelta;

		
		if (false == m_isFirstActive)
		{
			m_test = *(_float3*)m_WorldMatrix.m[3];
			m_isFirstActive = true;
		}
		m_pVIBufferCom->Spread_Side_XZ(fTimeDelta);		
	}
	else
	{
		m_isFirstActive = false;
		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	}
}

void CParticle_Jump::Late_Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CParticle_Jump::Render()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	*(_float3*)m_WorldMatrix.m[3] = m_test;

	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Jump::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_HelJump"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_HelJumpEffect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_Jump* CParticle_Jump::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Jump* pInstance = new CParticle_Jump(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Jump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_Jump::Clone(void* pArg)
{
	CParticle_Jump* pInstance = new CParticle_Jump(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Jump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Jump::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
