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
	_uint numSides = 24;					// �ܸ��� ���� �� (24���� �����)
	_uint numVertices = numSides * 2;		// �� �ܸ��� ���� ��
	m_iNumVertices = numVertices;
	m_iVertexStride = sizeof(VTXPOSTEX);

	_uint numTriangles = numSides * 2; // ���Ʒ� �� ���� �ﰢ�� ��
	_uint numIndices = numTriangles * 3; // �� �ﰢ���� 3���� �ε���

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

	/* �������ۿ� ä���� ������ ��������ؼ� �ӽ������� ������ �Ҵ��Ѵ�. */
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	_float angleStep = XM_2PI / numSides;
	for (_uint i = 0; i < numSides; ++i) {
		_float angle = i * angleStep;
		_float x = cosf(angle) * 0.5f; // �ܸ� �������� 0.5
		_float z = sinf(angle) * 0.5f;

		// �Ʒ� �ܸ��� ����
		pVertices[i].vPosition = _float3(x, -0.5f, z); // y ��ǥ -0.5�� �Ʒ���
		pVertices[i].vTexcoord = _float2(_float(i) / numSides, 0.0f);

		// ���� �ܸ��� ����
		pVertices[i + numSides].vPosition = _float3(x, 0.5f, z); // y ��ǥ 0.5�� ����
		pVertices[i + numSides].vTexcoord = _float2(_float(i) / numSides, 1.0f);
	}


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* �������۸� �����Ѵ�. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	/* �ε��������� ������ ä���ְ� */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �����Ѵ�. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_int idx = 0;
	for (_int i = 0; i < numSides; ++i) {
		_int next = (i + 1) % numSides;

		// �Ʒ� �ܸ�� ���� �ܸ��� �����ϴ� �� ���� �ﰢ��
		pIndices[idx++] = i;
		pIndices[idx++] = next;
		pIndices[idx++] = i + numSides;

		pIndices[idx++] = next;
		pIndices[idx++] = next + numSides;
		pIndices[idx++] = i + numSides;
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* �ε������۸� �����Ѵ�. */
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
	

	_float radius = 0.5f;  // ������� ������
	_uint numSides = m_iNumVertices / 2; // ���� ������ �� �ܸ��� ���� ��

	_float length = XMVectorGetX(XMVector3Length(vEnd - vStart)); // ������� ����
	_float frequency = 10.0f; // ������ ���ļ� (���� ���߸� ������ �� �ѷ����� �� �ֽ��ϴ�)
	_float amplitude = 0.3f; // ������ ���� (���� ���߸� ȿ���� �� �ѷ������ϴ�)

	for (_uint i = 0; i < numSides; ++i) {
		_float angle = (XM_2PI / numSides) * i;
		_float x = cosf(angle) * radius;
		_float z = sinf(angle) * radius;

		_vector vOffset = vRightNor * x + vUpNor * z;

		// �Ʒ� �ܸ��� ����
		_vector vBasePosStart = XMVectorAdd(vStart, vOffset);
		_vector vBasePosEnd = XMVectorAdd(vEnd, vOffset);

		// S�� ���·� �ܷ��̴� ȿ�� �߰�
		_float progress = (static_cast<_float>(i) / numSides); // 0.0f to 1.0f
		_float waveOffset = amplitude * sinf(frequency * progress * XM_2PI); // S�� ���� ����

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
