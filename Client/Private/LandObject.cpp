#include "stdafx.h"
#include "..\Public\LandObject.h"

#include "GameInstance.h"

CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice,pContext}
{
}

CLandObject::CLandObject(const CLandObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
	LANDOBJECT_DESC* pDesc = static_cast<LANDOBJECT_DESC*>(pArg);
	
	//if (nullptr != pDesc)
	//{
	//	m_pTerrainVIBuffer = pDesc->pTerrainVIBuffer;
	//	Safe_AddRef(m_pTerrainVIBuffer);
	//
	//	m_pTerrainTransform = pDesc->pTerrainTransform;
	//	Safe_AddRef(m_pTerrainTransform);
	//}

	__super::Initialize(pDesc);
	



	return S_OK;
}

void CLandObject::Priority_Update(_float fTimeDelta)
{
}

void CLandObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	if (m_strLayerName == L"Layer_PreView_Object")
	{
		SetUp_OnTerrain(m_pTransformCom, 0.5f);
	}

}

void CLandObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}

HRESULT CLandObject::SetUp_OnTerrain(CTransform* pTransform, _float fOffsetY)
{
	/* 지형을 타고하는 객체의 월드 위치를 얻어온다. */
	//_vector		vWorldPos = pTransform->Get_State(CTransform::STATE_POSITION);
	//
	///* 지형 정점과 비교 시, 로컬스페이스에서 비교할 요량이긷매ㅜㄴ에 객체의 위치를 지형버퍼의 로컬로 변환시키자. */
	///* 객체의 월드위치 * 지형모델을 그리는 객체의 월드 역행렬 */
	//_matrix		WorldMatrixInverse = m_pTerrainTransform->Get_WorldMatrix_Inverse();
	//_matrix     WorldMatrix = m_pTerrainTransform->Get_WorldMatrix();
	//
	//
	//_vector		vLocalPos{};
	//vLocalPos = XMVector3TransformCoord(vWorldPos, WorldMatrixInverse);
	//
	//
	//
	//
	//
	//_float		fHeight = m_pTerrainVIBuffer->Compute_Height(vLocalPos);
	//
	//_float ff = fHeight + (fOffsetY * pTransform->Get_Scaled().y);
	//
	//vLocalPos = XMVectorSetY(vLocalPos, fHeight + (fOffsetY * pTransform->Get_Scaled().y));// << 내 스케일에 맞게 y 위치조정
   	//
	///* 지형의 로컬 스페이스 상에 갱신된 로컬위치를 구한것이기때문에 */
	///* 이를 다시 월드로 보내기(지형로컬의 데이터를 -> 월드 )위해서는 월드행렬을 곱해야한다. */
	//
	//vWorldPos = XMVector3TransformCoord(vLocalPos , WorldMatrix);
	//
	//pTransform->Set_State(CTransform::STATE_POSITION, vWorldPos);

	return S_OK;
}



void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pTerrainVIBuffer);
	Safe_Release(m_pTerrainTransform);
}
