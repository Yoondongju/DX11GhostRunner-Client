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


	return S_OK;
}

void CElite_Block::Update(_float fTimeDelta)
{
	if (Check_Death())
		return;

	CModel* pModel = static_cast<CElite*>(m_pOwner)->Get_Model();

	_double Duration = pModel->Get_CurAnimation()->Get_Duration();
	_double TrackPos = pModel->Get_Referene_CurrentTrackPosition();


	






}

void CElite_Block::End_State()
{

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
