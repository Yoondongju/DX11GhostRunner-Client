#include "stdafx.h"
#include "..\Public\Weapon_Player.h"

#include "Player.h"
#include "Body_Player.h"
#include "Animation.h"

#include "GameInstance.h"

#include "SwordTrail.h"
#include "ShurikenTrail.h"

CWeapon_Player::CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon_Player::CWeapon_Player(const CWeapon_Player& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CWeapon_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Player::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;


	m_pSubShuriken[0] = pDesc->pSubShuriSken[0];
	m_pSubShuriken[1] = pDesc->pSubShuriSken[1];




	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Reday_Trail()))
		return E_FAIL;


	XMStoreFloat4x4(&m_RotationMatrix, XMMatrixIdentity());


	m_KatanaOriginMatrix = {
		0.181342006, 0.655698836, 0.207122847, 0.00000000,
		-0.963511229, 0.238839343, 0.0874769986, 0.00000000,
		0.0110353436, -0.301334143, 0.944287121, 0.00000000,
		0.0750860050, -0.0355550535, 0.0480655581, 1.00000000
	};

	m_ShurikenOriginMatrix = {
		0.181342006, 0.655698836, 0.207122847, 0.00000000,
		-0.963511229, 0.238839343, 0.0874769986, 0.00000000,
		0.0110353436, -0.301334143, 0.944287121, 0.00000000,
		0.0750860050, -0.0355550535, 0.0480655581, 1.00000000
	};



	XMStoreFloat4x4(&m_KatanaOriginMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_KatanaOriginMatrix), XMMatrixRotationZ(XMConvertToRadians(90.0f))));

	m_pTransformCom->Set_WorldMatrix(m_KatanaOriginMatrix);

	return S_OK;
}

void CWeapon_Player::Priority_Update(_float fTimeDelta)
{

}

