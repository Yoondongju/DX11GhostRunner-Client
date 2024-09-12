#include"../Public/KeyManager.h"

int g_arrKey[(_uint)KEY::LAST] =
{
	VK_LEFT, //LEFT,
	VK_RIGHT,//RIGHT,
	VK_UP,//UP,
	VK_DOWN,//DOWN,
	'Q',
	'W',
	'E',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',

	'A',
	'S',
	'D',
	'F',
	'G',
	'Z',
	'X',
	'C',
	'V',
	'B',

	VK_MENU,
	VK_CONTROL,
	VK_LSHIFT,
	VK_SPACE,
	VK_RETURN,
	VK_ESCAPE,

	VK_LBUTTON,
	VK_RBUTTON,

	'1',
	'2',
	'3',
	'4'
};

CKeyManager::CKeyManager()
{


}

HRESULT CKeyManager::Initialize(HWND hWnd)
{
	for (int i = 0; i < (int)KEY::LAST; i++)
	{
		m_vecKey.push_back(KEYINFO{ KEY_STATE::NONE,false });
	}

	m_hWnd = hWnd;

	return S_OK;
}


void CKeyManager::Update()
{
	// ������ ��Ŀ�� �˾Ƴ���
	//HWND hMainWnd = CCore::GetInst()->GetMainHwnd();
	HWND hWnd = GetFocus();

	// ������ ��Ŀ�� ���϶�
	if (hWnd != nullptr)
	{
		for (size_t i = 0; i < (int)KEY::LAST; i++)
		{
			if (GetAsyncKeyState(g_arrKey[i]) & 0x8000)   // �ش� Ű�� ���ȴٸ�?
			{
				if (m_vecKey[i].bPrev) // �������� ����
				{
					m_vecKey[i].eState = KEY_STATE::HOLD;
				}
				else // ������ �ȴ��ȴ�
				{
					m_vecKey[i].eState = KEY_STATE::TAP;
				}
				m_vecKey[i].bPrev = true;
			}

			else // �ش�Ű�� �ȴ���
			{
				if (m_vecKey[i].bPrev) // ������ ����
				{
					m_vecKey[i].eState = KEY_STATE::AWAY;
				}
				else // �������� �ȴ��ȴ�
				{
					m_vecKey[i].eState = KEY_STATE::NONE;
				}
				m_vecKey[i].bPrev = false;
			}
		}
	}

	else // ��Ŀ�� �� �ƴҶ�
	{
		for (size_t i = 0; i < (_uint)KEY::LAST; i++)
		{
			m_vecKey[i].bPrev = false;



			if (m_vecKey[i].eState == KEY_STATE::TAP || m_vecKey[i].eState == KEY_STATE::HOLD)
			{
				m_vecKey[i].eState = KEY_STATE::AWAY;
			}

			else if (m_vecKey[i].eState == KEY_STATE::AWAY)
			{
				m_vecKey[i].eState = KEY_STATE::NONE;
			}

		}
	}
}

CKeyManager* CKeyManager::Create(HWND hWnd)
{
	CKeyManager* pInstance = new CKeyManager();

	if (FAILED(pInstance->Initialize(hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CKeyManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CKeyManager::Free()
{
	__super::Free();
}
