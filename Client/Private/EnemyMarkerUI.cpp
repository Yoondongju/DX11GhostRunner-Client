#include "stdafx.h"
#include "../Public/EnemyMarkerUI.h"
#include "GameInstance.h"

#include "Player.h"
#include "Weapon_Player.h"

#include "NumberUI.h"
#include "Enemy.h"

CEnemyMarker::CEnemyMarker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CEnemyMarker::CEnemyMarker(const CEnemyMarker& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CEnemyMarker::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEnemyMarker::Initialize(void* pArg)
{
	ENEMYMARKER_DESC* pDesc = static_cast<ENEMYMARKER_DESC*>(pArg);
	m_pOwner = pDesc->pOwner;
	Safe_AddRef(m_pOwner);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;


	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_vOriginSize.x = m_fSizeX;
	m_vOriginSize.y = m_fSizeY;


	m_vTargetSize.x = m_fSizeX + 70.f;
	m_vTargetSize.y = m_fSizeY + 70.f;


	m_bActivate = false;

	return S_OK;
}

void CEnemyMarker::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CEnemyMarker::Update(_float fTimeDelta)
{
	// 내 시야각을 먼저 체크하고, 몬스터 갯수체크해서, 스킬을 사용못한다라고 폰트 띄우는걸 먼저 ㄱ  //  나미 , 컷올, 마인드컨트롤 썼을때 일단 가정하고

	m_bActivate = m_pOwner->IsTargetting();
	if (false == m_bActivate)
	{
		_float	fRotationSpeed = 360.f;
		if(m_fAccRotationSpeed != 0.f && m_fAccRotationSpeed != fRotationSpeed)		// 할당량 채워지지않았다면
		{
			_float fRemainingRotation = fRotationSpeed - m_fAccRotationSpeed;

			_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fRemainingRotation));
			m_pTransformCom->Turn(RotationMatrix, nullptr);

			m_fAccRotationSpeed = fRotationSpeed;
		}


		m_fAccRotationSpeed = 0.f;
		m_bChangeEnter = false;

		m_fSizeX = m_vOriginSize.x;
		m_fSizeY = m_vOriginSize.y;

		return;
	}
		

	if (true == m_pOwner->IsDead())
	{
		m_pGameInstance->Delete(LEVEL_GAMEPLAY, CRenderer::RG_UI, this);
		return;
	}




	CTransform* pPlayerTransform = m_pGameInstance->Find_Player(LEVEL_GAMEPLAY)->Get_Transform();

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

	_vector vOwnerPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	
	_vector vLookNor = XMVector3Normalize(vPlayerLook);
	_vector vDirNor = XMVector3Normalize(vOwnerPos - vPlayerPos);


	_float fDot = XMVectorGetX(XMVector3Dot(vLookNor, vDirNor));
	_float fAngle = XMConvertToDegrees(acos(fDot));


	if (fAngle > 45.f)
	{
		m_bActivate = false;
		return;
	}


	

	vOwnerPos = XMVectorSetY(vOwnerPos, vOwnerPos.m128_f32[1] + 30.f);

	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

	_vector NDCPos = XMVector3TransformCoord(XMVector3TransformCoord(vOwnerPos, ViewMatrix), ProjMatrix);

	_float fScreenX = ((XMVectorGetX(NDCPos) + 1.0f) / 2.0f) * m_fViewWidth;
	_float fScreenY = ((1.0f - XMVectorGetY(NDCPos)) / 2.0f) * m_fViewHeight;


	m_fX = fScreenX;
	m_fY = fScreenY;


	// 한바퀴 360도 회전한다

	_float	fRotationSpeed = 360.f;
	_float	fCurRotationSpeed = fRotationSpeed * 0.03f;

	
	if (m_fAccRotationSpeed + fCurRotationSpeed <= fRotationSpeed)
	{
		_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fCurRotationSpeed));
		m_pTransformCom->Turn(RotationMatrix, nullptr);

		m_fAccRotationSpeed += fCurRotationSpeed;
	}
	else
	{
		_float fRemainingRotation = fRotationSpeed - m_fAccRotationSpeed;

		_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fRemainingRotation));
		m_pTransformCom->Turn(RotationMatrix, nullptr);

		m_fAccRotationSpeed = fRotationSpeed;
	}




	if(true == m_pOwner->IsFinalTargetting())
	{
		m_bChangeEnter = true;

		if (m_vTargetSize.x > m_fSizeX &&
			m_vTargetSize.y > m_fSizeY)
		{
			m_fSizeX += fTimeDelta * 100.f;
			m_fSizeY += fTimeDelta * 100.f;
		}
		else
		{
			if (m_vOriginSize.x > m_fSizeX &&
				m_vOriginSize.y > m_fSizeY)
			{
				m_fSizeX = m_vOriginSize.x;
				m_fSizeY = m_vOriginSize.y;
			}
			else
			{
				m_fSizeX -= fTimeDelta * 70.f;
				m_fSizeY -= fTimeDelta * 70.f;
			}
		}
	}
	

	




	for (auto& pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CEnemyMarker::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CEnemyMarker::Render()
{
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




HRESULT CEnemyMarker::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_IconUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_EnemyMarker"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	


	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}



CEnemyMarker* CEnemyMarker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemyMarker* pInstance = new CEnemyMarker(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEnemyMarker"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CEnemyMarker::Clone(void* pArg)
{
	CEnemyMarker* pInstance = new CEnemyMarker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEnemyMarker"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemyMarker::Free()
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

	Safe_Release(m_pOwner);
}
