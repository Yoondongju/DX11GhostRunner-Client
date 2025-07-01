#include "stdafx.h"
#include "..\Public\Weapon_Elite.h"

#include "Player.h"

#include "GameInstance.h"

#include "Body_Player.h"

#include "EliteSwordTrail.h"

#include "Elite.h"

CWeapon_Elite::CWeapon_Elite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{

}

CWeapon_Elite::CWeapon_Elite(const CWeapon_Elite& Prototype)
	: CPartObject{ Prototype }
{

}

HRESULT CWeapon_Elite::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Elite::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Reday_Trail()))
		return E_FAIL;
	

	_float4x4 InitWorldMatrix = {
		0.181342006, 0.655698836, 0.207122847, 0.00000000,
		-0.963511229, 0.238839343, 0.0874769986, 0.00000000,
		0.0110353436, -0.301334143, 0.944287121, 0.00000000,
		0.0750860050, -0.0355550535, 0.0480655581, 1.00000000
	};


	m_pTransformCom->Set_WorldMatrix(InitWorldMatrix);

	m_pTransformCom->Scaling(1.6f, 1.6f, 1.6f);

	return S_OK;
}

void CWeapon_Elite::Priority_Update(_float fTimeDelta)
{

}

void CWeapon_Elite::Update(_float fTimeDelta)
{	
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	if (m_fCollisionCoolTime > 0.f)
		m_fCollisionCoolTime -= fTimeDelta;


	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Update(&m_WorldMatrix);

	m_pSwordTrail->Update(fTimeDelta);
}

void CWeapon_Elite::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	m_pSwordTrail->Late_Update(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CWeapon_Elite::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_HEIGHT, i)))
		{
			int test;
		}

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}




	return S_OK;
}

_bool CWeapon_Elite::Check_Collision()
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	CBody_Player* pBodyPlayer = static_cast<CBody_Player*>(pPlayer->Get_Part(CPlayer::PART_BODY));

	CCollider* pCollider = pBodyPlayer->Get_Collider_OBB();

	if (m_pColliderCom->Intersect(pCollider))
	{
		//충돌햇어 근데 지금 상태가 막기상태야 이 처리를 
		CFsm* pFsm = pPlayer->Get_Fsm();
		CPlayer::PLAYER_ANIMATIONID eCurState = (CPlayer::PLAYER_ANIMATIONID)pFsm->Get_CurStateIndex();


		if (m_fCollisionCoolTime <= 0.f && m_pColliderCom->IsBoundingCollisionEnter())
		{
			if (CPlayer::PLAYER_ANIMATIONID::BLOCK_R1 == eCurState ||
				CPlayer::PLAYER_ANIMATIONID::BLOCK_R2 == eCurState ||
				CPlayer::PLAYER_ANIMATIONID::BLOCK_R3 == eCurState)
			{
				pPlayer->Get_Part(CPlayer::PARTID::PART_PARTICLE_BLOCK)->SetActiveMyParticle(true , true);
				
				CElite* pElite = static_cast<CElite*>(m_pOwner);

				_float fCurEnergy = pElite->Get_Energy();
				fCurEnergy -= 27.f;

				if (fCurEnergy <= 0.f)
				{
					fCurEnergy = 0.f;
					m_pGameInstance->Set_TimeDelayActive(true, 1.f);
				}
				
				pElite->Set_Energy(fCurEnergy);


				if (false == pPlayer->Get_Shaking())
				{
					CTransform* pPlayerTransform = pPlayer->Get_Transform();
					_vector vRightVector = pPlayerTransform->Get_State(CTransform::STATE_RIGHT);
					vRightVector = XMVector3Normalize(vRightVector);

					pPlayer->Set_Shake(4, 0.2f, vRightVector);
				}

				m_fCollisionCoolTime = 1.f;
				return true;
			}

		}	
	}

	return false;
}

HRESULT CWeapon_Elite::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_Player_Katana"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(2.f, 2.f, 11.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vExtents.z);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CWeapon_Elite::Reday_Trail()
{
	CEliteSwordTrail::EFFECT_DESC		SwordTrailDesc{};
	SwordTrailDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	SwordTrailDesc.pSocketBoneMatrix = m_pOwner->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("Gun_r");
	SwordTrailDesc.pOwner = m_pOwner;			

	m_pSwordTrail = static_cast<CEliteSwordTrail*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_EliteSwordTrail"), &SwordTrailDesc));

	return S_OK;
}

CWeapon_Elite* CWeapon_Elite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Elite* pInstance = new CWeapon_Elite(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Elite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Elite::Clone(void* pArg)
{
	CWeapon_Elite* pInstance = new CWeapon_Elite(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Elite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Elite::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pModelCom);

	Safe_Release(m_pSwordTrail);
}
