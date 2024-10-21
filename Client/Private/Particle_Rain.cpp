#include "stdafx.h"
#include "..\Public\Particle_Rain.h"

#include "GameInstance.h"

CParticle_Rain::CParticle_Rain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle_Rain::CParticle_Rain(const CParticle_Rain& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticle_Rain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Rain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	XMStoreFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr(), XMMatrixIdentity());

	return S_OK;
}

void CParticle_Rain::Priority_Update(_float fTimeDelta)
{

}

void CParticle_Rain::Update(_float fTimeDelta)
{
	m_pVIBufferCom->Drop(fTimeDelta);
}

void CParticle_Rain::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CParticle_Rain::Render()
{	
	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(13)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CParticle_Rain::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Rain"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */

	CVIBuffer_Point_Instance::POINT_DESC PointDesc = {};
	PointDesc.isRandomSize = false;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rain"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom),&PointDesc)))
		return E_FAIL;

	return S_OK;
}

CParticle_Rain* CParticle_Rain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Rain* pInstance = new CParticle_Rain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Rain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_Rain::Clone(void* pArg)
{
	CParticle_Rain* pInstance = new CParticle_Rain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Rain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Rain::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
