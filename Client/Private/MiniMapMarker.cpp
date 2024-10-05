#include "stdafx.h"
#include "../Public/MiniMapMarker.h"
#include "GameInstance.h"

#include "Enemy.h"

#include "Player.h"


CMiniMapMarker::CMiniMapMarker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CMiniMapMarker::CMiniMapMarker(const CMiniMapMarker& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CMiniMapMarker::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CMiniMapMarker::Initialize(void* pArg)
{
	MINIMAPMARKER_DESC* pDesc = static_cast<MINIMAPMARKER_DESC*>(pArg);
	m_pOwner = pDesc->pOwner;
	Safe_AddRef(m_pOwner);


	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;





	return S_OK;
}

void CMiniMapMarker::Priority_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CMiniMapMarker::Update(_float fTimeDelta)
{
	if (nullptr == m_pOwner)
		return;

	if (true == m_pOwner->IsDead())		// 일단 이렇게 처리하고 나중에 panel지울때 삭 지우자
	{	
		m_bActivate = false;
		m_pOwner = nullptr;

		return;
	}


	_vector vOwnerPos = m_pOwner->Get_Transform()->Get_State(CTransform::STATE_POSITION);


	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	const _float4x4 PlayerRotationMatrix = pPlayer->Get_RotationMatrix();


	
	vOwnerPos = XMVectorSetY(vOwnerPos, 0.f);
	vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);

	_float fDistance = XMVectorGetX(XMVector3Length(vOwnerPos - vPlayerPos));

	if (fDistance > 300.f)
		m_bActivate = false;
	else
		m_bActivate = true;


	if (false == m_bActivate)
		return;


	_float fX = m_pParentUI->Get_fX();
	_float fY = m_pParentUI->Get_fY();
	_float fSizeX = m_pParentUI->Get_fSizeX();
	_float fSizeY = m_pParentUI->Get_fSizeY();



	

	_float fRatioX = 300.f / g_iWinSizeX;
	_float fRatioY = 300.f / g_iWinSizeY;



	_vector vRelativePos = vOwnerPos - vPlayerPos;

	// 회전 행렬을 적용하여 플레이어의 시점에 맞춘 상대 위치 계산
	_matrix RotationMatrix =  XMLoadFloat4x4(&PlayerRotationMatrix);
	_matrix InvertedRotationMatrix = XMMatrixInverse(nullptr, RotationMatrix);			// 플레이어 회전행렬의 역행렬  

	vRelativePos = XMVector3TransformCoord(vRelativePos, InvertedRotationMatrix);


	// 미니맵 좌표 계산




	m_fX = fX + XMVectorGetX(vRelativePos) * fRatioX;
	m_fY = fY - XMVectorGetZ(vRelativePos) * fRatioY;



	for (auto& pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CMiniMapMarker::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;


	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	//m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CMiniMapMarker::Render()
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



	_bool bCheck = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bClicked", &bCheck, sizeof(_bool))))
		return E_FAIL;




	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}




HRESULT CMiniMapMarker::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_IconUI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MiniMapMarker"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}



CMiniMapMarker* CMiniMapMarker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMiniMapMarker* pInstance = new CMiniMapMarker(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMiniMapMarker"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CMiniMapMarker::Clone(void* pArg)
{
	CMiniMapMarker* pInstance = new CMiniMapMarker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMiniMapMarker"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniMapMarker::Free()
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
