#include "stdafx.h"
#include "Enemy.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Player.h"
#include "EnemyMarkerUI.h"
#include "EnemyMarkerMC.h"

#include "EnemyFindUI.h"
#include "MiniMapMarker.h"
#include "MiniMapPanel.h"

CEnemy::CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
{

}

CEnemy::CEnemy(const CEnemy& Prototype)
    : CContainerObject(Prototype)
{

}

HRESULT CEnemy::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEnemy::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Create_EnemyMarker()))
        return E_FAIL;

    if (FAILED(Create_EnemyMarkerMC()))
        return E_FAIL;
    

    if (FAILED(Create_EnemyFind()))
        return E_FAIL;

    if (FAILED(Create_EnemyMiniMapMarker()))
        return E_FAIL;





    return S_OK;
}


void CEnemy::Priority_Update(_float fTimeDelta)
{
   
}

void CEnemy::Update(_float fTimeDelta)
{
   

}

void CEnemy::Late_Update(_float fTimeDelta)
{

}

HRESULT CEnemy::Render()
{ 

    return S_OK;
}


HRESULT CEnemy::Create_EnemyMarker()
{
    CEnemyMarker::ENEMYMARKER_DESC  Desc = {};
    Desc.pOwner = this;
    Desc.fX = 0;                    // 위치는 스킬쓸때 몬스터 위치 바로바로 갱신으로갈거임
    Desc.fY = 0;                    // 위치는 스킬쓸때 몬스터 위치 바로바로 갱신으로갈거임
    Desc.fSizeX = 70;
    Desc.fSizeY = 70;
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
   
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, L"Layer_EnemyMarkerUI", L"Prototype_GameObject_EnemyMarkerUI", L"No Model", &Desc)))
        return E_FAIL;

    return S_OK;
}


HRESULT CEnemy::Create_EnemyMarkerMC()
{
    CEnemyMarkerMC::ENEMYMARKERMC_DESC Desc = {};
    Desc.pOwner = this;
    Desc.fX = 0;                    // 위치는 스킬쓸때 몬스터 위치 바로바로 갱신으로갈거임
    Desc.fY = 0;                    // 위치는 스킬쓸때 몬스터 위치 바로바로 갱신으로갈거임
    Desc.fSizeX = 90;
    Desc.fSizeY = 90;
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, L"Layer_EnemyMarkerMC", L"Prototype_GameObject_EnemyMarkerMC", L"No Model", &Desc)))
        return E_FAIL;

    return S_OK;
}


HRESULT CEnemy::Create_EnemyFind()
{
    CEnemyFindUI::ENEMYMYFIND_DESC  Desc = {};
    Desc.pOwner = this;
    Desc.fX = 0;                    // 위치는 스킬쓸때 몬스터 위치 바로바로 갱신으로갈거임
    Desc.fY = 0;                    // 위치는 스킬쓸때 몬스터 위치 바로바로 갱신으로갈거임
    Desc.fSizeX = 40;
    Desc.fSizeY = 40;
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, L"Layer_EnemyFindUI", L"Prototype_GameObject_EnemyFind", L"No Model", &Desc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CEnemy::Create_EnemyMiniMapMarker()
{
    CMiniMapMarker::MINIMAPMARKER_DESC MiniMapMarkerDesc = {};

    MiniMapMarkerDesc.pOwner = this;
    MiniMapMarkerDesc.fX = 0;
    MiniMapMarkerDesc.fY = 0;
    MiniMapMarkerDesc.fSizeX = 20;
    MiniMapMarkerDesc.fSizeY = 20;
    MiniMapMarkerDesc.fSpeedPerSec = 10.f;
    MiniMapMarkerDesc.fRotationPerSec = XMConvertToRadians(90.0f);


    CMiniMapPanel* pMiniMapPanel = static_cast<CMiniMapPanel*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, L"Layer_MiniMapPanel", 0));
    
    CUIObject* pChildUI = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_MiniMapMarker", &MiniMapMarkerDesc));
    pChildUI->SetParentUI(pMiniMapPanel);

    pMiniMapPanel->GetChildUI().emplace_back(pChildUI);


    return S_OK;
}


void CEnemy::Free()
{
    __super::Free();


    Safe_Release(m_pTargetEnemy);

}
