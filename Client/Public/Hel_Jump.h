#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CHel_Jump final : public CState
{
private:
    CHel_Jump(class CGameObject* pOwner);
    virtual ~CHel_Jump() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _bool       m_isJumpStart = { false };
    _bool       m_isJumpExcute = { false };

public:
    static CHel_Jump* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
