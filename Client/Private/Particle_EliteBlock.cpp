#include "stdafx.h"
#include "..\Public\Particle_EliteBlock.h"

#include "GameInstance.h"

CParticle_EliteBlock::CParticle_EliteBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_EliteBlock::CParticle_EliteBlock(const CParticle_EliteBlock& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CParticle_EliteBlock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_EliteBlock::Initialize(void* pArg)
{
	PARTICLE_BLOCKEFFECT* pDesc = static_cast<CParticle_EliteBlock::PARTICLE_BLOCKEFFECT*>(pArg);


	m_SpwanPositionLocal = pDesc->SpwanPositionLocal;


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CParticle_EliteBlock::Priority_Update(_float fTimeDelta)
{


}

void CParticle_EliteBlock::Update(_float fTimeDelta)
{
	if (true == m_isPreActive && false == m_isActiveMyParticle)
	{
		m_pVIBufferCom->ResetTranslation();
		m_isPreActive = false;
	}


	if (m_isActiveMyParticle)
	{	
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));
		_vector vWorldPos = XMVector3TransformCoord(XMLoadFloat3(&m_SpwanPositionLocal), XMLoadFloat4x4(&m_WorldMatrix));


		m_WorldMatrix.m[3][0] = vWorldPos.m128_f32[0];
		m_WorldMatrix.m[3][1] = vWorldPos.m128_f32[1] + 45.f;
		m_WorldMatrix.m[3][2] = vWorldPos.m128_f32[2];

		m_pVIBufferCom->Spread(fTimeDelta);

		m_isPreActive = true;
	}
}

void CParticle_EliteBlock::Late_Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CParticle_EliteBlock::Render()
{
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


	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_EliteBlock::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleEliteBlock"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_EliteBlockEffect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_EliteBlock* CParticle_EliteBlock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_EliteBlock* pInstance = new CParticle_EliteBlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_EliteBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_EliteBlock::Clone(void* pArg)
{
	CParticle_EliteBlock* pInstance = new CParticle_EliteBlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_EliteBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_EliteBlock::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
