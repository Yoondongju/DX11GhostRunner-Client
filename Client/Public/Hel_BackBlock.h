#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CHel_BackBlock final : public CState
{
private:
    CHel_BackBlock(class CGameObject* pOwner);
    virtual ~CHel_BackBlock() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _bool       m_isStartBlock = { false };


public:
    static CHel_BackBlock* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
