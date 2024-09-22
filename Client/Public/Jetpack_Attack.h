#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CJetpack_Attack final : public CState
{
private:
    CJetpack_Attack(class CGameObject* pOwner);
    virtual ~CJetpack_Attack() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();




public:
    static CJetpack_Attack* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
