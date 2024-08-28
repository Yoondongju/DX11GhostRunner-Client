#include "stdafx.h"
#include "Player.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "Body_Player.h"
#include "Weapon_Player.h"

#include "Player_Idle.h"
#include "Player_Dash.h"
#include "Player_Block.h"
#include "Player_Attack.h"
#include "Player_Hook.h"
#include "Player_Climb.h"
#include "Player_Walk.h"
#include "Player_Run.h"
#include "Player_Jump.h"



CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
{

}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject(Prototype)
{

}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_State()))
        return E_FAIL;


    XMStoreFloat4x4(&m_RotationMatrix, XMMatrixIdentity());

    m_eCurAnimationID = PLAYER_ANIMATIONID::IDLE;
    m_pFsm->Set_State(PLAYER_ANIMATIONID::IDLE);


    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::HOLD)
        m_pTransformCom->Go_Straight(fTimeDelta);
    if (m_pGameInstance->Get_KeyState(KEY::S) == KEY_STATE::HOLD)
        m_pTransformCom->Go_Backward(fTimeDelta);
    if (m_pGameInstance->Get_KeyState(KEY::A) == KEY_STATE::HOLD)
        m_pTransformCom->Go_Left(fTimeDelta);
    if (m_pGameInstance->Get_KeyState(KEY::D) == KEY_STATE::HOLD)
        m_pTransformCom->Go_Right(fTimeDelta);

    POINT ptMousePos = {};
    GetCursorPos(&ptMousePos);

    if (g_hWnd == GetFocus() && m_pGameInstance->Get_KeyState(KEY::RBUTTON) == KEY_STATE::HOLD)
    {
        _long	MouseMove = {};
        static CGameObject* pPlayer = m_pGameInstance->Find_Player();

        if (MouseMove = ptMousePos.x - m_ptOldMousePos.x)
        {
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.07f, &m_RotationMatrix);
        }

        if (MouseMove = ptMousePos.y - m_ptOldMousePos.y)
        {
            m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.07f, &m_RotationMatrix);
        }

    }

    m_ptOldMousePos = ptMousePos;




    for (auto& pPartObject : m_Parts)
        pPartObject->Priority_Update(fTimeDelta);

}

void CPlayer::Update(_float fTimeDelta)
{ 
    m_pFsm->Update(fTimeDelta);
    m_pRigidBody->Update(fTimeDelta);


    for (auto& pPartObject : m_Parts)
        pPartObject->Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_Parts)
        pPartObject->Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{


    return S_OK;
}

HRESULT CPlayer::Ready_Component()
{
    /* For.Com_Fsm */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Player_FSM"),
        TEXT("Com_Fsm"), reinterpret_cast<CComponent**>(&m_pFsm), nullptr)))
        return E_FAIL;

    /* For.Com_RigidBody*/
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_RigidBody"),
        TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), nullptr)))
        return E_FAIL;
    m_pRigidBody->Set_OwnerTransform(m_pTransformCom);



    return S_OK;
}

HRESULT	CPlayer::Ready_State()
{
    if (FAILED(m_pFsm->Add_State(CPlayer_Idle::Create(this))))
        return E_FAIL;
    
    if (FAILED(m_pFsm->Add_State(CPlayer_Dash::Create(this))))
        return E_FAIL;
    
    if (FAILED(m_pFsm->Add_State(CPlayer_Block::Create(this))))
        return E_FAIL;
    
    if (FAILED(m_pFsm->Add_State(CPlayer_Attack::Create(this))))
        return E_FAIL;
    
    if (FAILED(m_pFsm->Add_State(CPlayer_Hook::Create(this))))
        return E_FAIL;
    
    if (FAILED(m_pFsm->Add_State(CPlayer_Climb::Create(this))))
        return E_FAIL;
    
    if (FAILED(m_pFsm->Add_State(CPlayer_Walk::Create(this))))
        return E_FAIL;
    
    if (FAILED(m_pFsm->Add_State(CPlayer_Run::Create(this))))
        return E_FAIL;

    if (FAILED(m_pFsm->Add_State(CPlayer_Jump::Create(this))))
        return E_FAIL;

    return S_OK;
}






HRESULT CPlayer::Ready_PartObjects()
{
    /* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
    m_Parts.resize(PART_END);

    CBody_Player::BODY_DESC		BodyDesc{};
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.pOwner = this;
    BodyDesc.InitWorldMatrix = XMMatrixIdentity();
    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
        return E_FAIL;


    CWeapon_Player::WEAPON_DESC		WeaponDesc{};
    WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pSocketBoneMatrix = dynamic_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Weapon_r");
    WeaponDesc.pOwner = this;
    WeaponDesc.InitWorldMatrix = XMMatrixIdentity();


    if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon_Player"), &WeaponDesc)))
        return E_FAIL;


    return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();

    // 애초에 파트 오브젝트는 레이어에 담겨잇지않아서 사본의 Free가 호출되지않음.
    // State도 마찬가지
    // 레이어에 담기지않는 녀석들은 m_pOwner 이런걸 쓰면안되겠다 릭을 못찾겠어
    
    // 깨달은점 : 1. 서로 다른 클래스에서 멤버로 순환참조 들고 있게하지말자
    //           2. 만약 Clone Object일경우 다른곳에서 이놈의 레퍼런스카운트를 증가시켜 이 사본의 Free가 호출되지 않는다면 이 사본의 멤버인 객체를 해제할수없다.
    //           3. 만약 레이어에 담기지 않는 오브젝트일경우 Layer쪽에서 호출이 안되기에 Free가 호출안되서 릭날 가능성도 있따...... 



    Safe_Release(m_pFsm);
    Safe_Release(m_pRigidBody);
}
