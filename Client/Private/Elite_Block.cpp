#include "stdafx.h"
#include "Elite_Block.h"


#include "Elite.h"
#include "GameInstance.h"
#include "Animation.h"


CElite_Block::CElite_Block(class CGameObject* pOwner)
	: CState{ CElite::ELITE_ANIMATION::BLOCK_F , pOwner }
{

}

HRESULT CElite_Block::Initialize()
{
	return S_OK;
}

HRESULT CElite_Block::Start_State(void* pArg)
{
	if (nullptr != pArg)
		m_bDashBlock = *(_bool*)(pArg);
	

	return S_OK;
}

void CElite_Block::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	CModel* pModel = static_cast<CElite*>(m_pOwner)->Get_Model();

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();


	if (true == m_bDashBlock)	// 앞으로 뛰쳐나오는 막기였다면
	{
		_vector vPlayerPos = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		CTransform* pEliteTransform = m_pOwner->Get_Transform();
		_vector vElitePos = pEliteTransform->Get_State(CTransform::STATE_POSITION);

		pEliteTransform->LookAt_XZ(vPlayerPos);

		_float fDistance = XMVectorGetX(XMVector3Length(XMVectorSubtract(vPlayerPos, vElitePos)));
		if (fDistance > 40.f)
		{
			m_fAccTime += fTimeDelta;
			pEliteTransform->Go_Straight(m_fAccTime);
		}
		else
			m_bDashBlock = false;		
	}

	if (0.9 <= TrackPos / (_float)Duration)
	{
		CFsm* pFsm = m_pOwner->Get_Fsm();

		pModel->SetUp_Animation(CElite::ELITE_ANIMATION::IDLE_TO_ALERT, true);
		pFsm->Change_State(CElite::ELITE_ANIMATION::IDLE_TO_ALERT);

	}

}

void CElite_Block::End_State()
{
	m_bDashBlock = false;
	m_fAccTime = 0.f;
}


_bool CElite_Block::Check_Death()
{
	return _bool();
}




CElite_Block* CElite_Block::Create(class CGameObject* pOwner)
{
	CElite_Block* pInstance = new CElite_Block(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CElite_Block"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElite_Block::Free()
{
	__super::Free();
}
