#pragma once

/* 컨테이너 오브젝트안에 추가될 수 있는 하나의 부품객체들의 부모가 클래스 */
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float4x4*	 pParentWorldMatrix = { nullptr };
		CGameObject* pOwner = { nullptr };

	}PARTOBJ_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;


public:
	_float4x4*    Get_PartObjectComBindWorldMatrixPtr() { return &m_WorldMatrix; }
	
public:
	virtual void			SetActiveMyParticle(_bool b, _bool isOtherFlag = false) { m_isActiveMyParticle = b; }
	// 만약 파생 클래스에서 함수 오버라이딩을 하지 않았다면, 부모 클래스의 두 함수 중 하나가 선택됩니다.
	// 이때, ** 함수 시그니처가 더 포괄적인 버전(매개변수 두 개 버전)** 이 선택될 수 있습니다.

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
	_float4x4*				m_pParentMatrix = { nullptr };
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