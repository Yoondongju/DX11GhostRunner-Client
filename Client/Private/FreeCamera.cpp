#include "stdafx.h"
#include "..\Public\FreeCamera.h"

#include "GameInstance.h"

#include "Player.h"
#include "Elite.h"
#include "Hel.h"

CFreeCamera::CFreeCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera { pDevice, pContext }
{
}

CFreeCamera::CFreeCamera(const CFreeCamera & Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CFreeCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFreeCamera::Initialize(void * pArg)
{
	CAMERA_FREE_DESC*		pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

	m_fSensor = pDesc->fSensor;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


	XMStoreFloat4(&m_OffsetByPlayer, XMVectorSet(0.f, 16.12215824f, 2.2040216f, 1.f));

	XMStoreFloat4(&m_OffsetByElite, XMVectorSet(0.f, 1.f, -2.f, 1.f));

	XMStoreFloat4(&m_OffsetByHel, XMVectorSet(0.f, 80.f, -700.f, 1.f));

	
	XMStoreFloat4(&m_OffsetByShuriken, XMVectorSet(0.f, -1.f, -6.f, 1.f));


	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));

	return S_OK;
}

void CFreeCamera::Priority_Update(_float fTimeDelta)
{
	
}


void CFreeCamera::Update(_float fTimeDelta)
{
	if (m_bShaking)
	{
		if (m_iShakingCount >= 0)
		{
			// 짝수일 때 오른쪽으로 회전

			// 짝수일때 이전프레임때 줫던힘 + 이번프레임의 힘의 강도
			// 다시 홀수일때 이번프레임의 힘의 강도의 2배  
			// 이번프레임의 힘의강도는 ?>

			_float fShakeForce = { 0.f };

			if (m_iShakingCount % 2 == 0)
			{
				fShakeForce = m_fForce * ((_float)m_iShakingCount / m_iOriginShakingCount);

				m_fCurRotation = fShakeForce;
			}
			else
			{
				m_fCurRotation = -m_fPreRotation * 2;
			}

			Active_Shake(0.2f * (m_fCurRotation - m_fPreRotation), XMLoadFloat3(&m_ShakeAsix));
			m_fPreRotation = m_fCurRotation;

		}
		else
		{
			m_bShaking = false;

			m_iShakingCount = 0;
			m_iShakingCount = 0;

			m_fForce = { 0.f };
			m_ShakeAsix = {};

			m_fCurRotation = 0.f;
			m_fPreRotation = 0.f;
		}
	}
	
}

void CFreeCamera::Late_Update(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	if (nullptr == pPlayer)
		return;

	if(true == m_isActiveFollowPlayer)
		FollowPlayer(pPlayer);






	__super::Late_Update(fTimeDelta);
}

HRESULT CFreeCamera::Render()
{
	return S_OK;
}


void CFreeCamera::FollowPlayer(CPlayer* pPlayer)
{
	CTransform* pPlayerTransform = pPlayer->Get_Transform();


	_vector vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);



	_vector RotationOffset = XMVector3TransformCoord(XMLoadFloat4(&m_OffsetByPlayer), XMLoadFloat4x4(&pPlayer->Get_RotationMatrix()));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition + RotationOffset);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, pPlayerTransform->Get_State(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, pPlayerTransform->Get_State(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, pPlayerTransform->Get_State(CTransform::STATE_LOOK));
}


void CFreeCamera::FollowElite(CElite* pElite)
{
	CTransform* pEliteTransform = pElite->Get_Transform();

	_vector vElitePosition = pEliteTransform->Get_State(CTransform::STATE_POSITION);


	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	WorldMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_vector RotationOffset = XMVector3TransformCoord(XMLoadFloat4(&m_OffsetByElite), WorldMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vElitePosition + RotationOffset);

}


void CFreeCamera::FollowHel(CHel* pHel)
{
	CTransform* pHelTransform = pHel->Get_Transform();

	_vector vHelPosition = pHelTransform->Get_State(CTransform::STATE_POSITION);


	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	WorldMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_vector RotationOffset = XMVector3TransformCoord(XMLoadFloat4(&m_OffsetByHel), WorldMatrix);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vHelPosition + RotationOffset);
}


void CFreeCamera::Active_Shake(_float fForce, _fvector vAsix)
{
	m_pTransformCom->Turn(vAsix, fForce, nullptr);
	--m_iShakingCount;
}



CFreeCamera * CFreeCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFreeCamera*		pInstance = new CFreeCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFreeCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject * CFreeCamera::Clone(void * pArg)
{
	CFreeCamera*		pInstance = new CFreeCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFreeCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFreeCamera::Free()
{
	__super::Free();

}
