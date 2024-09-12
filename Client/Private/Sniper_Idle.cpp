#include "stdafx.h"
#include "Sniper_Idle.h"


#include "Sniper.h"
#include "GameInstance.h"
#include "Animation.h"


CSniper_Idle::CSniper_Idle(class CGameObject* pOwner)
	: CState{ CSniper::SNIPER_ANIMATION::IDLE , pOwner }
{

}

HRESULT CSniper_Idle::Initialize()
{
	return S_OK;
}

HRESULT CSniper_Idle::Start_State()
{


	return S_OK;
}

void CSniper_Idle::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	m_fAccTime += fTimeDelta;
	CModel* pModel = m_pOwner->Get_Model();


	CSniper::SNIPER_ANIMATION eCurAnim = (CSniper::SNIPER_ANIMATION)pModel->Get_CurAnimationIndex();
	if (CSniper::SNIPER_ANIMATION::IDLE != eCurAnim)
	{
		_double Duration = pModel->Get_CurAnimation()->Get_Duration();
		const _double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
	
		if (0.9 <= (TrackPos / Duration))
		{
			CFsm* pFsm = m_pOwner->Get_Fsm();
	
			_double& TrackPos = pModel->Get_Referene_CurrentTrackPosition();
			TrackPos = 0.0;
	
			CTransform* pSniperTransform = m_pOwner->Get_Transform();
			_vector vNormalUp = XMVector3Normalize(pSniperTransform->Get_State(CTransform::STATE_UP));
	
			switch (eCurAnim)
			{
			case Client::CSniper::TURN_180:
				pSniperTransform->Turn(XMMatrixRotationAxis(vNormalUp, XMConvertToRadians(180.f)));
				break;
			case Client::CSniper::TURN_90_L:
				pSniperTransform->Turn(XMMatrixRotationAxis(vNormalUp, XMConvertToRadians(-90.f)));
				break;
			case Client::CSniper::TURN_90_R:
				pSniperTransform->Turn(XMMatrixRotationAxis(vNormalUp, XMConvertToRadians(90.f)));
				break;
			default:
				break;
			}
			pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::IDLE, true, 0.f);
		}
	}
	
	
	if (m_fAccTime >= 5.f)
	{
		pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::TURN_90_R, true);
		
		_float fRandom = m_pGameInstance->Get_Random(0.f, 3.f);
		switch ((_uint)fRandom)
		{
		case 0:
			pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::TURN_90_L, true);
			break;
		case 1:
			pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::TURN_90_R, true);
			break;
		case 2:
			pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::TURN_180, true);
			break;
		default:
			break;
		}
	
		m_fAccTime = 0.f;
	}

	if (Check_Detect())
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CSniper::SNIPER_ANIMATION::ATTACK, true);
		pFsm->Change_State(CSniper::SNIPER_ANIMATION::ATTACK);
		return;
	}

}

void CSniper_Idle::End_State()
{

}

_bool CSniper_Idle::Check_Detect()
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

_bool CSniper_Idle::Check_Death()
{
	return _bool();
}


CSniper_Idle* CSniper_Idle::Create(class CGameObject* pOwner)
{
	CSniper_Idle* pInstance = new CSniper_Idle(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CSniper_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSniper_Idle::Free()
{
	__super::Free();
}
