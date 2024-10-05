#include "stdafx.h"
#include "Monster_Bullet.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Player.h"
#include "Body_Player.h"
#include "Enemy.h"

CMonster_Bullet::CMonster_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{

}

CMonster_Bullet::CMonster_Bullet(const CMonster_Bullet& Prototype)
    : CPartObject(Prototype)
{

}

HRESULT CMonster_Bullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Bullet::Initialize(void* pArg)
{
    BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);

    m_pSocketMatrix = pDesc->pSocketBoneMatrix;

    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

 
    

    return S_OK;
}

void CMonster_Bullet::Priority_Update(_float fTimeDelta)
{


}

void CMonster_Bullet::Update(_float fTimeDelta)
{
    if (true == static_cast<CEnemy*>(m_pOwner)->IsDead())
        return;

    _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
    for (size_t i = 0; i < 3; i++)
    {
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
    }

    if (false == m_isAttacking)
    {
        *(_float3*)m_pTransformCom->Get_WorldMatrix_Ptr()->m[3] = m_PreAttackPos;

        XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

        m_PreAttackPos = *(_float3*)m_pTransformCom->Get_WorldMatrix_Ptr()->m[3];

        m_fChangeColorTime = 0.f;       // 색 변화시간
    }
    else
    {  
        _float fSpeed = 1.f;

        _bool bMindControling = static_cast<CEnemy*>(m_pOwner)->IsMindControling();

        if (false == bMindControling)
        {
            Check_Collision_Player();
        }
        else if (true == bMindControling)
        {
            fSpeed = 5.f;
            Check_Collision_Enemy();      // 마인드컨트롤 상태라면 다른 충돌 비교를 해야하고
        }

        if (true == m_isBouncedBullet)
        {
            fSpeed = -5.f;
            Check_Collision_Me();
        }         
           

        m_pTransformCom->Go_Straight(fTimeDelta * fSpeed * 5.f);        // 몬스터마다 총알속도 달라

        _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        vPos.m128_f32[1] = XMVectorGetY(SocketMatrix.r[3]);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

        XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

        m_fChangeColorTime += fTimeDelta * 1.5f;
    }


    m_pColliderCom->Update(&m_WorldMatrix);
}

void CMonster_Bullet::Late_Update(_float fTimeDelta)
{
    if (true == static_cast<CEnemy*>(m_pOwner)->IsDead())
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CMonster_Bullet::Render()
{
    if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

  
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fChangeColorTime", &m_fChangeColorTime , sizeof(_float))))
        return E_FAIL;

    

    _uint iPassNum = 4;
  
    _uint		iNumMeshes = m_pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {

        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(iPassNum)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }



    return S_OK;
}



void CMonster_Bullet::Check_Collision_Player()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
    CBody_Player* pBodyPlayer = static_cast<CBody_Player*>(pPlayer->Get_Part(CPlayer::PART_BODY));

    CCollider* pCollider = pBodyPlayer->Get_Collider_OBB();

    if (m_pColliderCom->Intersect(pCollider))
    {
        //충돌햇어 근데 지금 상태가 막기상태야 이 처리를 
        CFsm* pFsm = pPlayer->Get_Fsm();
        CPlayer::PLAYER_ANIMATIONID eCurState = (CPlayer::PLAYER_ANIMATIONID)pFsm->Get_CurStateIndex();


        if (CPlayer::PLAYER_ANIMATIONID::BLOCK_R1 == eCurState ||
            CPlayer::PLAYER_ANIMATIONID::BLOCK_R2 == eCurState ||
            CPlayer::PLAYER_ANIMATIONID::BLOCK_R3 == eCurState)
        {
            pPlayer->Get_Part(CPlayer::PARTID::PART_PARTICLE_BLOCK)->SetActiveMyParticle(true);
            m_isBouncedBullet = true;           
        }
    }

}


void CMonster_Bullet::Check_Collision_Enemy()
{
    CEnemy* pTartgetEnemy = static_cast<CEnemy*>(m_pOwner)->Get_TargetEnemy();
    CCollider* pCollider = pTartgetEnemy->Get_Collider();

    m_pColliderCom->Intersect(pCollider);
    
   
    if (m_pColliderCom->IsBoundingCollisionEnter())
    {
        pTartgetEnemy->Check_CollByTargetEnemy();  
        static_cast<CEnemy*>(m_pOwner)->Set_MindControling(false, nullptr);
    }

}

void CMonster_Bullet::Check_Collision_Me()
{
    CEnemy* pOwner =  static_cast<CEnemy*>(m_pOwner);

    CCollider* pMyOwnerCollider = pOwner->Get_Collider();

    m_pColliderCom->Intersect(pMyOwnerCollider);

    if (m_pColliderCom->IsBoundingCollisionEnter())
    {
        pOwner->Check_Collision_Me();
    }
}



HRESULT CMonster_Bullet::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    /* For.Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.f, 1.f, 1.f);
    ColliderDesc.vCenter = _float3(0.f, 0, 0.f);
   
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;



    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Bullet"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    return S_OK;
}

CMonster_Bullet* CMonster_Bullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster_Bullet* pInstance = new CMonster_Bullet(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMonster_Bullet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster_Bullet::Clone(void* pArg)
{
    CMonster_Bullet* pInstance = new CMonster_Bullet(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMonster_Bullet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonster_Bullet::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
    Safe_Release(m_pColliderCom);
}
