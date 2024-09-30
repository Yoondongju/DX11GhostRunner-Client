#include "stdafx.h"
#include "..\Public\ShurikenTrail.h"

#include "GameInstance.h"
#include "Player.h"

CShurikenTrail::CShurikenTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CShurikenTrail::CShurikenTrail(const CShurikenTrail& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CShurikenTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShurikenTrail::Initialize(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	


	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->eType)))
		return E_FAIL;





	return S_OK;
}

void CShurikenTrail::Priority_Update(_float fTimeDelta)
{

}



void CShurikenTrail::Update(_float fTimeDelta , const _float4x4* pWeaponMatrix)
{
	if (m_fDissolveAmount < 1.5f)
		m_fDissolveAmount += fTimeDelta * 2.f;

	_float3 StartLocal = {};
	_float3 EndLocal = {};
	_matrix WeaponMatrix = {};


	StartLocal = m_ShurikenStartLocal;
	EndLocal = m_ShurikenEndLocal;

	WeaponMatrix = XMLoadFloat4x4(pWeaponMatrix);

	if (false == m_bActive)
	{
		m_Trail.clear();
		m_fDissolveAmount = 0.f;

		m_StartPos = *(_float3*)pWeaponMatrix->m[3];
		return;
	}


	_float3	CurStart = {};
	_float3	CurEnd = {};
	XMStoreFloat3(&CurStart, XMVector3Transform(XMLoadFloat3(&StartLocal), WeaponMatrix));
	XMStoreFloat3(&CurEnd, XMVector3Transform(XMLoadFloat3(&EndLocal), WeaponMatrix));


	CVIBuffer_Trail::TRAIL_INFO TrailInfo = {};
	TrailInfo.CurStart = CurStart;
	TrailInfo.CurEnd = CurEnd;
	TrailInfo.fLifeTime = 0.5f;


	_vector vCurPos = WeaponMatrix.r[3];

	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&m_StartPos), vCurPos)));


	if (m_Trail.size() > 150)			// 2개당 사각형 하나지
	{
		m_Trail.pop_front();
	}


	m_Trail.emplace_back(TrailInfo);
	m_pVIBufferCom->Update_ShurikenTrail(fTimeDelta, m_Trail);
}


void CShurikenTrail::Late_Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}


HRESULT CShurikenTrail::Render()
{
	//_float4x4* pWeaponMatrix = static_cast<CWeapon_Player*>(static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_WEAPON))->Get_PartObjectComBindWorldMatrixPtr();
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", pWeaponMatrix)))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pDissolveTextureCom->Bind_ShadeResource(m_pShaderCom, "g_NoiseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShurikenTrail::Ready_Components(SHURIKEN_TYPE eType)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSwordTrail"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */									// Prototype_Component_Texture_SwordTrail
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleShuriken"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

												
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CreateDissolve"),
		TEXT("Com_Texture_Dissolve"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
		return E_FAIL;
	

	_wstring Prototype_TrailTag = L"";
	switch (eType)
	{
	case Client::CShurikenTrail::MAIN:
		Prototype_TrailTag = L"Prototype_Component_VIBuffer_MainShurikenTrail";
		break;
	case Client::CShurikenTrail::SUB0:
		Prototype_TrailTag = L"Prototype_Component_VIBuffer_SubShuriken0_Trail";
		break;
	case Client::CShurikenTrail::SUB1:
		Prototype_TrailTag = L"Prototype_Component_VIBuffer_SubShuriken1_Trail";
		break;
	case Client::CShurikenTrail::SHURIKEN_TYPE_END:
		break;
	default:
		break;
	}
	
	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Prototype_TrailTag,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CShurikenTrail* CShurikenTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShurikenTrail* pInstance = new CShurikenTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CShurikenTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CShurikenTrail::Clone(void* pArg)
{
	CShurikenTrail* pInstance = new CShurikenTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShurikenTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShurikenTrail::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pDissolveTextureCom);

	Safe_Release(m_pVIBufferCom);
}
