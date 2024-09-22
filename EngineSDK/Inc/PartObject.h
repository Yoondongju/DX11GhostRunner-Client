#pragma once

/* 컨테이너 오브젝트안에 추가될 수 있는 하나의 부품객체들의 부모가 클래스 */
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentWorldMatrix = { nullptr };
		CGameObject* pOwner = { nullptr };

	}PARTOBJ_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;


public:
	_float4x4*    Get_PartObjectComBindWorldMatrixPtr() { return &m_WorldMatrix; }
	
public:
	void					SetActiveMyParticle(_bool b) { m_isActiveMyParticle = b; }
	_bool					IsActiveMyParticle() { return m_isActiveMyParticle; }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	/* m_pTransformCom->m_WorldMatrix * 부모의 월드 */
	const _float4x4*		m_pParentMatrix = { nullptr };
	_float4x4				m_WorldMatrix = {};

	CGameObject*			m_pOwner = { nullptr };

	_bool					m_isActiveMyParticle = { false };
	

protected:
	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char * pContantName);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END