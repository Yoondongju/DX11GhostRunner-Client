#pragma once

#include "Client_Defines.h"
#include "State.h"


BEGIN(Client)

class CPlayer_CutAll final : public CState
{
public:
    struct Float3Wrapper {
        class CGameObject*  pSelf = { nullptr };    // �ڱ� �ڽ��� �ּ�
        _float3             fValue;        
        _float              fDistanceToPlayer;      // �÷��̾���� �Ÿ��� �̸� ����� ��

        // �Ÿ� �������� ��
        bool operator<(const Float3Wrapper& other) const {
            return fDistanceToPlayer < other.fDistanceToPlayer;  // �Ÿ��� �������� �������� ����
        }
    };




private:
    CPlayer_CutAll(class CGameObject* pOwner);
    virtual ~CPlayer_CutAll() = default;

public:
    virtual HRESULT Initialize();
    virtual HRESULT Start_State(void* pArg = nullptr);
    virtual void    Update(_float fTimeDelta);
    virtual void    End_State();


private:
    vector<Float3Wrapper>            m_Targets = {};                  



    _float             m_fVisibleAngle = { 90.f };               // �þ� ���� 
    _float             m_fVisiableDistance = { 600.f };         // �߰��Ҽ��ִ� �ִ� �Ÿ�
  

    _uint              m_iNumHunt = {};                 // ���� ����� Ÿ�� ����

    _float             m_fTargetDeleteTime = {};        // Ÿ���� �����ϱ����� ���ð�

    _float             m_fStartTime = {};
    _float             m_fAccSpeed = {};
    _bool              m_isHuntStart = { false };
    _uint              m_iNumKill = { 0 };

private:
    void        FindVisiableEnemy();
    void        Check_Collision();

public:
    static CPlayer_CutAll* Create(class CGameObject* pOwner);
    virtual void Free() override;
};

END
