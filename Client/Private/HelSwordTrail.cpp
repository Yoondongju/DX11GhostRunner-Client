#include "stdafx.h"
#include "..\Public\HelSwordTrail.h"

#include "GameInstance.h"
#include "Hel.h"
#include "Weapon_Hel.h"

CHelSwordTrail::CHelSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CHelSwordTrail::CHelSwordTrail(const CHelSwordTrail& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CHelSwordTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHelSwordTrail::Initialize(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;


	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;





	return S_OK;
}

void CHelSwordTrail::Priority_Update(_float fTimeDelta)
{

}



void CHelSwordTrail::Update(_float fTimeDelta)
{
	CWeapon_Hel* pWeapon = static_cast<CWeapon_Hel*>(static_cast<CHel*>(m_pOwner)->Get_Part(CHel::PARTID::PART_WEAPON));
	_float4x4* pWeaponMatrix = pWeapon->Get_PartObjectComBindWorldMatrixPtr();

	_float3 StartLocal = {};
	_float3 EndLocal = {};
	_matrix WeaponMatrix = {};




	StartLocal = m_KatanaStartLocal;
	EndLocal = m_KatanaEndLocal;

	WeaponMatrix = XMLoadFloat4x4(pWeaponMatrix);

	if (false == m_bActive)
	{
		m_Trail.clear();

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
	TrailInfo.fLifeTime = 1.5f;


	// 원래는 80개 채우면 넘어가는식인데 만약 이동이 빠를경우에 이전에 위치가 너무 동떨어져 있어서
	// 가는듯한 늒미을 받지못해
	// 달리면서 때린다면  or 스킬을 사용하면서 때린다면 등등								   


	_vector vCurPos = WeaponMatrix.r[3];

	_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&m_StartPos), vCurPos)));

	//if (fDistance > 200.f && m_Trail.size() > 1)
	//{
	//	m_Trail.clear();
	//}

	if (m_Trail.size() > 150)			// 2개당 사각형 하나지
	{
		m_Trail.pop_front();
	}


	m_Trail.emplace_back(TrailInfo);
	m_pVIBufferCom->Update_SwordTrail(fTimeDelta, m_Trail);
}

void CHelSwordTrail::Late_Update(_float fTimeDelta)
{
	if (false == m_bActive)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CHelSwordTrail::Render()
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




	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHelSwordTrail::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSwordTrail"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */				// Prototype_Component_Texture_ParticleShuriken
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleShuriken"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_HelSwordTrail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CHelSwordTrail* CHelSwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHelSwordTrail* pInstance = new CHelSwordTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CHelSwordTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CHelSwordTrail::Clone(void* pArg)
{
	CHelSwordTrail* pInstance = new CHelSwordTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CHelSwordTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHelSwordTrail::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
