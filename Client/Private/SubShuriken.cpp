#include "stdafx.h"
#include "..\Public\SubShuriken.h"

#include "Player.h"
#include "Body_Player.h"
#include "ShurikenTrail.h"

#include "Animation.h"
#include "GameInstance.h"

CSubShuriken::CSubShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CSubShuriken::CSubShuriken(const CSubShuriken& Prototype)
	: CPartObject{ Prototype }
{
}

HRESULT CSubShuriken::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSubShuriken::Initialize(void* pArg)
{
	SUBSHURIKEN_DESC* pDesc = static_cast<SUBSHURIKEN_DESC*>(pArg);
	m_MyOffSet = pDesc->MyOffSet;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Trail(pDesc->iNumSubShuriken)))
		return E_FAIL;
	

	return S_OK;
}

void CSubShuriken::Priority_Update(_float fTimeDelta)
{

}

void CSubShuriken::Update(_float4x4* pParentWMatrix , _bool isAttacking,  _float3 GoDir, _bool isLeft , _float3 InitPlayerPos ,_float fTimeDelta)
{
	if (m_fDissolveAmount < 1.5f)
		m_fDissolveAmount += fTimeDelta * 0.5f;


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




	CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);

	if (false == isAttacking)
	{
		m_fShurikenAccTime = 0.f;

		m_pTransformCom->Set_WorldMatrix(*pParentWMatrix);
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


		_matrix ParentRotation = XMLoadFloat4x4(pParentWMatrix);		// 내월드행렬에서 위치성분을 제외시키면 회전정보를 알수있다
		ParentRotation.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);			// 내월드행렬에서 위치성분을 제외시키면 회전정보를 알수있다
		_vector vOffset = XMVector3TransformCoord(XMLoadFloat3(&m_MyOffSet), ParentRotation);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + vOffset);
	}
	else
	{
		_vector vUpNor = XMVector3Normalize(pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP));
		_vector vDirNor = XMLoadFloat3(&GoDir);
		
		if (true == isLeft)
		{
			_matrix RoatationL = XMMatrixRotationAxis(vUpNor, XMConvertToRadians(10.f));
			vDirNor = XMVector3TransformCoord(vDirNor, RoatationL);
		}
		else
		{
			_matrix RoatationR = XMMatrixRotationAxis(vUpNor, XMConvertToRadians(-10.f));
			vDirNor = XMVector3TransformCoord(vDirNor, RoatationR);
		}

		_vector vInitDir = vDirNor;
		
		const _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		_vector vCurPos = XMLoadFloat3((_float3*)pWorldMatrix->m[3]);


		_float fSpeed = m_pTransformCom->Get_SpeedPerSec();
		_float fPlayerSpeed = pPlayer->Get_Transform()->Get_SpeedPerSec();		// 아마 80

		_vector vPlayerDir = pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK); // 플레이어가 바라보는 방향 (정면 방향)
		//vPlayerDir = XMVectorSetY(vPlayerDir, XMVectorGetY(vInitDir));
		vPlayerDir = XMVector3Normalize(vPlayerDir);
		
		_vector vRightDir = pPlayer->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
		//vRightDir = XMVectorSetY(vRightDir, XMVectorGetY(vInitDir));
		vRightDir = XMVector3Normalize(vRightDir);


		_vector vPlayerVelocity = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		
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

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fShurikenAccTime * fTimeDelta);

		_vector vFinalVelocity = vInitDir * fSpeed * m_fShurikenAccTime + vPlayerVelocity;
		vCurPos += vFinalVelocity * fTimeDelta;

		XMStoreFloat3((_float3*)pWorldMatrix->m[3], vCurPos);
	}


	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());


	//m_pShurikenTrail->Update(fTimeDelta , m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CSubShuriken::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	//m_pShurikenTrail->Late_Update(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CSubShuriken::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, i)))
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
		

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CSubShuriken::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Shuriken"),
		TEXT("Com_Shuriken"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CreateDissolve"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pCreateNoiseTexture), nullptr)))
		return E_FAIL;

	
	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(8.f, 8.f, 8.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
		TEXT("Com_Collider_Shuriken"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSubShuriken::Ready_Trail(_uint iNumSubShuriken)
{
	CBody_Player* pBodyPlayer = static_cast<CBody_Player*>(static_cast<CPlayer*>(m_pOwner)->Get_Part(CPlayer::PARTID::PART_BODY));

	CShurikenTrail::EFFECT_DESC  ShurikenTrailDesc = {};
	ShurikenTrailDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	ShurikenTrailDesc.pSocketBoneMatrix = pBodyPlayer->Get_BoneMatrix_Ptr("Weapon_r");
	ShurikenTrailDesc.pOwner = m_pOwner;

	if(0 == iNumSubShuriken)
		ShurikenTrailDesc.eType = CShurikenTrail::SHURIKEN_TYPE::SUB0;
	else if(1 == iNumSubShuriken)
		ShurikenTrailDesc.eType = CShurikenTrail::SHURIKEN_TYPE::SUB1;


	m_pShurikenTrail = static_cast<CShurikenTrail*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ShurikenTrail"), &ShurikenTrailDesc));


	return S_OK;
}


CSubShuriken* CSubShuriken::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSubShuriken* pInstance = new CSubShuriken(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSubShuriken"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSubShuriken::Clone(void* pArg)
{
	CSubShuriken* pInstance = new CSubShuriken(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSubShuriken"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSubShuriken::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);

	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pCreateNoiseTexture);
	Safe_Release(m_pShurikenTrail);

}
