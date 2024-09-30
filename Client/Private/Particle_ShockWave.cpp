#include "stdafx.h"
#include "..\Public\Particle_ShockWave.h"

#include "GameInstance.h"

CParticle_ShockWave::CParticle_ShockWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_ShockWave::CParticle_ShockWave(const CParticle_ShockWave& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CParticle_ShockWave::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_ShockWave::Initialize(void* pArg)
{
	SHOCKWAVE_DESC* pDesc = static_cast<CParticle_ShockWave::SHOCKWAVE_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void CParticle_ShockWave::Priority_Update(_float fTimeDelta)
{

}

void CParticle_ShockWave::Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
	{
		if (m_fDisableTime <= 0.f)
		{
			m_isActiveMyParticle = false;
			m_fDisableTime = 3.f;
			m_pVIBufferCom->ResetTranslation();
		}

		m_fDisableTime -= fTimeDelta;

		
		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));



		_float3* pRight = (_float3*)m_pParentMatrix->m[0];
		_float3* pUp = (_float3*)m_pParentMatrix->m[1];
		_float3* pLook = (_float3*)m_pParentMatrix->m[2];
		_float3* pPos = (_float3*)m_pParentMatrix->m[3];

		_vector vRightNor = XMVector3Normalize(XMLoadFloat3(pRight));
		_vector vUpNor = XMVector3Normalize(XMLoadFloat3(pUp));
		_vector vLookNor = XMVector3Normalize(XMLoadFloat3(pLook));

		_vector vPos = XMLoadFloat3(pPos);

		// vPos += vRightNor * -0.6f;
		vPos += vUpNor * 30.f;
		// vPos += vLookNor * 5.f;

		XMStoreFloat3((_float3*)m_WorldMatrix.m[3], XMVectorSetW(vPos, 1.f));

		m_pVIBufferCom->Spread_Side(fTimeDelta);
	}
}

void CParticle_ShockWave::Late_Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_ShockWave::Render()
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


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CParticle_ShockWave::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_ShockWave"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_ShockWaveEffect"),  
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_ShockWave* CParticle_ShockWave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_ShockWave* pInstance = new CParticle_ShockWave(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_ShockWave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_ShockWave::Clone(void* pArg)
{
	CParticle_ShockWave* pInstance = new CParticle_ShockWave(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_ShockWave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_ShockWave::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
