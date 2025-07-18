#include "stdafx.h"
#include "Elite.h"
#include "GameInstance.h"

#include "FreeCamera.h"
#include "Player.h"

#include "Weapon_Elite.h"
#include "Particle_Blood.h"
#include "Particle_EliteBlock.h"
#include "Particle_EliteDashBlock.h"

#include "Animation.h"

#include "Elite_Idle.h"
#include "Elite_Alert.h"
#include "Elite_Block.h"
#include "Elite_Turbo.h"
#include "Elite_Walk.h"
#include "Elite_Death.h"
#include "Elite_Stun.h"
#include "Elite_CutScene.h"

#include "BossHpPanel.h"

#include "EliteMotionTrail.h"

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
    
    if (FAILED(Create_BossHp()))
        return E_FAIL;
    

    Ready_Modify_Animation();



    m_pModel->SetUp_Animation(CElite::ELITE_ANIMATION::WALK_F, true);
    m_pFsm->Set_State(CElite::ELITE_ANIMATION::CUTSCENE);


    _vector vElitePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


    m_pTransformCom->Scaling(3.5f, 3.5f, 3.5f);

    m_fEnergy = 100.f;
    m_fHp = 100.f;

    m_fCollisionCoolTime = 1.f;



    if (FAILED(Ready_PhysX()))          // 스케일까지 세팅해주고 피직스세팅
        return E_FAIL;


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
            m_pGameInstance->Delete(g_CurLevel, CRenderer::RG_NONBLEND, this);

        m_fDiscard += fTimeDelta * 0.4f;
    }

    if(m_fCollisionCoolTime > 0.f)
        m_fCollisionCoolTime -= fTimeDelta;


    m_fAddMotionTrailTime += fTimeDelta;


    _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    m_PxTransform.p = { pWorldMatrix->m[3][0], pWorldMatrix->m[3][1] * 2.f + 30.f, pWorldMatrix->m[3][2] };
    m_pPxRigidDynamic->setGlobalPose(m_PxTransform);

    PhysXComputeCollision();                


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


    m_pMotionTrail->Late_Update(fTimeDelta);

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



    deque<CEliteMotionTrail::MOTION_TRAIL_INFO>& MotionTrailInfo = m_pMotionTrail->Get_MotionTrailInfo();
    if (MotionTrailInfo.size() > 10)
    {
        MotionTrailInfo.pop_front();
    }

    _uint iNumMeshes = m_pModel->Get_NumMeshes();

    CEliteMotionTrail::MOTION_TRAIL_INFO  MotionTrail = {};         // 월행 , 본메트릭스, 라이프타임                        
    XMStoreFloat4x4(&MotionTrail.WorldMatrix, m_pTransformCom->Get_WorldMatrix());
    MotionTrail.fLifeTime = 1.f;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // 내 모션트레일의 -> 트레일정보의 -> 이 메시에 영향을 주는 뼈행렬의 첫번째 주소를 준다음
        // 저 안에서 값을 채운다   ( 일단 이거 메쉬 1개라고 가정하고 생각한거임 )
        m_pModel->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i, MotionTrail.BoneMatrices[i]);

        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_HEIGHT, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(iPassNum)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }

    if (true == m_pMotionTrail->Is_Active() && m_fAddMotionTrailTime >= 0.13f)
    {
        MotionTrailInfo.emplace_back(MotionTrail);
        m_fAddMotionTrailTime = 0.f;
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
    
    
    if (m_fCollisionCoolTime <= 0.f && m_pColliderCom->IsBoundingCollisionEnter())
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


        if (110.f <= fAngle && 180.f >= fAngle)
        {
            m_pModel->SetUp_Animation(CElite::ELITE_ANIMATION::BLOCK_F, true);
            m_pTransformCom->LookAt_XZ(pPlayerTransform->Get_State(CTransform::STATE_POSITION));
            m_pFsm->Change_State(CElite::ELITE_ANIMATION::BLOCK_F);
        }
        else if (90.f <= fAngle && 109.f >= fAngle)
        {
            _vector vCross = XMVector3Cross(vPlayerLookNor, vEliteLookNor);

            // 외적의 z 성분을 확인하여 방향 판단
            if (XMVectorGetY(vCross) < 0)
            {
                // 왼쪽에서 막음
                m_pModel->SetUp_Animation(CElite::ELITE_ANIMATION::BLOCK_L, true);
                m_pFsm->Change_State(CElite::ELITE_ANIMATION::BLOCK_F);              
            }
            else
            {
                // 오른쪽에서 막음
                m_pModel->SetUp_Animation(CElite::ELITE_ANIMATION::BLOCK_R, true);
                m_pFsm->Change_State(CElite::ELITE_ANIMATION::BLOCK_F);            
            }
        }
        else
        {
            m_pModel->SetUp_Animation(CElite::ELITE_ANIMATION::BLOCK_F, true);
            m_pTransformCom->LookAt_XZ(pPlayerTransform->Get_State(CTransform::STATE_POSITION));
            m_pFsm->Change_State(CElite::ELITE_ANIMATION::BLOCK_F);
        }
       

        m_Parts[CElite::PARTID::PART_PARTICLE_BLOCK]->SetActiveMyParticle(true);

        if (false == m_isEnterPage2)
        {          
            m_fEnergy -= 29.f;
            m_fCollisionCoolTime = 1.f;
        }
           
        if (m_fEnergy <= 0.f)
        {
            m_fEnergy = 0.f;
            m_pGameInstance->Set_TimeDelayActive(true, 1.f);
        }
           

       
        return true;
    }

    return false;
}

