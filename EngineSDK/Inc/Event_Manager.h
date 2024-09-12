#pragma once
#include "Base.h"

BEGIN(Engine)

class CEvent_Manager final : public CBase
{
public:
	enum EVENT_TYPE { DELETE_OBJECT, CREATE_OBJECT, CHANGE_LEVEL, CHANGE_STATE };

	typedef struct
	{
		EVENT_TYPE				eEven;

		DWORD_PTR				lParam;
		_uint					wParam;

	}EVENT;


private:
	CEvent_Manager(class CObject_Manager* pObject_Manager);
	virtual ~CEvent_Manager() = default;

public:
	HRESULT Initialize();
	void update();

public:
	void AddEvent(EVENT tEvent) { m_Eventlist.push_back(tEvent); }



private:
	list<EVENT>				  m_Eventlist;
	list<class CGameObject*>  m_Deadlist;

	class CObject_Manager*	  m_pObject_Manager = { nullptr };



private:
	void Excute(const EVENT tEvent);


public:
	static CEvent_Manager* Create(class CObject_Manager* pObject_Manager);
	virtual void Free() override;
};

END
