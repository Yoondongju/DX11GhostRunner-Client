#pragma once

#include "GameObject.h"

/* 직교투영이 필요한 다수의 객체가 존재할 수 있다.  */
/* 직교투영에 필요한 데이터들과 기능을 제공해주는 역활 */
BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float	fX{}, fY{}, fSizeX{}, fSizeY{};
	} UI_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;


public:
	void SetMovement(bool _b) { m_bMovement = _b; }
	bool GetMovement() { return m_bMovement; }

	bool IsMouseOn() { return m_bMouseOn; }
	bool IsLbtnDown() { return m_bLbtnDown; }

	CUIObject* GetParent() { return m_pParentUI; }
	list<CUIObject*>& GetChildUI() { return m_childUI_List; }

	_float				Get_fX() { return m_fX; }
	_float				Get_fY() { return m_fY; }

	_float				Get_fSizeX() { return m_fSizeX; }
	_float				Get_fSizeY() { return m_fSizeY; }

	void				Set_Active(_bool b) { m_bActivate = b; }

	void		 SetParentUI(CUIObject* _p)
	{
		m_pParentUI = _p;
	}


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float				m_fX{}, m_fY{}, m_fSizeX{}, m_fSizeY{};
	_float4x4			m_ViewMatrix{}, m_ProjMatrix{};
	_uint				m_iDepth = {};

	_float				m_fViewWidth{}, m_fViewHeight{};


protected:
	list<CUIObject*>				m_childUI_List;

	CUIObject*						m_pParentUI = { nullptr };	 // 부모 UI가 있니

	_bool							m_bMovement = { false };     // UI가 움직일수 있는지?
	_bool							m_bMouseOn = { false };		 // UI 위에 마우스가 있는지
	_bool							m_bLbtnDown = { false };	 // UI에 왼쪽버튼이 눌러져 있는지

	_bool							m_bActivate = { true };		 // 활성화
	

protected:
	HRESULT		Add_Child(CUIObject* pChild);



protected:
	virtual void MouseOnCheck();				// 모든 자식 UI들은 부모꺼를 쓸거임

	virtual void MouseLBtnDown();
	virtual void MouseLBtnUp();
	virtual void MouseClick();


	virtual void MouseOn();
	virtual void MouseOut();

	




public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

	friend class CUI_Manager;
};

END