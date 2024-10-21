#include "stdafx.h"
#include "..\Public\Particle_EliteDashBlock.h"

#include "GameInstance.h"

CParticle_EliteDashBlock::CParticle_EliteDashBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_EliteDashBlock::CParticle_EliteDashBlock(const CParticle_EliteDashBlock& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CParticle_EliteDashBlock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_EliteDashBlock::Initialize(void* pArg)
{
	PARTICLE_DASHBLOCKEFFECT* pDesc = static_cast<CParticle_EliteDashBlock::PARTICLE_DASHBLOCKEFFECT*>(pArg);


	m_SpwanPositionLocal = pDesc->SpwanPositionLocal;


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CParticle_EliteDashBlock::Priority_Update(_float fTimeDelta)
{


}

void CParticle_EliteDashBlock::Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
	{
		m_fUnActiveTime += fTimeDelta;
		if (m_fUnActiveTime >= 1.f)
		{
			m_fUnActiveTime = 0.f;
			m_pVIBufferCom->ResetTranslation();

			m_isActiveMyParticle = false;
		}

		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));
		_vector vWorldPos = XMVector3TransformCoord(XMLoadFloat3(&m_SpwanPositionLocal), XMLoadFloat4x4(&m_WorldMatrix));


		_float3* pLook = (_float3*)m_WorldMatrix.m[2];

		_vector vLookNor = XMVector3Normalize(XMLoadFloat3(pLook));

		m_WorldMatrix.m[3][0] = vWorldPos.m128_f32[0];
		m_WorldMatrix.m[3][1] = vWorldPos.m128_f32[1] + 45.f;
		m_WorldMatrix.m[3][2] = vWorldPos.m128_f32[2];

		XMStoreFloat3((_float3*)m_WorldMatrix.m[3], XMLoadFloat3((_float3*)m_WorldMatrix.m[3]) + vLookNor * 10.f);

		m_pVIBufferCom->Spread(fTimeDelta);
	}
}

void CParticle_EliteDashBlock::Late_Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CParticle_EliteDashBlock::Render()
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


	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_EliteDashBlock::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleBlock"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_EliteBlockDashEffect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_EliteDashBlock* CParticle_EliteDashBlock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_EliteDashBlock* pInstance = new CParticle_EliteDashBlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_EliteDashBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_EliteDashBlock::Clone(void* pArg)
{
	CParticle_EliteDashBlock* pInstance = new CParticle_EliteDashBlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_EliteDashBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_EliteDashBlock::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
