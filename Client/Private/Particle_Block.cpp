#include "stdafx.h"
#include "..\Public\Particle_Block.h"

#include "GameInstance.h"

#include "Player.h"

CParticle_Block::CParticle_Block(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_Block::CParticle_Block(const CParticle_Block& Prototype)
	: CPartObject{ Prototype }
{
}

void CParticle_Block::SetActiveMyParticle(_bool b, _bool isOtherFlag)
{
	m_isActiveMyParticle = b;
	m_isRefraction = isOtherFlag;

	m_pGameInstance->Play_Sound(TEXT("PlayerBlock.mp3"), SOUND_PLAYEREFFECT, 3.f);
}

HRESULT CParticle_Block::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Block::Initialize(void* pArg)
{
	PARTICLE_BLOCKEFFECT* pDesc = static_cast<CParticle_Block::PARTICLE_BLOCKEFFECT*>(pArg);

	
	m_SpwanPositionLocal = pDesc->SpwanPositionLocal;


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CParticle_Block::Priority_Update(_float fTimeDelta)
{
	

}

void CParticle_Block::Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
	{
		if(true == m_isRefraction)
			m_pGameInstance->ActiveRefraction(static_cast<CPlayer*>(m_pOwner)->Get_TimeStopRefractionTex(),CRenderer::REFRACTION_TYPE::BLOCK , static_cast<CPlayer*>(m_pOwner)->Get_BlockMaskRefractionTex());

		if (m_fDisableTime <= 0.f)
		{
			m_pGameInstance->UnActiveRefraction();

			m_isActiveMyParticle = false;
			m_isRefraction = false;
			m_fDisableTime = 1.f;
			m_pVIBufferCom->ResetTranslation();
		}
		m_fDisableTime -= fTimeDelta;
		
		
	
		_float3* pRight = (_float3*)m_pParentMatrix->m[0];
		_float3* pUp = (_float3*)m_pParentMatrix->m[1];
		_float3* pLook = (_float3*)m_pParentMatrix->m[2];
		_float3* pPos = (_float3*)m_pParentMatrix->m[3];


		XMStoreFloat4x4(&m_WorldMatrix, /*XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix*/  XMLoadFloat4x4(m_pParentMatrix));


		_vector vRightNor = XMVector3Normalize(XMLoadFloat3(pRight));
		_vector vUpNor = XMVector3Normalize(XMLoadFloat3(pUp));
		_vector vLookNor = XMVector3Normalize(XMLoadFloat3(pLook));

		_vector vPos = XMLoadFloat3(pPos);

		//vPos += vRightNor * -0.6f;
		vPos += vUpNor * 16.f;
		vPos += vLookNor * 5.f;

		XMStoreFloat3((_float3*)m_WorldMatrix.m[3], XMVectorSetW(vPos, 1.f));




		m_pVIBufferCom->Spread(fTimeDelta);		
	}
}

void CParticle_Block::Late_Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CParticle_Block::Render()
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


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Block::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_BlockEffect"),  
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_Block* CParticle_Block::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Block* pInstance = new CParticle_Block(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Block"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_Block::Clone(void* pArg)
{
	CParticle_Block* pInstance = new CParticle_Block(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Block"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Block::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
