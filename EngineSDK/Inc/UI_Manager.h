#pragma once
#include "Base.h"

BEGIN(Engine)
class CUIObject;

class CUI_Manager final
	: public CBase
{
private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT Initialize();
	void Update();

	void		SetFocusUI(CUIObject* _pUI);

	CUIObject* GetFocusUI();
	CUIObject* GetTargetUI(CUIObject* _pFocusUI);

public:
	class CGameInstance* m_pGameInstance = { nullptr };

	CUIObject* m_pFocusUI = { nullptr };

public:
	static CUI_Manager* Create();
	virtual void Free() override;


};

END
