#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidBody final : public CComponent
{
public:
    enum FORCEMODE { FORCE, ACCELERATION, IMPLUSE, VELOCITYCHANGE, MODE_END };


private:
    CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRigidBody(const CRigidBody& Prototype);
    virtual ~CRigidBody() = default;

public:
    void                Set_OwnerTransform(class CTransform* pOwnerTransform);



    _float              Get_Mass() { return m_fMass; }
    void                Set_Mass(_float fMass) { m_fMass = fMass; }

    void                Set_ZeroTimer() { m_fTime = 0.f; }

    _vector             Get_Force() { return XMLoadFloat3(&m_vForce); }
    void                Set_Accel(_float3 vAccel) { m_vAccel = vAccel; }

    _vector             Get_Velocity() { return XMLoadFloat3(&m_vVelocity); }
    void                Set_Velocity(_float3 _vVelocity) { m_vVelocity = _vVelocity; }

    void                AddVelocity(_float3 _vAddV)
    {
        XMStoreFloat3(&m_vVelocity , XMLoadFloat3(&m_vVelocity) + XMLoadFloat3(&_vAddV));
    }

    _bool               Get_IsGravity() { return m_isGravity; }
    void                Set_IsGravity(bool isGravity) {
        m_isGravity = isGravity;
        if (!m_isGravity && m_vVelocity.y < 0)
            m_vVelocity.y = 0.f;
    }

    void                Set_VelocityLimit(_float3 vMaxVelocity) { m_vMaxVelocity = vMaxVelocity; }

    void                Set_Friction(_float fFriction) { m_fFriction = fFriction; }

    void                Set_GravityAccel(_float fGravityAccel) { m_fGravityAccel = fGravityAccel; }
    void                Set_GravityLimit(_float fGravityLimit) { m_fGravityLimit = fGravityLimit; }
    void                Set_GravityReduceRatio(_float fRatio) { m_fReducedGravity = fRatio; }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg) override;
    void Update(_float fTimeDelta, _float fTargetY , _bool isColl);

public:
    void Add_Force(FORCEMODE eForceMode, _fvector vForce);   // ���� �ִ� �Լ�
    void Add_Force_Direction(_fvector vDirection, _float fMagnitude, FORCEMODE eForceMode); // Ư�� �������� ���� �ִ� �Լ�



private:
    class CTransform* m_pOwnerTransform = { nullptr };


    _float                  m_fMass = { 1.f };                  // ����
    _float                  m_fGravityScale = { 5.f };          // �߷� ũ��
    _float                  m_fGravityAccel = { 9.8f };         // �߷� ���ӵ�
    _float                  m_fGravityLimit = { 400.f };         // �߷� �ִ� ��
    _float                  m_fFriction = { 0.3f };             // ������


    _float3                 m_vForce = { 0.f,0.f,0.f };                     // �������� ��
    _float3                 m_vAccel = { 0.f,0.f,0.f };                     // ���ӵ�
    _float3                 m_vVelocity = { 0.f,0.f,0.f };                  // �ӵ�
    _float3                 m_vMaxVelocity = { 5000.f,5000.f,5000.f };      // �ְ� �ӵ�




    _bool                   m_isActive = { true };      // Ȱ��ȭ ���־�?
    _bool                   m_isGravity = { true };     // �߷� �����Ұž�?
    _float                  m_fReducedGravity = { 1.f };   // Ư�� ��Ȳ���� �߷� ���̱�����
    _bool                   m_isGround = { false };     // ���� �پ��־�?
    _bool                   m_isKinematic = { false };  // true�� ����ȿ���� �� �� �ž�

    _float                  m_fTime = { 0.f };




private:
    void Calculate_Tranform(_float fTimeDelta, _float fTargetY); // ���� ��ġ ���
    void Calculate_Gravity(_float fTargetY); // �߷� ���
    void Calculate_Friction(); // ���� ���



public:
    static CRigidBody* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END