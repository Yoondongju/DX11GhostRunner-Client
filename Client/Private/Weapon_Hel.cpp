#include "stdafx.h"
#include "..\Public\Weapon_Hel.h"

#include "Player.h"

#include "GameInstance.h"

#include "Body_Player.h"

#include "HelSwordTrail.h"
#include "Hel.h"

CWeapon_Hel::CWeapon_Hel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{

}

CWeapon_Hel::CWeapon_Hel(const CWeapon_Hel& Prototype)
	: CPartObject{ Prototype }
{

}

HRESULT CWeapon_Hel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Hel::Initialize(void* pArg)
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

	
	m_pTransformCom->Scaling(1.6f, 1.6f, 1.6f);

	return S_OK;
}

void CWeapon_Hel::Priority_Update(_float fTimeDelta)
{

}

void CWeapon_Hel::Update(_float fTimeDelta)
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

void CWeapon_Hel::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	m_pSwordTrail->Late_Update(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CWeapon_Hel::Render()
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

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}




	return S_OK;
}

_bool CWeapon_Hel::Check_Collision()
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
				pPlayer->Get_Part(CPlayer::PARTID::PART_PARTICLE_BLOCK)->SetActiveMyParticle(true, true);

				CHel* pHel = static_cast<CHel*>(m_pOwner);
				pHel->Get_Part(CHel::PARTID::PART_PARTICLE_ATTACK)->SetActiveMyParticle(true);
				m_pGameInstance->Play_Sound(TEXT("HelPiece.ogg"), SOUND_HEL_PIECE, 1.f);

				_int iRandom = m_pGameInstance->Get_Random_Interger(0, 3);
				switch (iRandom)
				{
				case 0:
					m_pGameInstance->Play_Sound(TEXT("HelHitVO1.ogg"), SOUND_HEL_VOICE, 19.f);
					break;
				case 1:
					m_pGameInstance->Play_Sound(TEXT("HelHitVO2.ogg"), SOUND_HEL_VOICE, 19.f);
					break;
				case 2:
					m_pGameInstance->Play_Sound(TEXT("HelHitVO3.ogg"), SOUND_HEL_VOICE, 19.f);
					break;
				}

				

				_float fCurEnergy = pHel->Get_Energy();
				fCurEnergy -= 13.f;

				if (fCurEnergy <= 0.f)
				{
					fCurEnergy = 0.f;
					m_pGameInstance->Set_TimeDelayActive(true, 1.f);
				}
					

				pHel->Set_Energy(fCurEnergy);


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

HRESULT CWeapon_Hel::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* FOR.Com_Model */									// Prototype_Component_Model_Player_Weapon
	if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_Player_Weapon"),
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

HRESULT CWeapon_Hel::Reday_Trail()
{
	CHelSwordTrail::EFFECT_DESC		SwordTrailDesc{};
	SwordTrailDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	SwordTrailDesc.pSocketBoneMatrix = m_pOwner->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("weapon_bone");
	SwordTrailDesc.pOwner = m_pOwner;

	m_pSwordTrail = static_cast<CHelSwordTrail*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_HelSwordTrail"), &SwordTrailDesc));

	return S_OK;
}

CWeapon_Hel* CWeapon_Hel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Hel* pInstance = new CWeapon_Hel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Hel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Hel::Clone(void* pArg)
{
	CWeapon_Hel* pInstance = new CWeapon_Hel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Hel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Hel::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pModelCom);

	Safe_Release(m_pSwordTrail);
}
