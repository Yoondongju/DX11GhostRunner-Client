#include "../Public/Event_Manager.h"
#include "../Public/GameObject.h"

#include "../Public/Level_Manager.h"
#include "../Public/Object_Manager.h"

#include "../Public/Level.h"

CEvent_Manager::CEvent_Manager()
{

}

HRESULT CEvent_Manager::Initialize()
{
	return S_OK;
}

void CEvent_Manager::update()
{
	for (auto& pDeadObject : m_Deadlist)
	{
		Safe_Release(pDeadObject);
	}
	m_Deadlist.clear();


	for (auto Event = m_Eventlist.begin(); Event != m_Eventlist.end();)
	{
		Excute(*Event);

		Event = m_Eventlist.erase(Event);
	}
	m_Eventlist.clear();
}



void CEvent_Manager::Excute(const EVENT tEvent)
{
	switch ((UINT)tEvent.eEven)
	{
	case (UINT)EVENT_TYPE::CREATE_OBJECT:
	{
		// lParam : 안씀
		// wParam : 안씀
		// pArg   : Clone_Desc 정보
		//const CLONE_DESC* CloneDesc = tEvent.CloneDesc;

		//m_pObject_Manager->Add_CloneObject_ToLayer(CloneDesc->iLevelIndex, CloneDesc->strLayerTag, CloneDesc->strPrototypeTag, CloneDesc->pArg);
	}
	break;

	case (UINT)EVENT_TYPE::DELETE_OBJECT:
	{
		// lParam : Object adress
		// wParam: Group Type
		// Object를 Dead상태로 변경
		// 삭제예정 오브젝트들을 모아둔다.

		CGameObject* pDeadObj = (CGameObject*)tEvent.lParam;
		m_Deadlist.push_back(pDeadObj);

	}
	break;


	case (UINT)EVENT_TYPE::CHANGE_LEVEL:
	{
		// lParam:   Level Index
		// wParam :  Next Scene Type
		//m_pLevel_Manager->Change_Level((_uint)tEvent.lParam, (CLevel*)tEvent.wParam);
	}
	break;

	case (UINT)EVENT_TYPE::CHANGE_STATE:
	{
		// lParam : AI
		// wParam : Next Type

	}
	break;
	}
}



CEvent_Manager* CEvent_Manager::Create()
{
	CEvent_Manager* pInstance = new CEvent_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CEvent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Manager::Free()
{
	__super::Free();


	for (auto pDeadObject : m_Deadlist)
		Safe_Release(pDeadObject);
	m_Deadlist.clear();

	m_Eventlist.clear();
}



