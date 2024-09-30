#include "stdafx.h"
#include "..\Public\IconUI.h"

#include "GameInstance.h"

#include "Player.h"
#include "Weapon_Player.h"

#include "NumberUI.h"


_bool* CIconUI::m_NextIconArrive[ICON_TYPE::ICON_TYPE_END] = { nullptr };

CIconUI::CIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CIconUI::CIconUI(const CIconUI& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CIconUI::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CIconUI::Initialize(void* pArg)
{
	ICON_DESC* pDesc = static_cast<ICON_DESC*>(pArg);
	
	if (nullptr != pDesc)
	{
		m_eType = pDesc->eType;
	}


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	Safe_AddRef(m_pPlayer);


	m_vOriginSize.x = m_fSizeX;
	m_vOriginSize.y = m_fSizeY;


	m_vTargetSize.x = m_fSizeX + 20.f;
	m_vTargetSize.y = m_fSizeY + 20.f;



	m_vTargetPos.x = m_fX;
	m_vTargetPos.y = m_fY;

	m_vInSidePos.x = - m_fSizeX;		// 안에 박혀있는 위치
	m_vInSidePos.y = m_fY;				// 안에 박혀있는 위치


	m_NextIconArrive[m_eType] = &m_isArrive;
	if (HOMING_SH == m_eType)
		m_bActivate = false;



	CUIObject::UI_DESC			Desc{};
	Desc.fX = m_fX;
	Desc.fY = m_fY;
	Desc.fSizeX = 20.f;
	Desc.fSizeY = 20.f;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	CUIObject* pDotUI = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_NumberUI"), &Desc));
	if (FAILED(Add_Child(pDotUI)))
		return E_FAIL;

	return S_OK;
}

void CIconUI::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CIconUI::Update(_float fTimeDelta)
{
	CWeapon_Player::WEAPON_TYPE eCurType = m_pPlayer->Get_CurWeaponType();
	CWeapon_Player::WEAPON_TYPE ePreType = m_pPlayer->Get_PreWeaponType();

	if (eCurType != ePreType)
	{
		for (_uint i = 0; i < CIconUI::ICON_TYPE::ICON_TYPE_END; i++)
		{
			*m_NextIconArrive[i] = false;
		}
		m_fAccTime = 0.f;
	}

	if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == eCurType)
	{		
		Go_InSide(fTimeDelta);
	}
	else
	{
		Go_Out(fTimeDelta);		
	}



	if (false == m_bActivate)
		return;



	switch (m_eType)
	{
	case Client::CIconUI::BLOCK:
	{
		CNumberUI* pNmuber = static_cast<CNumberUI*>(m_childUI_List.back());

		if (false == m_pPlayer->IsBlockActive())
		{
			if (!m_bChangeEnter && m_vTargetSize.x > m_fSizeX &&
				m_vTargetSize.y > m_fSizeY)
			{
				m_fSizeX += fTimeDelta * 80.f;
				m_fSizeY += fTimeDelta * 80.f;
			}
			else
			{
				m_pPlayer->Set_StartCountBlockTime(true);
				m_bChangeEnter = true;

				_float fBlockCoolTime = m_pPlayer->Get_BlockCoolTime();
				_float fRemainingTime = m_pPlayer->Get_BlockRemainingTime();
				pNmuber->Set_Active(true);

				_float fResult = (fBlockCoolTime - fRemainingTime);


				if (fResult >= 1.0f)
				{
					fResult = 2.f;
				}
				else if (fResult >= 0.0f)
				{
					fResult = 1.f;
				}
				pNmuber->Set_Number((_int)fResult);


				if (m_vOriginSize.x > m_fSizeX &&
					m_vOriginSize.y > m_fSizeY)
				{
					m_fSizeX = m_vOriginSize.x;
					m_fSizeY = m_vOriginSize.y;
				}
				else
				{
					m_fSizeX -= fTimeDelta * 40.f;
					m_fSizeY -= fTimeDelta * 40.f;
				}
			}
		}
		else
		{
			m_bChangeEnter = false;
			pNmuber->Set_Active(false);
			return;
		}
	}
		break;
	case Client::CIconUI::SHURIKEN:
	{
		if (CWeapon_Player::WEAPON_TYPE::KATANA == m_pPlayer->Get_CurWeaponType())
		{
			m_bChangeEnter = false;
			return;
		}	
		else if (CWeapon_Player::WEAPON_TYPE::SHURIKEN == m_pPlayer->Get_CurWeaponType())
		{
			if (!m_bChangeEnter && m_vTargetSize.x > m_fSizeX &&
				m_vTargetSize.y > m_fSizeY)
			{
				m_fSizeX += fTimeDelta * 80.f;
				m_fSizeY += fTimeDelta * 80.f;
			}
			else
			{
				m_bChangeEnter = true;

				if (m_vOriginSize.x > m_fSizeX &&
					m_vOriginSize.y > m_fSizeY)
				{
					m_fSizeX = m_vOriginSize.x;
					m_fSizeY = m_vOriginSize.y;

				}
				else
				{
					m_fSizeX -= fTimeDelta * 40.f;
					m_fSizeY -= fTimeDelta * 40.f;
				}
			}
		}
	}
		break;
	case Client::CIconUI::DASH:
	{
		CNumberUI* pNmuber = static_cast<CNumberUI*>(m_childUI_List.back());

		if (false == m_pPlayer->IsDashActive())
		{
			if (!m_bChangeEnter && m_vTargetSize.x > m_fSizeX &&
				m_vTargetSize.y > m_fSizeY)
			{
				m_fSizeX += fTimeDelta * 80.f;
				m_fSizeY += fTimeDelta * 80.f;
			}
			else
			{
				m_pPlayer->Set_StartCountDashTime(true);
				m_bChangeEnter = true;
				
				_float fDashCoolTime = m_pPlayer->Get_DashCoolTime();
				_float fRemainingTime = m_pPlayer->Get_DashRemainingTime();
				pNmuber->Set_Active(true);

				_float fResult = (fDashCoolTime - fRemainingTime);

				if (fResult >= 2.0f)
				{
					fResult = 3.f;
				}
				else if (fResult >= 1.0f)
				{
					fResult = 2.f;
				}
				else if (fResult >= 0.0f)
				{
					fResult = 1.f;
				}
				pNmuber->Set_Number((_int)fResult);
				

				if (m_vOriginSize.x > m_fSizeX &&
					m_vOriginSize.y > m_fSizeY)
				{
					m_fSizeX = m_vOriginSize.x;
					m_fSizeY = m_vOriginSize.y;
				}
				else
				{
					m_fSizeX -= fTimeDelta * 40.f;
					m_fSizeY -= fTimeDelta * 40.f;
				}
			}
		}
		else
		{
			m_bChangeEnter = false;		
			pNmuber->Set_Active(false);
			return;
		}
	}
		break;
	case Client::CIconUI::CUTALL:
	{
		CNumberUI* pNmuber = static_cast<CNumberUI*>(m_childUI_List.back());

		if (false == m_pPlayer->IsCutAllActive())
		{
			if (!m_bChangeEnter && m_vTargetSize.x > m_fSizeX &&
				m_vTargetSize.y > m_fSizeY)
			{
				m_fSizeX += fTimeDelta * 80.f;
				m_fSizeY += fTimeDelta * 80.f;
			}
			else
			{
				m_pPlayer->Set_StartCountCutAllTime(true);
				m_bChangeEnter = true;

				_float fCutAllCoolTime = m_pPlayer->Get_CutAllCoolTime();
				_float fRemainingTime = m_pPlayer->Get_CutAllRemainingTime();
				pNmuber->Set_Active(true);

				_float fResult = (fCutAllCoolTime - fRemainingTime);

				if (fResult >= 2.0f)
				{
					fResult = 3.f;
				}
				else if (fResult >= 1.0f)
				{
					fResult = 2.f;
				}
				else if (fResult >= 0.0f)
				{
					fResult = 1.f;
				}
				pNmuber->Set_Number((_int)fResult);


				if (m_vOriginSize.x > m_fSizeX &&
					m_vOriginSize.y > m_fSizeY)
				{
					m_fSizeX = m_vOriginSize.x;
					m_fSizeY = m_vOriginSize.y;
				}
				else
				{
					m_fSizeX -= fTimeDelta * 40.f;
					m_fSizeY -= fTimeDelta * 40.f;
				}
			}
		}
		else
		{
			m_bChangeEnter = false;
			pNmuber->Set_Active(false);
			return;
		}
	}
	break;
	case Client::CIconUI::TIMESTOP:
	{
		CNumberUI* pNmuber = static_cast<CNumberUI*>(m_childUI_List.back());

		if (false == m_pPlayer->IsTimeStopActive())
		{
			if (!m_bChangeEnter && m_vTargetSize.x > m_fSizeX &&
				m_vTargetSize.y > m_fSizeY)
			{
				m_fSizeX += fTimeDelta * 80.f;
				m_fSizeY += fTimeDelta * 80.f;
			}
			else
			{
				m_pPlayer->Set_StartCountTimeStopTime(true);
				m_bChangeEnter = true;

				_float fTimeStopCoolTime = m_pPlayer->Get_TimeStopCoolTime();
				_float fRemainingTime = m_pPlayer->Get_TimeStopRemainingTime();
				pNmuber->Set_Active(true);

				_float fResult = (fTimeStopCoolTime - fRemainingTime);


				if (fResult >= 6.0f)
				{
					fResult = 7.f;
				}

				else if (fResult >= 5.0f)
				{
					fResult = 6.f;
				}

				else if (fResult >= 4.0f)
				{
					fResult = 5.f;
				}

				else if (fResult >= 3.0f)
				{
					fResult = 4.f;
				}

				else if (fResult >= 2.0f)
				{
					fResult = 3.f;
				}
				else if (fResult >= 1.0f)
				{
					fResult = 2.f;
				}
				else if (fResult >= 0.0f)
				{
					fResult = 1.f;
				}
				pNmuber->Set_Number((_int)fResult);


				if (m_vOriginSize.x > m_fSizeX &&
					m_vOriginSize.y > m_fSizeY)
				{
					m_fSizeX = m_vOriginSize.x;
					m_fSizeY = m_vOriginSize.y;
				}
				else
				{
					m_fSizeX -= fTimeDelta * 40.f;
					m_fSizeY -= fTimeDelta * 40.f;
				}
			}
		}
		else
		{
			m_bChangeEnter = false;
			pNmuber->Set_Active(false);
			return;
		}
	}
	break;
	case Client::CIconUI::NAMI:
	{
		CNumberUI* pNmuber = static_cast<CNumberUI*>(m_childUI_List.back());

		if (false == m_pPlayer->IsNamiActive())
		{
			if (!m_bChangeEnter && m_vTargetSize.x > m_fSizeX &&
				m_vTargetSize.y > m_fSizeY)
			{
				m_fSizeX += fTimeDelta * 80.f;
				m_fSizeY += fTimeDelta * 80.f;
			}
			else
			{
				m_pPlayer->Set_StartCountNamiTime(true);
				m_bChangeEnter = true;

				_float fCoolTime = m_pPlayer->Get_NamiCoolTime();
				_float fRemainingTime = m_pPlayer->Get_NamiRemainingTime();
				pNmuber->Set_Active(true);

				_float fResult = (fCoolTime - fRemainingTime);


				if (fResult >= 6.0f)
				{
					fResult = 7.f;
				}

				else if (fResult >= 5.0f)
				{
					fResult = 6.f;
				}

				else if (fResult >= 4.0f)
				{
					fResult = 5.f;
				}

				else if (fResult >= 3.0f)
				{
					fResult = 4.f;
				}

				else if (fResult >= 2.0f)
				{
					fResult = 3.f;
				}
				else if (fResult >= 1.0f)
				{
					fResult = 2.f;
				}
				else if (fResult >= 0.0f)
				{
					fResult = 1.f;
				}
				pNmuber->Set_Number((_int)fResult);


				if (m_vOriginSize.x > m_fSizeX &&
					m_vOriginSize.y > m_fSizeY)
				{
					m_fSizeX = m_vOriginSize.x;
					m_fSizeY = m_vOriginSize.y;
				}
				else
				{
					m_fSizeX -= fTimeDelta * 40.f;
					m_fSizeY -= fTimeDelta * 40.f;
				}
			}
		}
		else
		{
			m_bChangeEnter = false;
			pNmuber->Set_Active(false);
			return;
		}
	}
	break;
	case Client::CIconUI::MINDCONTROL:
	{
		CNumberUI* pNmuber = static_cast<CNumberUI*>(m_childUI_List.back());

		if (false == m_pPlayer->IsMindControlActive())
		{
			if (!m_bChangeEnter && m_vTargetSize.x > m_fSizeX &&
				m_vTargetSize.y > m_fSizeY)
			{
				m_fSizeX += fTimeDelta * 80.f;
				m_fSizeY += fTimeDelta * 80.f;
			}
			else
			{
				m_pPlayer->Set_StartCountMindControlTime(true);
				m_bChangeEnter = true;

				_float fCoolTime = m_pPlayer->Get_MindControlCoolTime();
				_float fRemainingTime = m_pPlayer->Get_MindControlRemainingTime();
				pNmuber->Set_Active(true);

				_float fResult = (fCoolTime - fRemainingTime);


				if (fResult >= 6.0f)
				{
					fResult = 7.f;
				}

				else if (fResult >= 5.0f)
				{
					fResult = 6.f;
				}

				else if (fResult >= 4.0f)
				{
					fResult = 5.f;
				}

				else if (fResult >= 3.0f)
				{
					fResult = 4.f;
				}

				else if (fResult >= 2.0f)
				{
					fResult = 3.f;
				}
				else if (fResult >= 1.0f)
				{
					fResult = 2.f;
				}
				else if (fResult >= 0.0f)
				{
					fResult = 1.f;
				}
				pNmuber->Set_Number((_int)fResult);


				if (m_vOriginSize.x > m_fSizeX &&
					m_vOriginSize.y > m_fSizeY)
				{
					m_fSizeX = m_vOriginSize.x;
					m_fSizeY = m_vOriginSize.y;
				}
				else
				{
					m_fSizeX -= fTimeDelta * 40.f;
					m_fSizeY -= fTimeDelta * 40.f;
				}
			}
		}
		else
		{
			m_bChangeEnter = false;
			pNmuber->Set_Active(false);
			return;
		}
	}
	break;
	case Client::CIconUI::HOMING_SH:
	{
		CNumberUI* pNmuber = static_cast<CNumberUI*>(m_childUI_List.back());

		if (false == m_pPlayer->IsHomingShActive())
		{
			if (!m_bChangeEnter && m_vTargetSize.x > m_fSizeX &&
				m_vTargetSize.y > m_fSizeY)
			{
				m_fSizeX += fTimeDelta * 80.f;
				m_fSizeY += fTimeDelta * 80.f;
			}
			else
			{
				m_pPlayer->Set_StartCountHomingShTime(true);
				m_bChangeEnter = true;

				_float fCoolTime = m_pPlayer->Get_HomingShCoolTime();
				_float fRemainingTime = m_pPlayer->Get_HomingShRemainingTime();
				pNmuber->Set_Active(true);

				_float fResult = (fCoolTime - fRemainingTime);


				if (fResult >= 6.0f)
				{
					fResult = 7.f;
				}

				else if (fResult >= 5.0f)
				{
					fResult = 6.f;
				}

				else if (fResult >= 4.0f)
				{
					fResult = 5.f;
				}

				else if (fResult >= 3.0f)
				{
					fResult = 4.f;
				}

				else if (fResult >= 2.0f)
				{
					fResult = 3.f;
				}
				else if (fResult >= 1.0f)
				{
					fResult = 2.f;
				}
				else if (fResult >= 0.0f)
				{
					fResult = 1.f;
				}
				pNmuber->Set_Number((_int)fResult);


				if (m_vOriginSize.x > m_fSizeX &&
					m_vOriginSize.y > m_fSizeY)
				{
					m_fSizeX = m_vOriginSize.x;
					m_fSizeY = m_vOriginSize.y;
				}
				else
				{
					m_fSizeX -= fTimeDelta * 40.f;
					m_fSizeY -= fTimeDelta * 40.f;
				}
			}
		}
		else
		{
			m_bChangeEnter = false;
			pNmuber->Set_Active(false);
			return;
		}
	}
	break;
	case Client::CIconUI::ICON_TYPE_END:
		break;
	default:
		break;
	}
	

	
	for (auto& pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CIconUI::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CIconUI::Render()
{
	if (false == m_bActivate)
		return S_OK;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bClicked", &m_bChangeEnter, sizeof(_bool))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_ClickedColor", &m_clickedColor, sizeof(_float4))))
		return E_FAIL;
	


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}

