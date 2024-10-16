#include "stdafx.h"
#include "..\Public\Particle_Explosion.h"

#include "GameInstance.h"

CParticle_Explosion::CParticle_Explosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CParticle_Explosion::CParticle_Explosion(const CParticle_Explosion& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CParticle_Explosion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Explosion::Initialize(void* pArg)
{
	PARTICLE_EXPLOSION* pDesc = static_cast<CParticle_Explosion::PARTICLE_EXPLOSION*>(pArg);



	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CParticle_Explosion::Priority_Update(_float fTimeDelta)
{


}

void CParticle_Explosion::Update(_float fTimeDelta)
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


		if (false == m_pGameInstance->Check_IsPlaying(SOUND_ENEMYEFFECT))
		{
			//_uint iRandom = m_pGameInstance->Get_Random_Interger(0, 2);
			//switch (iRandom)
			//{
			//case 0:
			//	m_pGameInstance->Play_Sound(TEXT("SFX_frogger_slam_01.ogg"), SOUND_ENEMYEFFECT, 1.f);
			//	break;
			//case 1:
			//	m_pGameInstance->Play_Sound(TEXT("SFX_frogger_slam_02.ogg"), SOUND_ENEMYEFFECT, 1.f);
			//	break;
			//case 2:
			//	m_pGameInstance->Play_Sound(TEXT("SFX_frogger_slam_03.ogg"), SOUND_ENEMYEFFECT, 1.f);
			//	break;
			//default:
			//	break;
			//}
		}	
		

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_pSpwanPos));
		m_pVIBufferCom->Spread(fTimeDelta);
	}
}


void CParticle_Explosion::Late_Update(_float fTimeDelta)
{
	if (m_isActiveMyParticle)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}


HRESULT CParticle_Explosion::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Explosion::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleExplosion"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Explosion"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_Explosion* CParticle_Explosion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Explosion* pInstance = new CParticle_Explosion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CParticle_Explosion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CParticle_Explosion::Clone(void* pArg)
{
	CParticle_Explosion* pInstance = new CParticle_Explosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CParticle_Explosion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Explosion::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
