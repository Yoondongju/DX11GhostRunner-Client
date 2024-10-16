#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CJetpack_Fly final : public CState
{
private:
    CJetpack_Fly(class CGameObject* pOwner);
    virtual ~CJetpack_Fly() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _float      m_fCanFindPlayerDistance = {160.f};
    _float      m_fCanFollowPlayerDistance = { 320.f };

    _bool       m_isFindPlayer = { false };
    _float      m_fFlyTime = { 2.f };
    _bool       m_isFlyUp = { false };
    _bool       m_isForceVelocity = { false };


    _float3     m_fOriginRight = {};
    _float3     m_fOriginUp = {};
    _float3     m_fOriginLook = {};

private:
    _bool       Check_Detect();
    _bool       Check_FollowDetect();

public:
    static CJetpack_Fly* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
