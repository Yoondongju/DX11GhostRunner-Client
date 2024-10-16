#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_Hook final : public CState
{
private:
    CPlayer_Hook(class CGameObject* pOwner);
    virtual ~CPlayer_Hook() = default;

public:
    class CGrapplingPointUI*        Get_GrapUI() { return m_pGrapUI; }


public:
    HRESULT         Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    class CGrapplingPointUI*        m_pGrapUI = { nullptr };
    _bool                           m_isStartHook = { false };


private:
    _bool           m_isHookSoundActive = { false };




public:
    static CPlayer_Hook* Create(class CGameObject* pOwner );
    virtual void Free() override;
};

END
