#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_MindControl final : public CState
{
private:
    CPlayer_MindControl(class CGameObject* pOwner);
    virtual ~CPlayer_MindControl() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    vector<class CGameObject*>       m_VisibleEnemy;                        // �þ߿� ���� ����
    _float                           m_fVisibleAngle = { 90.f };            // �þ� ���� 
    _float                           m_fVisiableDistance = {500.f};         // �߰��Ҽ��ִ� �ִ� �Ÿ�

    _bool                            m_isOrderCommand = { false };
    _uint                            m_iCanVisibleNum = { 3 };              // ���� ã�����ִ� ���Ͱ���

private:
    void             FindVisiableEnemy();
    void             CommandToAttackEachEnemy();        

public:
    static CPlayer_MindControl* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
