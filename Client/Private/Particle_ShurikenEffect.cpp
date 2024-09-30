#include "stdafx.h"
#include "..\Public\Particle_ShurikenEffect.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Weapon_Player.h"
#include "Player.h"

CParticle_ShurikenEffect::CParticle_ShurikenEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CParticle_ShurikenEffect::CParticle_ShurikenEffect(const CParticle_ShurikenEffect& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CParticle_ShurikenEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_ShurikenEffect::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_pShuriken = static_cast<PARTICLE_SHURIKENEFFECT*>(pArg)->pShuriken;
	Safe_AddRef(m_pShuriken);

	return S_OK;
}


void CParticle_ShurikenEffect::Priority_Update(_float fTimeDelta)
{

}


void CParticle_ShurikenEffect::Update(_float fTimeDelta)
{
	CPartObject* pShuriken = static_cast<CPartObject*>(m_pShuriken);
	CWeapon_Player* pMainShuriken = dynamic_cast<CWeapon_Player*>(pShuriken);

	if (nullptr != pMainShuriken)
	{
		m_pTransformCom->Set_WorldMatrix(*pShuriken->Get_PartObjectComBindWorldMatrixPtr());
	
		m_bActive = pMainShuriken->IsHomingActive();
	}
	else
		m_pTransformCom->Set_WorldMatrix(*pShuriken->Get_Transform()->Get_WorldMatrix_Ptr());

	if (false == m_bActive)
		return;

	
	m_pVIBufferCom->Spread(fTimeDelta);
}

void CParticle_ShurikenEffect::Late_Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_ShurikenEffect::Render()
{
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

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_ShurikenEffect::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Lightning"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_ShurikenEffect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_ShurikenEffect* CParticle_ShurikenEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_ShurikenEffect* pInstance = new CParticle_ShurikenEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_ShurikenEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_ShurikenEffect::Clone(void* pArg)
{
	CParticle_ShurikenEffect* pInstance = new CParticle_ShurikenEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_ShurikenEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_ShurikenEffect::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pShuriken);
}