void CWeapon_Player::Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	if (false == m_isChangeWeapon)
	{
		m_ePreType = m_eCurType;
	}
	else
		m_isChangeWeapon = false;


	if (m_eCurType == SHURIKEN)
	{
		if (m_fDissolveAmount < 1.5f)					// Dissolve
			m_fDissolveAmount += fTimeDelta * 0.3f;


		if (m_isEmissiveIncreasing) 
		{
			if (m_fEmissiveEmphasize < 1.f)  // Emissive
			{
				m_fEmissiveEmphasize += fTimeDelta; // 0에서 1로 증가
			}
			else
			{
				m_isEmissiveIncreasing = false; // 최대치에 도달하면 감소 상태로 변경
			}
		}
		else		
		{
			if (m_fEmissiveEmphasize > 0.f) // 1에서 0으로 감소
			{
				m_fEmissiveEmphasize -= fTimeDelta;
			}
			else
			{
				m_isEmissiveIncreasing = true; // 최소치에 도달하면 증가 상태로 변경
			}
		}
			
		if (true == m_isHomingShurikenActive)
		{
			if(false == m_isHomingStartHunt)
				XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
			else
				LockTransformHoming(fTimeDelta);

			return;
		}

		


		m_pTransformCom->Set_WorldMatrix(m_ShurikenOriginMatrix);

		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

		if (false == m_isAttacking)
		{
			m_fShurikenAccTime = 0.f;

			if (true == m_isPreAttaciong)
				*(_float3*)m_WorldMatrix.m[3] = m_InitPos;
		

			XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
		
			XMStoreFloat3(&m_InitGoDir, XMVector3Normalize(pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK)));	// 초기 방향
		
			m_InitPos = *(_float3*)m_WorldMatrix.m[3];		// 표창의 초기위치고 
		
			XMStoreFloat3(&m_InitPlayerPos, pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION));	// 플레이어의 초기위치
		}
		else
		{
			_vector vInitDir = XMLoadFloat3(&m_InitGoDir);			
			_vector vCurPos = XMLoadFloat3((_float3*)m_WorldMatrix.m[3]);
		
		
			_float fSpeed = m_pTransformCom->Get_SpeedPerSec();						// 아마 30 
			_float fPlayerSpeed = pPlayer->Get_Transform()->Get_SpeedPerSec();		// 아마 80
		
			_vector vPlayerDir = pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK); // 플레이어가 바라보는 방향 (정면 방향)
			vPlayerDir = XMVector3Normalize(vPlayerDir);
		
			_vector vRightDir = pPlayer->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
			vRightDir = XMVector3Normalize(vRightDir);
		
		
			_vector vPlayerVelocity = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			deque<CVIBuffer_Trail::TRAIL_INFO>& Trail = m_pShurikenTrail->Get_Trail();
		
			if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
			{
				vPlayerVelocity += vPlayerDir * fPlayerSpeed; // 앞쪽 방향
			}
			if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
			{
				vPlayerVelocity -= vPlayerDir * fPlayerSpeed;
			}
			if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
			{
				vPlayerVelocity -= vRightDir * fPlayerSpeed;
			}
			if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
			{
				vPlayerVelocity += vRightDir * fPlayerSpeed; // 뒤쪽 방향
			}
			vPlayerVelocity = XMVectorSetY(vPlayerVelocity, 0.f);
		
			
			
			CModel* pModel = static_cast<CContainerObject*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY)->Get_Model();
			_double Duration = pModel->Get_CurAnimation()->Get_Duration();
			const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();


			_float MinSpeed = 2.f;   // 최소 속도
			_float MaxSpeed = 30.f;  // 최대 속도

			_float Ratio = (_float)(TrackPos / Duration);
			m_fShurikenAccTime = MinSpeed + (MaxSpeed - MinSpeed) * (Ratio * Ratio);


			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fShurikenAccTime* fTimeDelta);
	
			_vector vFinalVelocity = vInitDir * fSpeed * m_fShurikenAccTime + vPlayerVelocity;
			vCurPos += vFinalVelocity * fTimeDelta;

			XMStoreFloat3((_float3*)m_WorldMatrix.m[3], vCurPos);
		}
	}
	else if (m_eCurType == KATANA)
	{
		m_fDissolveAmount = 0.f;
		m_pTransformCom->Set_WorldMatrix(m_KatanaOriginMatrix);
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));		
	}
		
	
	m_pColliderCom[m_eCurType]->Update(&m_WorldMatrix);


	if (m_eCurType == SHURIKEN)
	{
		//m_pShurikenTrail->Update(fTimeDelta, &m_WorldMatrix);

		for (_uint i = 0; i < 2; i++)
		{
			_bool bLeft = false;;
			if (i % 2 == 0)
				bLeft = true;
		
			m_pSubShuriken[i]->Update(&m_WorldMatrix, m_isAttacking, m_InitGoDir, bLeft, m_InitPlayerPos, fTimeDelta);
		}
	}
	else if (m_eCurType == KATANA)
		m_pSwordTrail->Update(fTimeDelta);

}

void CWeapon_Player::Late_Update(_float fTimeDelta)
{
	if(m_eCurType == SHURIKEN)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
	else
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	if (true == m_isHomingShurikenActive)
	{
		m_pShurikenTrail->Late_Update(fTimeDelta);

#ifdef _DEBUG
		m_pGameInstance->Add_DebugObject(m_pColliderCom[m_eCurType]);
#endif
		return;
	}
		


	if (m_eCurType == SHURIKEN)
	{
		//m_pShurikenTrail->Late_Update(fTimeDelta);

		for (_uint i = 0; i < 2; i++)
		{
			m_pSubShuriken[i]->Late_Update(fTimeDelta);
		}
	}
	else if (m_eCurType == KATANA)
		m_pSwordTrail->Late_Update(fTimeDelta);


#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom[m_eCurType]);
#endif
}

