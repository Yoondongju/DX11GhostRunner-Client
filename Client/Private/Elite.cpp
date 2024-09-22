#include "stdafx.h"
#include "Elite.h"
#include "GameInstance.h"

#include "FreeCamera.h"
#include "Player.h"

#include "Weapon_Elite.h"
#include "Particle_Blood.h"

#include "Animation.h"

#include "Elite_Idle.h"
#include "Elite_Alert.h"
#include "Elite_Block.h"
#include "Elite_Turbo.h"
#include "Elite_Walk.h"
#include "Elite_Death.h"

CElite::CElite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEnemy(pDevice, pContext)
{

}


CElite::CElite(const CElite& Prototype)
    : CEnemy(Prototype)
{

}


HRESULT CElite::Initialize_Prototype()
{
    return S_OK;
}


HRESULT CElite::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_Parts()))
        return E_FAIL;

    if (FAILED(Ready_State()))
        return E_FAIL;
    
    if (FAILED(Ready_Change_Layer()))
        return E_FAIL;
    
    Ready_Modify_Animation();



    m_pModel->SetUp_Animation(CElite::ELITE_ANIMATION::IDLE, true);
    m_pFsm->Set_State(CElite::ELITE_ANIMATION::IDLE);

    m_pTransformCom->Scaling(2.5f, 2.5f, 2.5f);

    return S_OK;
}

void CElite::Priority_Update(_float fTimeDelta)
{

    for (auto& pPartObject : m_Parts)
        pPartObject->Priority_Update(fTimeDelta);
}

void CElite::Update(_float fTimeDelta)
{
    if (m_isDead)
    {
        if (m_fDiscard >= 1.f)
            m_pGameInstance->Delete(LEVEL_GAMEPLAY, CRenderer::RG_NONBLEND, this);

        m_fDiscard += fTimeDelta * 0.4f;
    }

    m_pFsm->Update(fTimeDelta);
    m_pModel->Play_Animation(fTimeDelta);


    for (auto& pPartObject : m_Parts)
        pPartObject->Update(fTimeDelta);


    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CElite::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif

    for (auto& pPartObject : m_Parts)
        pPartObject->Late_Update(fTimeDelta);

}

HRESULT CElite::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4((CPipeLine::D3DTS_VIEW)))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;



    _uint iPassNum = 0;
    if (true == m_isDead)
    {
        iPassNum = 1;


        _float dissovleAmount = 0.5f;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveAmount", &m_fDiscard, sizeof(_float))))
            return E_FAIL;

        if (FAILED(m_pDeadNoiseTexture->Bind_ShadeResource(m_pShaderCom, "g_NoiseTexture", 0)))
            return E_FAIL;
    }



    _uint iNumMeshes = m_pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModel->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(iPassNum)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }


    return S_OK;
}

_bool CElite::Check_Collision()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));


    CWeapon_Player* pPlayerWeapon = static_cast<CWeapon_Player*>(pPlayer->Get_Part(CPlayer::PART_WEAPON));
    CCollider* pCollider = pPlayerWeapon->Get_Collider();
    
    if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayerWeapon->Get_CurType())
    {
        m_pColliderCom->Intersect(pCollider);
    }
    
    
    if (m_pColliderCom->IsBoundingCollisionEnter())
    {
        _double& TrackPos = m_pModel->Get_Referene_CurrentTrackPosition();
        TrackPos = 0.0;
    

        // 충돌햇는데  플레이어가 공격한 방향과 내가 보고잇는 Look의 각도차이를 구하고
        // 그 각도 차이에 따라서   BLCOK F,L,R 뭘 할지를 정한다.     뒤로 막는건없다고 가정

        CTransform* pPlayerTransform = pPlayer->Get_Transform();
        
        _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
        vPlayerLook = XMVectorSetY(vPlayerLook, 0);

        _vector vPlayerLookNor = XMVector3Normalize(vPlayerLook);
        _vector vEliteLookNor = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

        _float fAngle = XMConvertToDegrees(acos(XMVectorGetX(XMVector3Dot(vPlayerLookNor, vEliteLookNor))));

        // 135 ~ 180    앞에서막아 
        // 135 ~ 90     오른쪽막아
        // 90도 벗어나게 되면?     못막아 


        if (135.f <= fAngle && 180.f >= fAngle)
        {
            m_pModel->SetUp_Animation(CElite::ELITE_ANIMATION::BLOCK_F, true);
        }
        else if (90.f <= fAngle && 135.f >= fAngle)
        {
            _vector vCross = XMVector3Cross(vPlayerLookNor, vEliteLookNor);

            // 외적의 z 성분을 확인하여 방향 판단
            if (XMVectorGetY(vCross) > 0)
            {
                // 왼쪽에서 막음
                m_pModel->SetUp_Animation(CElite::ELITE_ANIMATION::BLOCK_L, true);
            }
            else
            {
                // 오른쪽에서 막음
                m_pModel->SetUp_Animation(CElite::ELITE_ANIMATION::BLOCK_R, true);
            }
        }

        m_pTransformCom->LookAt_XZ(pPlayerTransform->Get_State(CTransform::STATE_POSITION));
        //m_pFsm->Change_State(CElite::ELITE_ANIMATION::DEATH_1);
    
        
 
        return true;
    }

    return false;
}



HRESULT CElite::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    /* For.Com_Fsm */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Elite_FSM"),
        TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsm), nullptr)))
        return E_FAIL;


    /* For.Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
    ColliderDesc.vExtents = _float3(3.f, 10.f, 3.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
    ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_CCollider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;



    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Elite"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    /* For.Com_Texture*/
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_DeadNoiseTexture"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDeadNoiseTexture), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CElite::Ready_Parts()
{
    m_Parts.resize(PART_END);

    CWeapon_Elite::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("Gun_r");
    WeaponDesc.pOwner = this;
    WeaponDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon_Elite"), &WeaponDesc)))
        return E_FAIL;


    CParticle_Blood::BLOOD_DESC	BloodDesc{};

    BloodDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BloodDesc.pSocketBoneMatrix = m_pModel->Get_BoneCombindTransformationMatrix_Ptr("spine_01");
    BloodDesc.pOwner = this;
    BloodDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_EFFECT, TEXT("Prototype_GameObject_Particle_Blood"), &BloodDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CElite::Ready_State()
{
    if (FAILED(m_pFsm->Add_State(CElite_Idle::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CElite_Block::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CElite_Death::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CElite_Alert::Create(this))))
        return E_FAIL;
    
    if (FAILED(m_pFsm->Add_State(CElite_Turbo::Create(this))))
        return E_FAIL;
    
    if (FAILED(m_pFsm->Add_State(CElite_Walk::Create(this))))
        return E_FAIL;

    return S_OK;
}

HRESULT CElite::Ready_Change_Layer()
{
    m_strChangeLayerName = L"Layer_Elite";

    return S_OK;
}

void CElite::Ready_Modify_Animation()
{
    vector<CAnimation*>& Animations = m_pModel->Get_Animations();


    Animations[CElite::ELITE_ANIMATION::TURBO_TO_DASH]->Set_SpeedPerSec(10.f);

}

CElite* CElite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CElite* pInstance = new CElite(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CElite"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CElite::Clone(void* pArg)
{
    CElite* pInstance = new CElite(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CElite"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CElite::Free()
{
    __super::Free();

    Safe_Release(m_pDeadNoiseTexture);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);

    Safe_Release(m_pColliderCom);
    Safe_Release(m_pFsm);
}
