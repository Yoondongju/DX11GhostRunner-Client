#include "stdafx.h"
#include "..\Public\Particle_BigElectric.h"

#include "GameInstance.h"

#include "Hel.h"

CParticle_BigElectric::CParticle_BigElectric(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_BigElectric::CParticle_BigElectric(const CParticle_BigElectric& Prototype)
	: CPartObject{ Prototype }
{
}


void CParticle_BigElectric::Set_FirstActive(_bool b)
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


HRESULT CParticle_BigElectric::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_BigElectric::Initialize(void* pArg)
{
	PARTICLE_DESC* pDesc = static_cast<CParticle_BigElectric::PARTICLE_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketBoneMatrix;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	return S_OK;
}

void CParticle_BigElectric::Priority_Update(_float fTimeDelta)
{

}

void CParticle_BigElectric::Update(_float fTimeDelta)
{
	if (m_fDisableTime <= 0.f)
	{
		m_isActive = false;
		m_isFirstActive = false;
		m_fDisableTime = 2.f;
		m_pVIBufferCom->ResetTranslation();

		m_iRandomSpread = m_pGameInstance->Get_Random_Interger(0, 2);
	}
	m_fDisableTime -= fTimeDelta;


	m_pVIBufferCom->Spread_Side_XZ(fTimeDelta);
}

void CParticle_BigElectric::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CParticle_BigElectric::Render()
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

HRESULT CParticle_BigElectric::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BigElectricInstance"),  
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_BigElectric* CParticle_BigElectric::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_BigElectric* pInstance = new CParticle_BigElectric(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_BigElectric"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CParticle_BigElectric::Clone(void* pArg)
{
	CParticle_BigElectric* pInstance = new CParticle_BigElectric(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_BigElectric"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_BigElectric::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
