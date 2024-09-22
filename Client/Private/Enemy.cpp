#include "stdafx.h"
#include "Enemy.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Player.h"

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


void CEnemy::Free()
{
    __super::Free();

    Safe_Release(m_pTargetEnemy);
}
