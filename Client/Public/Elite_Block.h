#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CElite_Block final : public CState
{
private:
    CElite_Block(class CGameObject* pOwner);
    virtual ~CElite_Block() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _bool   Check_Death();




public:
    static CElite_Block* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
