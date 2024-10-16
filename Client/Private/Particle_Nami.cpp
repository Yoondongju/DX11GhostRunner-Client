#include "stdafx.h"
#include "..\Public\Particle_Nami.h"

#include "GameInstance.h"

CParticle_Nami::CParticle_Nami(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_Nami::CParticle_Nami(const CParticle_Nami& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CParticle_Nami::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Nami::Initialize(void* pArg)
{
	PARTICLE_NAMIEFFECT* pDesc = static_cast<CParticle_Nami::PARTICLE_NAMIEFFECT*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;



	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CParticle_Nami::Priority_Update(_float fTimeDelta)
{


}

void CParticle_Nami::Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
	{
		if (m_fDisableTime <= 0.f)
		{
			m_isActiveMyParticle = false;
			m_fDisableTime = 2.f;
			m_pVIBufferCom->ResetTranslation();
		}

		m_fDisableTime -= fTimeDelta;

		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}

		_float3* pRight = (_float3*)m_pParentMatrix->m[0];
		_float3* pUp = (_float3*)m_pParentMatrix->m[1];
		_float3* pLook = (_float3*)m_pParentMatrix->m[2];
		_float3* pPos = (_float3*)m_pParentMatrix->m[3];


		XMStoreFloat4x4(&m_WorldMatrix, /*XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix*/  XMLoadFloat4x4(m_pParentMatrix));
				

		_vector vRightNor = XMVector3Normalize(XMLoadFloat3(pRight));
		_vector vUpNor = XMVector3Normalize(XMLoadFloat3(pUp));
		_vector vLookNor = XMVector3Normalize(XMLoadFloat3(pLook));

		_vector vPos = XMLoadFloat3(pPos);

		vPos += vRightNor * -0.6f;
		vPos += vUpNor * 14.f;
		vPos += vLookNor * 7.f;

		XMStoreFloat3((_float3*)m_WorldMatrix.m[3], XMVectorSetW(vPos,1.f));


		m_pVIBufferCom->Spread_Side_XY(fTimeDelta);
	}
}

void CParticle_Nami::Late_Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CParticle_Nami::Render()
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


	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Nami::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleNami"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_NamiEffect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_Nami* CParticle_Nami::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Nami* pInstance = new CParticle_Nami(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Nami"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_Nami::Clone(void* pArg)
{
	CParticle_Nami* pInstance = new CParticle_Nami(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Nami"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Nami::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
