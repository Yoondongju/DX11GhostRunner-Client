#include "stdafx.h"
#include "Elite_CutScene.h"

#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"

#include "FreeCamera.h"

CElite_CutScene::CElite_CutScene(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::CUTSCENE , pOwner }
{

}


HRESULT CElite_CutScene::Initialize()
{
	m_CutSceneExitPointPage1 = { 33.1385498, 17.1262779, -720.377197, };

	m_CutSceneExitPointPage2 = { 33.1385498, 17.1262779, -4000.377197, };

	return S_OK;
}


HRESULT CElite_CutScene::Start_State(void* pArg)
{
	CElite* pElite = static_cast<CElite*>(m_pOwner);

	if (true == pElite->IsPage2())
	{
		_float4x4   InitWorldMatrix =
		{
			0.999939024, -2.95103746e-08, -0.0107781896, 0.00000000,
			-0.000992476242, 0.995753646, -0.0920315087, 0.00000000,
			0.0107342554, 0.0920358375, 0.995693147, 0.00000000,
			33.1385498, 17.1262779, -720.377197, 1.00000000
		};

		pElite->Get_Transform()->Set_WorldMatrix(InitWorldMatrix);
		pElite->Get_Transform()->Scaling(3.5f, 3.5f, 3.5f);
	}


	CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
	CTransform* pCamTransform = pCamera->Get_Transform();

	pCamTransform->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	pCamTransform->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	pCamTransform->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
		
	return S_OK;
}


void CElite_CutScene::Update(_float fTimeDelta)
{
	CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
	CElite* pElite = static_cast<CElite*>(m_pOwner);
	pCamera->Set_FollowPlayer(false);
	pCamera->FollowElite(pElite);

	_vector vCurPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_float4& OffsetEliteRef = pCamera->Get_OffsetByEliteRef();

	if (false == pElite->IsPage2())
	{		
		_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_CutSceneExitPointPage1) - vCurPos));
		if (fDistance >= 30.f)
		{
			if (OffsetEliteRef.y <= 80.f)
				OffsetEliteRef.y += fTimeDelta * 7.f;

			if (OffsetEliteRef.z >= -90.f)
				OffsetEliteRef.z -= fTimeDelta * 7.3f;

			m_pOwner->Get_Transform()->Go_Straight(fTimeDelta * 10.f * m_fSpeed);
		}
		else
		{
			CModel* pModel = m_pOwner->Get_Model();
			CFsm* pFsm = m_pOwner->Get_Fsm();

			_bool	bDashBlock = true;
			pModel->SetUp_Animation(CElite::ELITE_ANIMATION::DASH_TO_ALERDLB, true);
			pFsm->Change_State(CElite::ELITE_ANIMATION::BLOCK_F, -1, &bDashBlock);
		}

		if (fDistance <= 1700.f)
		{
			_float	fRotationSpeed = 180.f;
			_float	fCurRotationSpeed = fRotationSpeed * fTimeDelta * 0.2f;

			CTransform* pCamTransform = pCamera->Get_Transform();

			if (m_fAccRotationSpeed + fCurRotationSpeed <= fRotationSpeed)
			{
				_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fCurRotationSpeed));
				pCamTransform->Turn(RotationMatrix, nullptr);

				m_fAccRotationSpeed += fCurRotationSpeed;



				if (OffsetEliteRef.z >= -110.f)
					OffsetEliteRef.z -= fTimeDelta * 4.5f;
			}
			else
			{
				_float fRemainingRotation = fRotationSpeed - m_fAccRotationSpeed;

				_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRemainingRotation));
				pCamTransform->Turn(RotationMatrix, nullptr);

				m_fAccRotationSpeed = fRotationSpeed;

				m_fSpeed = 2.f;
			}
		}
	}
	else         // Page 2ÀÏ¶§
	{
		_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_CutSceneExitPointPage2) - vCurPos));
		if (fDistance >= 50.f)
		{
			OffsetEliteRef.y = 70.f;
			OffsetEliteRef.z = -80.f;

			m_pOwner->Get_Transform()->Go_Backward(fTimeDelta * 22.f * m_fSpeed);
		}
		else
		{
			CModel* pModel = m_pOwner->Get_Model();
			CFsm* pFsm = m_pOwner->Get_Fsm();

			_bool	bDashBlock = true;
			pModel->SetUp_Animation(CElite::ELITE_ANIMATION::IDLE_TO_ALERT, true);
			pFsm->Change_State(CElite::ELITE_ANIMATION::IDLE_TO_ALERT);
		}

		_float	fRotationSpeed = 90.f;
		_float	fCurRotationSpeed = fRotationSpeed * fTimeDelta * 0.2f;

		CTransform* pCamTransform = pCamera->Get_Transform();

		if (m_fAccRotationSpeed + fCurRotationSpeed <= fRotationSpeed)
		{
			_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fCurRotationSpeed));
			pCamTransform->Turn(RotationMatrix, nullptr);

			m_fAccRotationSpeed += fCurRotationSpeed;
		}
		else
		{
			_float fRemainingRotation = fRotationSpeed - m_fAccRotationSpeed;

			_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRemainingRotation));
			pCamTransform->Turn(RotationMatrix, nullptr);

			m_fAccRotationSpeed = fRotationSpeed;
		}
	}
	
}

void CElite_CutScene::End_State()
{
	m_fSpeed = 1.f;
	m_fAccRotationSpeed = 0.f;

	CFreeCamera* pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY));
	pCamera->Set_FollowPlayer(true);

	static_cast<CElite*>(m_pOwner)->Set_Groggy(false);
}


CElite_CutScene* CElite_CutScene::Create(class CGameObject* pOwner)
{
	CElite_CutScene* pInstance = new CElite_CutScene(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_CutScene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_CutScene::Free()
{
	__super::Free();
}
