#include "stdafx.h"
#include "..\Public\HelMotionTrail.h"

#include "GameInstance.h"
#include "Hel.h"
#include "Weapon_Hel.h"

CHelMotionTrail::CHelMotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CHelMotionTrail::CHelMotionTrail(const CHelMotionTrail& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CHelMotionTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHelMotionTrail::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;



	

	return S_OK;
}

void CHelMotionTrail::Priority_Update(_float fTimeDelta)
{

}



void CHelMotionTrail::Update(_float fTimeDelta)
{


}

void CHelMotionTrail::Late_Update(_float fTimeDelta)
{
	//if (false == m_bActive)
	//	return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CHelMotionTrail::Render()
{
	for (_uint i = 0; i < m_MotionTrailInfo.size(); i++)
	{
		if (m_MotionTrailInfo[i].fLifeTime > 0.f)
			m_MotionTrailInfo[i].fLifeTime -= 0.016f;
		else
		{
			m_MotionTrailInfo.pop_front();
			continue;
		}	

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_MotionTrailInfo[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fMotionTrailTime", &m_MotionTrailInfo[i].fLifeTime , sizeof(_float))))
			return E_FAIL;

		

		_uint iNumMeshes = m_pModel->Get_NumMeshes();

		for (size_t j = 0; j < iNumMeshes; j++)
		{
			_uint iNumAffectBone = m_pModel->Get_NumAffectBone(j);

			if (FAILED(m_pShaderCom->Bind_Matrices("g_BoneMatrices", m_MotionTrailInfo[i].BoneMatrices[j], iNumAffectBone)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;

			if (FAILED(m_pModel->Render(j)))
				return E_FAIL;
		}	
	}
	return S_OK;
}

HRESULT CHelMotionTrail::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */				// Prototype_Component_Texture_ParticleShuriken
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SwordTrail"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STAGE2_BOSS, TEXT("Prototype_Component_Model_Hel"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel))))
		return E_FAIL;

	return S_OK;
}

CHelMotionTrail* CHelMotionTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHelMotionTrail* pInstance = new CHelMotionTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHelMotionTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CHelMotionTrail::Clone(void* pArg)
{
	CHelMotionTrail* pInstance = new CHelMotionTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CHelMotionTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHelMotionTrail::Free()
{
	__super::Free();

	m_MotionTrailInfo.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModel);
}
