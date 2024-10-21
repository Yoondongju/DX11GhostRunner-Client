#include "stdafx.h"
#include "..\Public\Particle_Electric.h"

#include "GameInstance.h"

#include "Hel.h"

CParticle_Electric::CParticle_Electric(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_Electric::CParticle_Electric(const CParticle_Electric& Prototype)
	: CPartObject{ Prototype }
{
}


void CParticle_Electric::Set_FirstActive(_bool b)
{
	m_isFirstActive = b;

	if (true == m_isFirstActive)
	{
		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
	}
}


HRESULT CParticle_Electric::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Electric::Initialize(void* pArg)
{
	PARTICLE_DESC* pDesc = static_cast<CParticle_Electric::PARTICLE_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void CParticle_Electric::Priority_Update(_float fTimeDelta)
{

}

void CParticle_Electric::Update(_float fTimeDelta)
{
	if (m_fDisableTime <= 0.f)
	{
		m_isActive = false;
		m_isFirstActive = false;
		m_fDisableTime = 1.f;
		m_pVIBufferCom->ResetTranslation();

		m_iRandomSpread = m_pGameInstance->Get_Random_Interger(0, 2);
	}
	m_fDisableTime -= fTimeDelta;


	switch (m_iRandomSpread)
	{
	case 0:
		m_pVIBufferCom->Spread(fTimeDelta * 2.f);
		break;
	case 1:
		m_pVIBufferCom->Spread2(fTimeDelta * 2.f);
		break;
	case 2:
		m_pVIBufferCom->Spread3(fTimeDelta * 2.f);
		break;
	default:
		break;
	}

}

void CParticle_Electric::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CParticle_Electric::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;



	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Electric::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleBlock"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_ElectricInstance"), 
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_Electric* CParticle_Electric::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Electric* pInstance = new CParticle_Electric(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Electric"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CParticle_Electric::Clone(void* pArg)
{
	CParticle_Electric* pInstance = new CParticle_Electric(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Electric"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Electric::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
