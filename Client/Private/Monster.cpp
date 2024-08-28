#include "stdafx.h"
#include "Monster.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Player.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{

}

CMonster::CMonster(const CMonster& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CMonster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player());
    Safe_AddRef(m_pPlayer);

    return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
   

}

void CMonster::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

}

void CMonster::Late_Update(_float fTimeDelta)
{

}

HRESULT CMonster::Render()
{ 

    return S_OK;
}


void CMonster::Free()
{
    __super::Free();

    Safe_Release(m_pPlayer);
}
