#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CElite_Turbo final : public CState
{
private:
    CElite_Turbo(class CGameObject* pOwner);
    virtual ~CElite_Turbo() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    _bool           m_bStartTurboDash = { false };
    _float          m_fAccSpeed = { 0.f };
    _float3         m_GoDirNor = {};

    _float          m_fMaxRushDistance = { 250 - 30 };     // �ִ� ���� �Ÿ�     Elite_Alert�� �߰��Ҽ��ִ� Ž���Ÿ��� �Ͳٸ�����
 
    _float3         m_RushStartPos = {};                   // ������ ó�� ������ �Ÿ�

    _uint           m_iCountSuccessParrying = { 0 };

private:
    _bool   Check_Death();
    _bool   Check_TurboDash(_float fTimeDelta);





public:
    static CElite_Turbo* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
