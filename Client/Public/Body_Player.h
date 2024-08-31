#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END


BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{

	}BODY_DESC;



private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

public:
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;
	virtual CModel* Get_Model() override { return m_pModelCom; }
	virtual CCollider* Get_Collider() { return m_pColliderCom; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };



private:
	PxTransform				m_PxTransform = {};
	PxRigidDynamic* m_pPxRigidDynamic = { nullptr };
	PxShape* m_pShape = { nullptr };

	PxVec3					m_vDir = {};
	PxReal					m_vDepth = {};



private:
	HRESULT Ready_Components();

	void	Ready_Modify_Animation();

	HRESULT	Ready_PhysX();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END