HRESULT CIconUI::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_IconUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	switch (m_eType)
	{
	case Client::CIconUI::BLOCK:
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Block"),
			TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;

	case Client::CIconUI::SHURIKEN:
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Shuriken"),
			TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;

	case Client::CIconUI::DASH:
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Dash"),
			TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;

	case Client::CIconUI::MINDCONTROL:
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_MindControl"),
			TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;

	case Client::CIconUI::NAMI:
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Nami"),
			TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;

	case Client::CIconUI::CUTALL:
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_CutAll"),
			TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;

	case Client::CIconUI::TIMESTOP:
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_TimeStop"),
			TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
		break;
	case Client::CIconUI::HOMING_SH:
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Icon_Sh_Skill"),
			TEXT("Com_Texture_Frame"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	break;

	case Client::CIconUI::ICON_TYPE_END:
		break;
	default:
		break;
	}
	



	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}




void CIconUI::Go_InSide(_float fTimeDelta)
{
	if (CIconUI::DASH == m_eType ||
		CIconUI::SHURIKEN == m_eType)
		return;


	_float fSpeed = { 5.f };

	switch (m_eType)
	{
	case Client::CIconUI::BLOCK:
		break;
	case Client::CIconUI::CUTALL:
		if (false == *m_NextIconArrive[BLOCK])
			return;
		break;
	case Client::CIconUI::NAMI:
		if (false == *m_NextIconArrive[CUTALL])
			return;
		break;
	case Client::CIconUI::TIMESTOP:
		if (false == *m_NextIconArrive[NAMI])
			return;
		break;
	case Client::CIconUI::MINDCONTROL:
		if (false == *m_NextIconArrive[TIMESTOP])
			return;
		break;
	case Client::CIconUI::HOMING_SH:
		if (true == *m_NextIconArrive[MINDCONTROL])
		{
			m_bActivate = true;

			if (m_vTargetPos.x <= m_fX)	// 목표를 넘었음을 기록
			{
				m_pPlayer->Set_CanSwapWeapon(true);
			}
			
			m_fAccTime += fTimeDelta * 30.f;
			m_fX += m_fAccTime * ((m_vTargetPos.x - m_fX) / m_vTargetPos.x);
			return;
		}
		break;
	case Client::CIconUI::ICON_TYPE_END:
		break;
	default:
		break;
	}
	


	if (m_vInSidePos.x < m_fX)
	{
		if (60 >= fabs(m_fX - m_vInSidePos.x))
			m_isArrive = true;

		m_fAccTime += fTimeDelta * 30.f;
		m_fX -= m_fAccTime;
	}
	else
	{
		m_fX = m_vInSidePos.x;		
	}		
}


