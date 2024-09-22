#include "..\Public\VIBuffer_Wire.h"

#include "GameInstance.h"

CVIBuffer_Wire::CVIBuffer_Wire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Wire::CVIBuffer_Wire(const CVIBuffer_Wire& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_Wire::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	_uint numSides = 24;					// 단면의 정점 수 (24각형 원기둥)
	_uint numVertices = numSides * 2;		// 두 단면의 정점 수
	m_iNumVertices = numVertices;
	m_iVertexStride = sizeof(VTXPOSTEX);

	_uint numTriangles = numSides * 2; // 위아래 두 면의 삼각형 수
	_uint numIndices = numTriangles * 3; // 각 삼각형당 3개의 인덱스

	m_iNumIndices = numIndices;
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
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	_float angleStep = XM_2PI / numSides;
	for (_uint i = 0; i < numSides; ++i) {
		_float angle = i * angleStep;
		_float x = cosf(angle) * 0.5f; // 단면 반지름은 0.5
		_float z = sinf(angle) * 0.5f;

		// 아래 단면의 정점
		pVertices[i].vPosition = _float3(x, -0.5f, z); // y 좌표 -0.5는 아래쪽
		pVertices[i].vTexcoord = _float2(_float(i) / numSides, 0.0f);

		// 위쪽 단면의 정점
		pVertices[i + numSides].vPosition = _float3(x, 0.5f, z); // y 좌표 0.5는 위쪽
		pVertices[i + numSides].vTexcoord = _float2(_float(i) / numSides, 1.0f);
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
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_int idx = 0;
	for (_int i = 0; i < numSides; ++i) {
		_int next = (i + 1) % numSides;

		// 아래 단면과 위쪽 단면을 연결하는 두 개의 삼각형
		pIndices[idx++] = i;
		pIndices[idx++] = next;
		pIndices[idx++] = i + numSides;

		pIndices[idx++] = next;
		pIndices[idx++] = next + numSides;
		pIndices[idx++] = i + numSides;
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

HRESULT CVIBuffer_Wire::Initialize(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Wire::Update_VertexData(const _fvector& vStart, const _fvector& vEnd)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	VTXPOSTEX* pVertices = (VTXPOSTEX*)MappedResource.pData;

	_vector vLookNor = XMVector3Normalize(vEnd - vStart);
	_vector vRightNor = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLookNor);
	_vector vUpNor = XMVector3Normalize(XMVector3Cross(vLookNor, vRightNor));
	

	_float radius = 0.5f;  // 원기둥의 반지름
	_uint numSides = m_iNumVertices / 2; // 정점 개수는 두 단면의 정점 수

	_float length = XMVectorGetX(XMVector3Length(vEnd - vStart)); // 원기둥의 길이
	_float frequency = 10.0f; // 진동의 주파수 (값을 낮추면 진동이 더 뚜렷해질 수 있습니다)
	_float amplitude = 0.3f; // 진동의 진폭 (값을 낮추면 효과가 덜 뚜렷해집니다)

	for (_uint i = 0; i < numSides; ++i) {
		_float angle = (XM_2PI / numSides) * i;
		_float x = cosf(angle) * radius;
		_float z = sinf(angle) * radius;

		_vector vOffset = vRightNor * x + vUpNor * z;

		// 아래 단면의 정점
		_vector vBasePosStart = XMVectorAdd(vStart, vOffset);
		_vector vBasePosEnd = XMVectorAdd(vEnd, vOffset);

		// S자 형태로 꿀렁이는 효과 추가
		_float progress = (static_cast<_float>(i) / numSides); // 0.0f to 1.0f
		_float waveOffset = amplitude * sinf(frequency * progress * XM_2PI); // S자 진동 생성

		vBasePosStart = XMVectorAdd(vBasePosStart, vUpNor * waveOffset);
		vBasePosEnd = XMVectorAdd(vBasePosEnd, vUpNor * waveOffset);

		XMStoreFloat3(&pVertices[i].vPosition, vBasePosStart);
		pVertices[i].vTexcoord = _float2(_float(i) / numSides, 0.0f);

		XMStoreFloat3(&pVertices[i + numSides].vPosition, vBasePosEnd);
		pVertices[i + numSides].vTexcoord = _float2(_float(i) / numSides, 1.0f);
	}






	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Wire* CVIBuffer_Wire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Wire* pInstance = new CVIBuffer_Wire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Wire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CVIBuffer_Wire::Clone(void* pArg)
{
	CVIBuffer_Wire* pInstance = new CVIBuffer_Wire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Wire"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Wire::Free()
{
	__super::Free();

}
