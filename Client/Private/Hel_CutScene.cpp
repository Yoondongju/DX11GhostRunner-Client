#include "stdafx.h"
#include "Hel_CutScene.h"

#include "Hel.h"
#include "GameInstance.h"
#include "Animation.h"

CHel_CutScene::CHel_CutScene(class CGameObject* pOwner)
	: CState{ CHel::HEL_ANIMATION::CUTSCENE , pOwner }
{

}

HRESULT CHel_CutScene::Initialize()
{
	return S_OK;
}

HRESULT CHel_CutScene::Start_State(void* pArg)
{


	return S_OK;
}


void CHel_CutScene::Update(_float fTimeDelta)
{


}



void CHel_CutScene::End_State()
{

}


CHel_CutScene* CHel_CutScene::Create(class CGameObject* pOwner)
{
	CHel_CutScene* pInstance = new CHel_CutScene(pOwner);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CHel_CutScene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHel_CutScene::Free()
{
	__super::Free();
}