void CIconUI::Go_Out(_float fTimeDelta)
{
	switch (m_eType)
	{
	case Client::CIconUI::BLOCK:
	{
		if (false == *m_NextIconArrive[CUTALL])
			return;

		if(true == m_isArrive)
			m_pPlayer->Set_CanSwapWeapon(true);	
	}
		
		break;
	case Client::CIconUI::CUTALL:
		if (false == *m_NextIconArrive[NAMI])
			return;
		break;
	case Client::CIconUI::NAMI:
		if (false == *m_NextIconArrive[TIMESTOP])
			return;
		break;
	case Client::CIconUI::TIMESTOP:
		if (false == *m_NextIconArrive[MINDCONTROL])
			return;
		break;
	case Client::CIconUI::MINDCONTROL:
		if (false == *m_NextIconArrive[HOMING_SH])
			return;
		break;
	case Client::CIconUI::HOMING_SH:
	{
		if (m_vInSidePos.x < m_fX)
		{
			if (60 >= fabs(m_fX - m_vInSidePos.x))
				m_isArrive = true;

			m_fAccTime += fTimeDelta * 30.f;
			m_fX -= m_fAccTime;
		}
		else
		{
			m_fX = m_vInSidePos.x;
			m_bActivate = false;
		}
		return;
	}
		break;
	case Client::CIconUI::SHURIKEN:
		return;
		break;
	case Client::CIconUI::DASH:
		return;
		break;
	case Client::CIconUI::ICON_TYPE_END:
		break;
	default:
		break;
	}


	if (m_fX == m_vTargetPos.x)
	{
		m_isArrive = true;
		return;
	}
	else if (false == m_isOverShoot)
	{ 
		if (60 >= fabs(m_fX - m_vTargetPos.x))
			m_isArrive = true;

		if (m_vTargetPos.x + m_fOverShoot <= m_fX)   // 목표를 넘었음을 기록
			m_isOverShoot = true; 

		m_fAccTime += fTimeDelta * 30.f;
		m_fX += m_fAccTime * ((m_vTargetPos.x - m_fX) / m_vTargetPos.x);
	}
	else
	{
		if (m_vTargetPos.x < m_fX)
		{
			m_fAccTime -= fTimeDelta * 10.f; // 반대로 감속
			m_fX -= m_fAccTime * ((m_vTargetPos.x - m_fX) / m_vTargetPos.x);

			// 목표에 도착했을 때
			if (m_fX <= m_vTargetPos.x)
			{
				m_fX = m_vTargetPos.x;
				m_isOverShoot = false; // 초기화
			}
		}
	}

	
}



CIconUI* CIconUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIconUI* pInstance = new CIconUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CIconUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CIconUI::Clone(void* pArg)
{
	CIconUI* pInstance = new CIconUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CIconUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIconUI::Free()
{
	// 레이어 에서 얘의 Free 들어왓고
	// 자식들이 자기 부모를 계속 지운단말이야

	for (auto& pUI : m_childUI_List)
	{
		Safe_Release(pUI);
	}
	m_childUI_List.clear();

	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pPlayer);
}
