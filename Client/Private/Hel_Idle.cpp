#include "stdafx.h"
#include "Hel_Idle.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_Idle::CHel_Idle(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::IDLE , pOwner }
{

}

HRESULT CHel_Idle::Initialize()
{
	return S_OK;
}

HRESULT CHel_Idle::Start_State(void* pArg)
{	
	CHel* pHel = static_cast<CHel*>(m_pOwner);	
	if (true == pHel->IsPage2())
		m_fDetectRadius = 500.f;



	return S_OK;
}


void CHel_Idle::Update(_float fTimeDelta)
{
	CModel* pModel = m_pOwner->Get_Model();
	CFsm* pFsm = m_pOwner->Get_Fsm();


	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	CTransform* pHelTransform = m_pOwner->Get_Transform();
	_vector vHelPos = pHelTransform->Get_State(CTransform::STATE_POSITION);


	if (250 <= fabs(vPlayerPos.m128_f32[1] - vHelPos.m128_f32[1]))
	{
		pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE_TO_JUMP, true);
		pFsm->Change_State(CHel::HEL_ANIMATION::IDLE_TO_JUMP);

		return;
	}

	if (Check_Detect())
	{
		// 다음상태에 들어가기전에 먼저 플레이어 쳐다보고 ㄱ
		_vector vHelLookNor = XMVector3Normalize(pHelTransform->Get_State(CTransform::STATE_LOOK));

		_vector vHelToPlayerDir = XMVectorSubtract(vPlayerPos, vHelPos);
		vHelToPlayerDir = XMVectorSetY(vHelToPlayerDir, 0.0f);  // Y축 성분 제거
		_vector vHelToPlayerDirNor = XMVector3Normalize(vHelToPlayerDir);

		_float fDot = XMVectorGetX(XMVector3Dot(vHelLookNor, vHelToPlayerDirNor));

		_float fAngle = XMConvertToDegrees(acos(fDot));

		if (1.f < fAngle)
		{
			pHelTransform->LookAt_XZSmooth(vPlayerPos, fTimeDelta * 3.f, nullptr);
		}
		else
		{
			_float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vHelPos));
			if (fDistance > 150.f && fDistance < 300.f)
			{
				CHel* pHel = static_cast<CHel*>(m_pOwner);
				if (true == pHel->IsPage2())
				{
					_uint iCurAnimIndex = pModel->Get_CurAnimationIndex();
					if (iCurAnimIndex != CHel::HEL_ANIMATION::ATTACK2)
					{
						pModel->SetUp_Animation(CHel::HEL_ANIMATION::ATTACK2, true);
						pFsm->Change_State(CHel::HEL_ANIMATION::DASH_TO_IDLE_ATTACK);
					}			
				}


				return;
			}
			else if (fDistance < 150.f)
			{
				_uint iCurAnimIndex = pModel->Get_CurAnimationIndex();
				if (iCurAnimIndex != CHel::HEL_ANIMATION::ATTACK4 &&
					iCurAnimIndex != CHel::HEL_ANIMATION::ATTACK2 &&
					iCurAnimIndex != CHel::HEL_ANIMATION::ATTACK1)
				{
					_int iRandom = m_pGameInstance->Get_Random_Interger(0, 2);    // 랜덤으로   Sprint, Run_Jump 
					switch (iRandom)
					{
					case 0:
						pModel->SetUp_Animation(CHel::HEL_ANIMATION::ATTACK4, true);
						break;
					case 1:
						pModel->SetUp_Animation(CHel::HEL_ANIMATION::ATTACK2, true);
						break;
					case 2:
						pModel->SetUp_Animation(CHel::HEL_ANIMATION::ATTACK1, true);
						break;
					}

					pFsm->Change_State(CHel::HEL_ANIMATION::ATTACK1);
					return;
				}
				else
				{
					return;
				}
			}
			else
			{
				_int iRandom = m_pGameInstance->Get_Random_Interger(0, 1);    // 랜덤으로   Sprint, Run_Jump 
				switch (iRandom)
				{
				case 0:
				{
					pModel->SetUp_Animation(CHel::HEL_ANIMATION::SPRINT, true);
					pFsm->Change_State(CHel::HEL_ANIMATION::SPRINT);
				}
				break;
				case 1:
				{
					pModel->SetUp_Animation(CHel::HEL_ANIMATION::RUN_JUMP, true);
					pFsm->Change_State(CHel::HEL_ANIMATION::RUN_JUMP);
				}
				break;
				default:
					break;
				}
				return;
			}
			
		}	
	}
	else
	{
		m_fAccTime += fTimeDelta;
		if (m_fAccTime >= 1.5f)
		{
			pModel->SetUp_Animation(CHel::HEL_ANIMATION::IDLE_TO_JUMP, true);
			pFsm->Change_State(CHel::HEL_ANIMATION::IDLE_TO_JUMP);
		}

	}

}


void CHel_Idle::End_State()
{
	m_fAccTime = 0.f;
}

_bool CHel_Idle::Check_Detect()
{
	_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector vSniperPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	_float  fPlayerX = XMVectorGetX(vPlayerPos);
	_float  fPlayerZ = XMVectorGetZ(vPlayerPos);

	_float  fSniperX = XMVectorGetX(vSniperPos);
	_float  fSniperZ = XMVectorGetZ(vSniperPos);


	_float fDistanceSquar = (fPlayerX - fSniperX) * (fPlayerX - fSniperX) + (fPlayerZ - fSniperZ) * (fPlayerZ - fSniperZ);


	return (fDistanceSquar <= (m_fDetectRadius * m_fDetectRadius));
}


CHel_Idle* CHel_Idle::Create(class CGameObject* pOwner)
{
	CHel_Idle* pInstance = new CHel_Idle(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_Idle::Free()
{
	__super::Free();
}