_bool CElite::Check_CollisionGroggy()       // 패링 3번 이상햇을때 켜짐
{
    if (true == m_isDead)
        return false;

    CWeapon_Player* pPlayerWeapon = static_cast<CWeapon_Player*>(static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY))->Get_Part(CPlayer::PART_WEAPON));
    CCollider* pCollider = pPlayerWeapon->Get_Collider();


    if (CWeapon_Player::WEAPON_TYPE::KATANA == pPlayerWeapon->Get_CurType())
    {
        m_pColliderCom->Intersect(pCollider);
    }


    if (m_fCollisionCoolTime <= 0.f && m_pColliderCom->IsBoundingCollisionEnter())
    {
        _double& TrackPos = m_pModel->Get_Referene_CurrentTrackPosition();
        TrackPos = 0.0;

        static_cast<CParticle_Blood*>(m_Parts[PART_EFFECT])->SetActiveMyParticle(true);
        m_fHp -= 18.f;

        if (m_fHp <= 50.f)
        {
            if (false == m_isEnterPage2)
            {
                m_isEnterPage2 = true;
                m_fEnergy = 100.f;
            
                m_pFsm->Change_State(CElite::ELITE_ANIMATION::CUTSCENE);
            }     
        }

        if (m_fHp <= 0.f)
        {
            m_pGameInstance->Set_TimeDelayActive(true, 1.f);
            m_fHp = 0.f;
            m_fEnergy = 0.f;

            m_pModel->SetUp_Animation(ELITE_ANIMATION::DEATH_1, true);
            m_pFsm->Change_State(ELITE_ANIMATION::DEATH_1);
        }
            

        m_pGameInstance->Play_Sound(TEXT("HitBlood1.ogg"), SOUND_PISTOLBLOOD, 3.f);

        m_fCollisionCoolTime = 1.f;
        return true;
    }

    return false;
}



