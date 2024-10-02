#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CFsm :
	public CComponent
{

private:
	CFsm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFsm(const CFsm& Prototype);
	virtual ~CFsm() = default;

public:
	//class CGameObject* Get_Owner() { return m_pOwner; }
	_uint			Get_CurStateIndex();
	class CState*	Get_CurState() { return m_pNowState; }

public:
	virtual HRESULT Initialize_Prototype(_uint iNumMaxState);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta);
	virtual void Release_States();

public:
	HRESULT				Add_State(class CState* pState);
	void				Change_State(_uint iStateIndex, _int iStateDir = -1, void* pArg = nullptr);
	void				Set_State(_uint iStateIndex, void* pArg = nullptr);

private:
	vector<class CState*> m_States;						// 모든 상태를 갖고 있는 벡터 - 인덱스 접근용
	_uint				  m_iNumMaxState = {};			// 이 Fsm이 가질수있는 최대 State갯수 

	class CState* m_pNowState = { nullptr };			//지금 상태
	class CState* m_pPreState = { nullptr };			//이전 상태

	

public:
	static CFsm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumMaxState);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
