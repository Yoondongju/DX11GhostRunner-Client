#include "../Public/UI_Manager.h"
#include "../Public/GameInstance.h"

#include "../Public/UIObject.h"

CUI_Manager::CUI_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Manager::Initialize()
{
	return S_OK;
}

void CUI_Manager::Update()
{
	// 포커싱 된 UI를 찾는다

	m_pFocusUI = GetFocusUI();

	if (m_pFocusUI)
	{
		//Dead 처리를 어떻게하지
		/*if (m_pFocusUI->Get_Dead())
		{
			m_pFocusUI = nullptr;
			return;
		}			*/
	}
	else
		return;



	CUIObject* pTargetUI = GetTargetUI(m_pFocusUI);

	bool bLbtnTap = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP;
	bool bLbtnAway = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::AWAY;   // 딱 뗀 시점

	if (pTargetUI)
	{
		if (bLbtnTap)
		{
			pTargetUI->MouseLBtnDown();
			pTargetUI->m_bLbtnDown = true;
		}
		else if (bLbtnAway)
		{
			pTargetUI->MouseLBtnUp();

			if (pTargetUI->m_bLbtnDown)
				pTargetUI->MouseClick();

			pTargetUI->m_bLbtnDown = false;
		}
	}



}


void CUI_Manager::SetFocusUI(CUIObject* _pUI)
{
	m_pFocusUI = _pUI;

}



CUIObject* CUI_Manager::GetFocusUI()
{
	//CScene* pCureScene = CSceneMgr::CreateSingleTonInst()->GetCurScene();

	list<CGameObject*>& playerUIList = m_pGameInstance->Get_RenderList(CRenderer::RG_UI);

	bool bLbtnTap = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP;
	bool bLbtnAway = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::AWAY;

	bool bLbtnHold = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::HOLD;


	if (0 == playerUIList.size())
	{
		m_pFocusUI = nullptr;
	}

	CUIObject* pFocusUI = m_pFocusUI;

	list<CUIObject*> queue;
	CUIObject* pUI = nullptr;
	CUIObject* pTargetUI = nullptr;

	list<CGameObject*>::iterator targetiter = playerUIList.end();
	list<CGameObject*>::iterator iter = playerUIList.begin();

	if (!bLbtnTap)  // 내가 누른적이 없다면 포커싱은 안주되, MouseOn은 호출해줘야함..
	{
		for (; iter != playerUIList.end(); ++iter)
		{
			pUI = ((CUIObject*)*iter);  // 최상위 부모
			queue.push_back(pUI); // 그 부모를 큐에 넣고

			while (!queue.empty())
			{
				pUI = queue.front();
				queue.pop_front();

				if (pUI->IsMouseOn())
				{
					pTargetUI = pUI;

					if (nullptr != pTargetUI)
						pTargetUI->MouseOn();
				}

				else if (!pUI->IsMouseOn())
				{
					pTargetUI = pUI;

					if (nullptr != pTargetUI)
						pTargetUI->MouseOut();
				}

				// 그 부모 pUI의 자식UI벡터를 vecChild한테 줌
				const list<CUIObject*>& PlayerUIList = pUI->GetChildUI();

				// queue에 자식 UI추가
				for (auto& iter : PlayerUIList)
				{
					queue.push_back(iter);
				}
			}
		}

		return pFocusUI;
	}

	if (bLbtnTap || bLbtnHold)
	{
		for (iter; iter != playerUIList.end(); ++iter)
		{
			pUI = ((CUIObject*)(*iter));
			bool m = pUI->IsMouseOn();
			if (pUI->IsMouseOn()) // && pUI->GetActivate()
			{
				targetiter = iter;
			}
		}
	}

	if (targetiter == playerUIList.end())
	{
		return pFocusUI;
	}


	pFocusUI = (CUIObject*)(*targetiter);


	playerUIList.erase(targetiter);
	playerUIList.push_back(pFocusUI);

	return pFocusUI;
}

CUIObject* CUI_Manager::GetTargetUI(CUIObject* _pFocusUI)
{

	CUIObject* pTargetUI = nullptr;

	bool bLbtnAway = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::AWAY;   // 딱 뗀 시

	static list<CUIObject*> queue;
	static vector<CUIObject*> vecNoneTarget;


	queue.push_back(_pFocusUI);


	while (!queue.empty())			// 안에 들어있는 자식들이 있을때까지 돌고
	{
		CUIObject* pUI = queue.front();
		queue.pop_front();

		if (pUI->IsMouseOn())
		{
			if (nullptr != pTargetUI)
			{
				vecNoneTarget.push_back(pTargetUI);
			}
			pTargetUI = pUI;
		}
		else
		{
			vecNoneTarget.push_back(pUI);
		}


		const list<CUIObject*>& PlayerUIList = pUI->GetChildUI();

		for (auto& iter : PlayerUIList)
		{
			queue.push_back(iter);
		}
	}


	//if (bLbtnAway)
	//{
	//	for (auto& iter : vecNoneTarget)
	//	{
	//		iter->m_bLbtnDown = false;
	//	}
	//}

	vecNoneTarget.clear();

	return pTargetUI;
}

CUI_Manager* CUI_Manager::Create()
{
	CUI_Manager* pInstance = new CUI_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pFocusUI);
}