void CElite::PhysXComputeCollision()
{
    m_pColliderCom->Set_PrePhysXCollision(m_pColliderCom->Get_CurPhysXCollision());	// 충돌 들어가기전에 항상 이전 충돌정보를 세팅!

    if (true == m_pGameInstance->CollisionUpdate_PlayerToTriangleMeshGeometry(&m_vDir, &m_vDepth, m_pShape, &m_PxTransform, &m_pCollisionDestObject))
    {
        m_pColliderCom->Set_CurPhysXCollision(true);

        _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

        //pWorldMatrix->m[3][0] += m_vDir.x * (m_vDepth + 0.1f);
        //pWorldMatrix->m[3][1] += m_vDir.y * (m_vDepth + 0.1f);
        //pWorldMatrix->m[3][2] += m_vDir.z * (m_vDepth + 0.1f);            // Y축 충돌이 섞여서 문제가 생겨 

        // 충돌한 객체가 벽을 탈수있는애인걸 확인해야함

        if (m_pColliderCom->IsPhysXCollisionEnter())
        {
            _bool	bDashBlock = true;

            m_pModel->SetUp_Animation(CElite::ELITE_ANIMATION::DASH_TO_ALERDLB, true);
            m_pFsm->Change_State(CElite::ELITE_ANIMATION::BLOCK_F, -1, &bDashBlock);
        }

    }
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
    if (FAILED(__super::Add_Component(g_CurLevel, TEXT("Prototype_Component_Model_Elite"),
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





    _float3				KatanaStartLocal = _float3(-0.0776796862, -0.202111647, 0.839834869);
    _float3				KatanaEndLocal = _float3(0.0776796862, 1.43591797, 9.31558323);
    _float3             KatanaMiddleLocal;

    KatanaMiddleLocal.x = (KatanaStartLocal.x + KatanaEndLocal.x) * 0.5f;
    KatanaMiddleLocal.y = (KatanaStartLocal.y + KatanaEndLocal.y) * 0.5f;
    KatanaMiddleLocal.z = (KatanaStartLocal.z + KatanaEndLocal.z) * 0.5f;



    CParticle_EliteBlock::PARTICLE_BLOCKEFFECT	EliteBlockDesc{};
    
    EliteBlockDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    EliteBlockDesc.pOwner = this;
    EliteBlockDesc.SpwanPositionLocal = KatanaMiddleLocal;
    if (FAILED(__super::Add_PartObject(PART_PARTICLE_BLOCK, TEXT("Prototype_GameObject_Particle_EliteBlockEffect"), &EliteBlockDesc)))
        return E_FAIL;


    CParticle_EliteDashBlock::PARTICLE_DASHBLOCKEFFECT	EliteDashBlockDesc{};

    EliteDashBlockDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    EliteDashBlockDesc.pOwner = this;
    EliteDashBlockDesc.SpwanPositionLocal = KatanaMiddleLocal;
    if (FAILED(__super::Add_PartObject(PART_PARTICLE_DASHBLOCK, TEXT("Prototype_GameObject_Particle_EliteDashBlockEffect"), &EliteBlockDesc)))
        return E_FAIL;




    m_pMotionTrail = static_cast<CEliteMotionTrail*>(m_pGameInstance->Clone_GameObject(L"Prototype_GameObject_EliteMotionTrail"));



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

    if (FAILED(m_pFsm->Add_State(CElite_Stun::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CElite_CutScene::Create(this))))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CElite::Ready_PhysX()
{
    _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    m_PxTransform = PxTransform(PxMat44(pWorldMatrix->_11));

    PxVec3 vStartPos = { pWorldMatrix->m[1][0] , pWorldMatrix->m[1][1] , pWorldMatrix->m[1][2] };
    PxVec3 vEndPos = { -pWorldMatrix->m[1][0], -pWorldMatrix->m[1][1] , -pWorldMatrix->m[1][2] };
    m_PxTransform = PxTransformFromSegment(vStartPos, vEndPos);


    m_pPxRigidDynamic = m_pGameInstance->Get_Physics()->createRigidDynamic(m_PxTransform);

    PxMaterial* Material = m_pGameInstance->Get_Material();


    vector<CMesh*>& Meshs = m_pModel->Get_Meshes();


    // 4. Triangle Mesh 기반 Shape 생성
    m_pShape = m_pGameInstance->Get_Physics()->createShape(PxCapsuleGeometry(20.f, 1.f), *Material);
    if (!m_pShape) {
        MSG_BOX(TEXT("createShape failed!"));
        return E_FAIL;
    }


    //PxTransform localPose(PxVec3(0.f, 10.f, 0.f));  // (5.f, 0.f, 0.f) 위치로 이동
    //m_pShape->setLocalPose(localPose);
    m_pPxRigidDynamic->attachShape(*m_pShape);
    m_pGameInstance->Get_Scene()->addActor(*m_pPxRigidDynamic);



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


    Animations[CElite::ELITE_ANIMATION::TURBO_TO_DASH]->Set_SpeedPerSec(15.f);
    Animations[CElite::ELITE_ANIMATION::HIT_STUN]->Set_SpeedPerSec(15.f);
}

HRESULT CElite::Create_BossHp()
{
    CBossHpPanel::BOSSHP_DESC PanelDesc = {};
    PanelDesc.eBossType = CBossHpPanel::BOSSTYPE::ELITE;
    PanelDesc.pOwner = this;
    PanelDesc.fX = g_iWinSizeX * 0.5f;
    PanelDesc.fY = 100.f;
    PanelDesc.fSizeX = 1360;
    PanelDesc.fSizeY = 30;
    PanelDesc.fSpeedPerSec = 10.f;
    PanelDesc.fRotationPerSec = XMConvertToRadians(90.0f);
    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STAGE1_BOSS, L"Layer_BossHpUI", L"Prototype_GameObject_BossHpPanel", L"No Model", &PanelDesc)))
        return E_FAIL;

   
 



    return S_OK;
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

    Safe_Release(m_pCollisionDestObject);
}
