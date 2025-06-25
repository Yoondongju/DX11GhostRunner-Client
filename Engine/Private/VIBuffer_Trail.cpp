#include "../Public/VIBuffer_Trail.h"

#include "GameInstance.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& Prototype)
	: CVIBuffer{ Prototype }
	, m_iNumSegment{ Prototype.m_iNumSegment }
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype(_uint iNumSegment)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumSegment = iNumSegment;					// 최대 세그먼트 갯수

	m_iNumVertexBuffers = 1;
	m_iNumVertices = 2 *  (m_iNumSegment + 1);							// m_iNumVertices = 2 * (N + 1);
	m_iVertexStride = sizeof(VTXTRAILTEX);
	m_iNumIndices = 6 * m_iNumSegment;									// m_iNumIndices = 6 * N;
	m_iIndexStride = sizeof(_ushort);
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXTRAILTEX* pVertices = new VTXTRAILTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTRAILTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		pVertices[i].vLifeTime.x = m_pGameInstance->Get_Random(0.05,0.05f);
		pVertices[i].vLifeTime.y = 0.f;
	}


	

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
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);




	for (_uint i = 0; i < m_iNumSegment; ++i) {
		_uint baseIndex = i * 2;
		pIndices[i * 6 + 0] = baseIndex;
		pIndices[i * 6 + 1] = baseIndex + 1;
		pIndices[i * 6 + 2] = baseIndex + 2;

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



void CVIBuffer_Trail::Update_SwordTrail(_float fTimeDelta, deque<TRAIL_INFO>& TrailInfo)
{	
	for (auto it = TrailInfo.begin(); it != TrailInfo.end();)
	{
		it->fLifeTime -= fTimeDelta; 
		if (it->fLifeTime <= 0.0f) 
		{
			it = TrailInfo.erase(it); 
		}
		else
		{
			++it; 
		}
	}
	
	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
		return;

	VTXTRAILTEX* pVertices = static_cast<VTXTRAILTEX*>(SubResource.pData);
	m_iNumIndices = 6 * (TrailInfo.size() - 1);			

	if (TrailInfo.size() < 2)
	{
		ZeroMemory(pVertices, sizeof(VTXTRAILTEX) * m_iNumVertices);

		_float2 fOrigineLifeTime = { 0.5f , 0.f};
		for (_uint i = 0; i < m_iNumVertices; i++)
		{
			memcpy(&(pVertices[i].vLifeTime), &fOrigineLifeTime, sizeof(_float2));
		}
		m_pContext->Unmap(m_pVB, 0);
		return;
	}
	
	for (_uint i = 0; i < TrailInfo.size(); i++)		
	{
		_uint iIndex = i * 4; 
		
		_float3 CurStart = TrailInfo[i].CurStart;
		_float3 CurEnd = TrailInfo[i].CurEnd;

		pVertices[iIndex].vPosition = CurStart;
		pVertices[iIndex].vTexcoord = _float2(0.0f, 0.0f);
		pVertices[iIndex].vLifeTime.y += fTimeDelta;

		pVertices[iIndex + 1].vPosition = CurEnd;
		pVertices[iIndex + 1].vTexcoord = _float2(1.0f, 0.0f);
		pVertices[iIndex + 1].vLifeTime.y += fTimeDelta;

		if (i < TrailInfo.size() - 1)
		{
			_float3 NextStart = TrailInfo[i + 1].CurStart;
			_float3 NextEnd = TrailInfo[i + 1].CurEnd;

			pVertices[iIndex + 2].vPosition = NextStart;
			pVertices[iIndex + 2].vTexcoord = _float2(0.0f, 1.0f);
			pVertices[iIndex + 2].vLifeTime.y += fTimeDelta;

			pVertices[iIndex + 3].vPosition = NextEnd;
			pVertices[iIndex + 3].vTexcoord = _float2(1.0f, 1.0f);
			pVertices[iIndex + 3].vLifeTime.y += fTimeDelta;
		}
		else
		{
			pVertices[iIndex + 2].vPosition = CurStart;
			pVertices[iIndex + 2].vTexcoord = _float2(0.0f, 1.0f);
			pVertices[iIndex + 2].vLifeTime.y += fTimeDelta;

			pVertices[iIndex + 3].vPosition = CurEnd;
			pVertices[iIndex + 3].vTexcoord = _float2(1.0f, 1.0f);
			pVertices[iIndex + 3].vLifeTime.y += fTimeDelta;
		}
	}
	m_pContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Trail::Update_ShurikenTrail(_float fTimeDelta, deque<TRAIL_INFO>& TrailInfo)
{
	for (auto it = TrailInfo.begin(); it != TrailInfo.end();)
	{
		it->fLifeTime -= fTimeDelta; // 생명 주기 감소
		if (it->fLifeTime <= 0.0f) // 만료된 세그먼트 확인
		{
			it = TrailInfo.erase(it); // 세그먼트 삭제
		}
		else
		{
			++it; // 다음 세그먼트로 이동
		}
	}


	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
		return;
	// D3D11_MAP_WRITE_DISCARD
	// D3D11_MAP_WRITE_NO_OVERWRITE
	VTXTRAILTEX* pVertices = static_cast<VTXTRAILTEX*>(SubResource.pData);



	m_iNumIndices = 6 * (TrailInfo.size() - 1);			// 인덱스버퍼 참조하는 인덱스 갯수가 달라져야해 

	if (TrailInfo.size() < 2)
	{
		ZeroMemory(pVertices, sizeof(VTXTRAILTEX) * m_iNumVertices);

		_float2 fOrigineLifeTime = { 0.5f , 0.f };
		for (_uint i = 0; i < m_iNumVertices; i++)
		{
			memcpy(&(pVertices[i].vLifeTime), &fOrigineLifeTime, sizeof(_float2));
		}
		m_pContext->Unmap(m_pVB, 0);
		return;
	}


	for (_uint i = 0; i < TrailInfo.size(); i++)		// m_iNumPanel == TrailInfo.size()
	{
		_uint iIndex = i * 4;

		_float3 CurStart = TrailInfo[i].CurStart;
		_float3 CurEnd = TrailInfo[i].CurEnd;

		pVertices[iIndex].vPosition = CurStart;
		pVertices[iIndex].vTexcoord = _float2(0.0f, 0.0f);
		pVertices[iIndex].vLifeTime.y += fTimeDelta;

		pVertices[iIndex + 1].vPosition = CurEnd;
		pVertices[iIndex + 1].vTexcoord = _float2(1.0f, 0.0f);
		pVertices[iIndex + 1].vLifeTime.y += fTimeDelta;


		// 다음 세그먼트 존재 확인
		if (i < TrailInfo.size() - 1)
		{
			_float3 NextStart = TrailInfo[i + 1].CurStart;
			_float3 NextEnd = TrailInfo[i + 1].CurEnd;

			pVertices[iIndex + 2].vPosition = NextStart;
			pVertices[iIndex + 2].vTexcoord = _float2(0.0f, 1.0f);
			pVertices[iIndex + 2].vLifeTime.y += fTimeDelta;

			pVertices[iIndex + 3].vPosition = NextEnd;
			pVertices[iIndex + 3].vTexcoord = _float2(1.0f, 1.0f);
			pVertices[iIndex + 3].vLifeTime.y += fTimeDelta;
		}
		else
		{
			// 마지막 세그먼트일 때: 마지막 포인트들을 그대로 유지
			pVertices[iIndex + 2].vPosition = CurStart;
			pVertices[iIndex + 2].vTexcoord = _float2(0.0f, 1.0f);
			pVertices[iIndex + 2].vLifeTime.y += fTimeDelta;

			pVertices[iIndex + 3].vPosition = CurEnd;
			pVertices[iIndex + 3].vTexcoord = _float2(1.0f, 1.0f);
			pVertices[iIndex + 3].vLifeTime.y += fTimeDelta;
		}
	}
	m_pContext->Unmap(m_pVB, 0);

}








CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumSegment)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumSegment)))
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