HRESULT CWeapon_Player::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	

	_uint		iNumMeshes = m_pModelCom[m_eCurType]->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom[m_eCurType]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;
	
		_uint iPassNum = 0;

		switch (m_eCurType)
		{
		case Client::CWeapon_Player::KATANA:
			iPassNum = 0;
			break;
		case Client::CWeapon_Player::SHURIKEN:
		{
			if (FAILED(m_pModelCom[m_eCurType]->Bind_Material(m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveEmphasize", &m_fEmissiveEmphasize, sizeof(_float))))
				return E_FAIL;

			if (1.5f > m_fDissolveAmount)
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pCreateNoiseTexture->Bind_ShadeResource(m_pShaderCom, "g_NoiseTexture", 0)))
					return E_FAIL;
			}

			iPassNum = 2;
		}
			
			break;
		case Client::CWeapon_Player::WEAPON_TYPE_END:
			break;
		default:
			break;
		}

		if (FAILED(m_pModelCom[m_eCurType]->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_HEIGHT, i)))
		{
			// 뭐야
			int test;
		}

		if (FAILED(m_pShaderCom->Begin(iPassNum)))
			return E_FAIL;

		if (FAILED(m_pModelCom[m_eCurType]->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}



void CWeapon_Player::LockTransformHoming(_float fTimeDelta)
{	
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));


	_matrix CopyMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	CopyMatrix.r[0] = XMVector3Normalize(CopyMatrix.r[0]) * 0.2f;
	CopyMatrix.r[1] = XMVector3Normalize(CopyMatrix.r[1]) * 0.2f;
	CopyMatrix.r[2] = XMVector3Normalize(CopyMatrix.r[2]) * 0.2f;

	_float4x4 ColliderMatrix = {};
	XMStoreFloat4x4(&ColliderMatrix, CopyMatrix);
	m_pColliderCom[m_eCurType]->Update(&ColliderMatrix);					// 충돌체 크기 줄여

	m_pShurikenTrail->Update(fTimeDelta, &m_WorldMatrix);
}


HRESULT CWeapon_Player::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Katana"),
		TEXT("Com_Katana"), reinterpret_cast<CComponent**>(&m_pModelCom[WEAPON_TYPE::KATANA]))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Shuriken"),
		TEXT("Com_Shuriken"), reinterpret_cast<CComponent**>(&m_pModelCom[WEAPON_TYPE::SHURIKEN]))))
		return E_FAIL;


	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CreateDissolve"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pCreateNoiseTexture), nullptr)))
		return E_FAIL;



	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(2.f, 2.f, 11.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vExtents.z);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
		TEXT("Com_Collider_Katana"), reinterpret_cast<CComponent**>(&m_pColliderCom[WEAPON_TYPE::KATANA]), &ColliderDesc)))
		return E_FAIL;


	/* FOR.Com_Collider */
	ColliderDesc.vExtents = _float3(8.f, 8.f, 8.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
		TEXT("Com_Collider_Shuriken"), reinterpret_cast<CComponent**>(&m_pColliderCom[WEAPON_TYPE::SHURIKEN]), &ColliderDesc)))
		return E_FAIL;


	
	return S_OK;
}

HRESULT CWeapon_Player::Reday_Trail()
{
	CBody_Player* pBodyPlayer = static_cast<CBody_Player*>(static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY));


	CSwordTrail::EFFECT_DESC		SwordTrailDesc{};
	SwordTrailDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	SwordTrailDesc.pSocketBoneMatrix = pBodyPlayer->Get_BoneMatrix_Ptr("Weapon_r");
	SwordTrailDesc.pOwner = m_pOwner;			//   m_pOwner  == Player

	m_pSwordTrail = static_cast<CSwordTrail*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_SwordTrail"), &SwordTrailDesc));


	CShurikenTrail::EFFECT_DESC  ShurikenTrailDesc = {};
	ShurikenTrailDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	ShurikenTrailDesc.pSocketBoneMatrix = pBodyPlayer->Get_BoneMatrix_Ptr("Weapon_r");
	ShurikenTrailDesc.pOwner = m_pOwner;
	ShurikenTrailDesc.eType = CShurikenTrail::SHURIKEN_TYPE::MAIN;

	m_pShurikenTrail = static_cast<CShurikenTrail*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ShurikenTrail"), &ShurikenTrailDesc));


	return S_OK;
}



CWeapon_Player* CWeapon_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Player* pInstance = new CWeapon_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Player::Clone(void* pArg)
{
	CWeapon_Player* pInstance = new CWeapon_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Player"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Player::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	for (_uint i = 0; i < WEAPON_TYPE::WEAPON_TYPE_END; i++)
	{
		Safe_Release(m_pModelCom[i]);
	}

	for (_uint i = 0; i < WEAPON_TYPE::WEAPON_TYPE_END; i++)
	{
		Safe_Release(m_pColliderCom[i]);
	}

	for (_uint i = 0; i < 2; i++)
	{
		Safe_Release(m_pSubShuriken[i]);
	}

	Safe_Release(m_pCreateNoiseTexture);

	Safe_Release(m_pSwordTrail);
	Safe_Release(m_pShurikenTrail);
}

HRESULT Reday_Trail()
{
	return E_NOTIMPL;
}
