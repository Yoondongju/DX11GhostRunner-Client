#include "stdafx.h"
#include "Pistol_Idle.h"


#include "Pistol.h"
#include "GameInstance.h"
#include "Animation.h"


CPistol_Idle::CPistol_Idle(class CGameObject* pOwner)
	: CState{ CPistol::PISTOL_ANIMATION::IDLE_1 , pOwner }
{

}

HRESULT CPistol_Idle::Initialize()
{
	return S_OK;
}

HRESULT CPistol_Idle::Start_State()
{


	return S_OK;
}

void CPistol_Idle::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	m_fAccTime += fTimeDelta;
	CModel* pModel = m_pOwner->Get_Model();


	CPistol::PISTOL_ANIMATION eCurAnim = (CPistol::PISTOL_ANIMATION)pModel->Get_CurAnimationIndex();
	if (CPistol::PISTOL_ANIMATION::IDLE_1 != eCurAnim)
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
			case Client::CPistol::TURN_L:
				pSniperTransform->Turn(XMMatrixRotationAxis(vNormalUp, XMConvertToRadians(-90.f)));
				break;
			case Client::CPistol::TURN_R:
				pSniperTransform->Turn(XMMatrixRotationAxis(vNormalUp, XMConvertToRadians(90.f)));
				break;
			case Client::CPistol::IDLE_2:
				break;
			default:
				break;
			}
			pModel->SetUp_Animation(CPistol::PISTOL_ANIMATION::IDLE_1, true, 0.f);
		}
	}


	if (m_fAccTime >= 5.f)
	{
		pModel->SetUp_Animation(CPistol::PISTOL_ANIMATION::TURN_L, true);

		_float fRandom = m_pGameInstance->Get_Random(0.f, 3.f);
		switch ((_uint)fRandom)
		{
		case 0:
			pModel->SetUp_Animation(CPistol::PISTOL_ANIMATION::TURN_L, true);
			break;
		case 1:
			pModel->SetUp_Animation(CPistol::PISTOL_ANIMATION::TURN_R, true);
			break;
		case 2:
			pModel->SetUp_Animation(CPistol::PISTOL_ANIMATION::IDLE_2, true);
			break;
		default:
			break;
		}

		m_fAccTime = 0.f;
	}

	if (Check_Detect())
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CPistol::PISTOL_ANIMATION::WALK_F, true);
		pFsm->Change_State(CPistol::PISTOL_ANIMATION::WALK_F);
		return;
	}

}

void CPistol_Idle::End_State()
{

}

_bool CPistol_Idle::Check_Detect()
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

_bool CPistol_Idle::Check_Death()
{
	return _bool();
}


CPistol_Idle* CPistol_Idle::Create(class CGameObject* pOwner)
{
	CPistol_Idle* pInstance = new CPistol_Idle(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPistol_Idle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPistol_Idle::Free()
{
	__super::Free();
}
