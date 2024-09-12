#include "stdafx.h"
#include "..\Public\FreeCamera.h"

#include "GameInstance.h"

#include "Player.h"

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
	
}

void CFreeCamera::Late_Update(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	
	if (nullptr == pPlayer)
		return;

	_vector vCamPosition = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	

	_vector RotationOffset = XMVector3TransformCoord(XMLoadFloat4(&m_OffsetByPlayer), XMLoadFloat4x4(&pPlayer->Get_RotationMatrix()));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition + RotationOffset);


	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, pPlayer->Get_Transform()->Get_State(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK));



	__super::Late_Update(fTimeDelta);
}

HRESULT CFreeCamera::Render()
{
	return S_OK;
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
