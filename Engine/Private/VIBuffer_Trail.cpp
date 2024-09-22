#include "../Public/VIBuffer_Trail.h"

#include "GameInstance.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& Prototype)
	: CVIBuffer{ Prototype }
	, m_iNumPanel { Prototype.m_iNumPanel }
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumPanel = 15;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = 2 *  (m_iNumPanel + 1);							// m_iNumVertices = 2 * (N + 1);
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumIndices = 6 * m_iNumPanel;									// m_iNumIndices = 6 * N;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);


	// 각 구간은 2개의 정점을 사용한다

	for (_uint i = 0; i < m_iNumPanel; ++i) {
		_uint baseIndex = i * 2;

		// 첫 번째 삼각형
		pIndices[i * 6 + 0] = baseIndex;
		pIndices[i * 6 + 1] = baseIndex + 1;
		pIndices[i * 6 + 2] = baseIndex + 2;

		// 두 번째 삼각형
		pIndices[i * 6 + 3] = baseIndex + 1;
		pIndices[i * 6 + 4] = baseIndex + 3;
		pIndices[i * 6 + 5] = baseIndex + 2;
	}




	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion



	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void* pArg)
{


	
	return S_OK;
}



void CVIBuffer_Trail::Update_Trail(_float fTimeDelta , const _fvector& vPreStart, const _fvector& vPreEnd, const _fvector& vCurStart, const _fvector& vCurEnd)
{	
	// 상수 버퍼 데이터 업데이트
	
	// 한번의 드로우 콜로 그린다라고 이야기를 하고 
	// 버텍스 두점을 칼 양 끝으로 잡는다
	// 이후 0,1    /  2,3 이전위치와 다음위치로 그린다
	
	 // 버텍스 버퍼 업데이트


	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return;

	VTXNORTEX* pVertices = static_cast<VTXNORTEX*>(SubResource.pData);



	for (_uint i = 0; i < m_iNumPanel; i++)
	{
		_uint iIndex = i * 2; 
	
	
		XMStoreFloat3(&pVertices[iIndex].vPosition, vPreStart);					 // 이전 프레임의 시작점
		pVertices[iIndex].vTexcoord = _float2(0.0f, 0.0f);  
	
		XMStoreFloat3(&pVertices[iIndex +1].vPosition, vPreEnd);				 // 이전 프레임의 시작점
		pVertices[iIndex + 1].vTexcoord = _float2(1.0f, 0.0f);  
	
	
		XMStoreFloat3(&pVertices[iIndex + 2].vPosition, vCurStart);				 // 이전 프레임의 시작점
		pVertices[iIndex + 2].vTexcoord = _float2(0.0f, 1.0f); 
	
		XMStoreFloat3(&pVertices[iIndex + 3].vPosition, vCurEnd);				 // 이전 프레임의 시작점
		pVertices[iIndex + 3].vTexcoord = _float2(1.0f, 1.0f);  
	}

	
	

	m_pContext->Unmap(m_pVB, 0);
	
}

_float3 CVIBuffer_Trail::CatmullRom(const _fvector& vPreStart, const _fvector& vPreEnd, const _fvector& vCurStart, const _fvector& vCurEnd, float t)
{
	_float t2 = t * t;
	_float t3 = t2 * t;
	XMFLOAT3 result;



	_float3		PreStart;
	_float3		PreEnd;
	_float3		CurStart;
	_float3		CurEnd;


	XMStoreFloat3(&PreStart, vPreStart);
	XMStoreFloat3(&PreEnd, vPreEnd);
	XMStoreFloat3(&CurStart, vCurStart);
	XMStoreFloat3(&CurEnd, vCurEnd);

	result.x = 0.5f * ((2.0f * PreEnd.x) +
		(-PreStart.x + CurStart.x) * t +
		(2.0f * PreStart.x - 5.0f * PreEnd.x + 4.0f * CurStart.x - CurEnd.x) * t2 +
		(-PreStart.x + 3.0f * PreEnd.x - 3.0f * CurStart.x + CurEnd.x) * t3);

	result.y = 0.5f * ((2.0f * PreEnd.y) +
		(-PreStart.y + CurStart.y) * t +
		(2.0f * PreStart.y - 5.0f * PreEnd.y + 4.0f * CurStart.y - CurEnd.y) * t2 +
		(-PreStart.y + 3.0f * PreEnd.y - 3.0f * CurStart.y + CurEnd.y) * t3);

	result.z = 0.5f * ((2.0f * PreEnd.z) +
		(-PreStart.z + CurStart.z) * t +
		(2.0f * PreStart.z - 5.0f * PreEnd.z + 4.0f * CurStart.z - CurEnd.z) * t2 +
		(-PreStart.z + 3.0f * PreEnd.z - 3.0f * CurStart.z + CurEnd.z) * t3);

	return result;
}




CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();

	
}
