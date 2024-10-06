#include "stdafx.h"
#include "Elite_Walk.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"

#include "EliteSwordTrail.h"
#include "Weapon_Elite.h"

CElite_Walk::CElite_Walk(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::WALK_F , pOwner }
{

}

HRESULT CElite_Walk::Initialize()
{
	return S_OK;
}

HRESULT CElite_Walk::Start_State(void* pArg)
{
	m_fRetreatTime = m_pGameInstance->Get_Random(1.f, 2.f);

	return S_OK;
}

void CElite_Walk::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	CElite* pElite = static_cast<CElite*>(m_pOwner);

	CModel* pModel = pElite->Get_Model();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	_double Duration = pModel->Get_CurAnimation()->Get_Duration();


	

	if (m_fRetreatTime > 0.f)
	{
		m_fRetreatTime -= fTimeDelta;

		CTransform* pEliteTransform = m_pOwner->Get_Transform();

		_float fSpeed = 1.f;
		if (true == pElite->IsPage2())
			fSpeed = 1.8f;

		switch (m_eDir)
		{
		case Engine::CState::FRONT:		// 없서
			break;
		case Engine::CState::BACK:
			pEliteTransform->Go_Backward(fTimeDelta * 5.f * fSpeed);
			break;
		case Engine::CState::LEFT:
			pEliteTransform->Go_Left(fTimeDelta * 5.f);
			break;
		case Engine::CState::RIGHT:
			pEliteTransform->Go_Right(fTimeDelta * 5.f);
			break;
		case Engine::CState::STATE_DIR_END:
			break;
		default:
			break;
		}

	}
	else
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();
		if (CState::BACK == m_eDir)  	// 뒤로 백했으면	
		{
			if (false == pElite->IsPage2())
			{
				_int iRandom = m_pGameInstance->Get_Random_Interger(0, 1);
				switch (iRandom)
				{
				case 0:
				{
					pModel->SetUp_Animation(CElite::ELITE_ANIMATION::WALK_L, true);
					pFsm->Change_State(CElite::ELITE_ANIMATION::WALK_F, CState::STATE_DIR::LEFT);
				}
				break;
				case 1:
				{
					pModel->SetUp_Animation(CElite::ELITE_ANIMATION::WALK_R, true);
					pFsm->Change_State(CElite::ELITE_ANIMATION::WALK_F, CState::STATE_DIR::RIGHT);
				}
				break;
				default:
					break;
				}
			}
			else
			{
				_bool	bDashBlock = true;

				pModel->SetUp_Animation(CElite::ELITE_ANIMATION::DASH_TO_ALERDLB, true);
				pFsm->Change_State(CElite::ELITE_ANIMATION::BLOCK_F, -1, &bDashBlock);
			}
	
		}
		else   
		{
			pModel->SetUp_Animation(CElite::ELITE_ANIMATION::IDLE_TO_ALERT, true);
			pFsm->Change_State(CElite::ELITE_ANIMATION::IDLE_TO_ALERT);
		}
	}
		
}

void CElite_Walk::End_State()
{
	
}


_bool CElite_Walk::Check_Death()
{
	return _bool();
}




CElite_Walk* CElite_Walk::Create(class CGameObject* pOwner)
{
	CElite_Walk* pInstance = new CElite_Walk(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_Walk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_Walk::Free()
{
	__super::Free();
}
