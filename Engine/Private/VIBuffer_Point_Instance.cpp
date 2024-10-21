#include "..\Public\VIBuffer_Point_Instance.h"

#include "GameInstance.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype)
	: CVIBuffer_Instancing{ Prototype ,true }
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	if (FAILED(__super::Initialize_Prototype(Desc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize(void* pArg)
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iNumIndices = m_iNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iInstanceStride = sizeof(VTXPOINTINSTANCE);
	m_iIndexCountPerInstance = 1;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);
	if (nullptr != pArg)
	{
		if (false == static_cast<POINT_DESC*>(pArg)->isRandomSize)
		{
			pVertices->vPSize = _float2(m_vSize.x, m_vSize.y);
		}
	}
	else
	{
		_float	fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);
		pVertices->vPSize = _float2(fScale, fScale);
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

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXPOINTINSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXPOINTINSTANCE) * m_iNumInstance);

	VTXPOINTINSTANCE* pInstanceVertices = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);


		if (false == m_isStartFromOrigin)
		{
			pInstanceVertices[i].vTranslation = _float4(
				m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f),
				m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f),
				m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f),
				1.f);
		}
		else
		{
			pInstanceVertices[i].vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
		}


		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;
#pragma endregion




#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

void CVIBuffer_Point_Instance::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}



void CVIBuffer_Point_Instance::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vMoveDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Point_Instance::DirectionSpread(_float fTimeDelta, _fvector vDir)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector vFinalDir =  XMVector3Normalize(vDir);

		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + vFinalDir * m_pSpeed[i] * fTimeDelta);


		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Point_Instance::Spread_OriginPos(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		//_vector		vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);
		//vMoveDir.m128_f32[1] = 0.f;

		_float fAngle = (2 * XM_PI / m_iNumInstance) * i; // 360도를 인스턴스 수로 나눠 각 인스턴스마다 고유한 각도 부여

		_vector vMoveDir = XMVectorSet(cosf(fAngle), sinf(fAngle), sinf(fAngle), 0.f);
		_vector vMovement = XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta;


		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + vMovement);



		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Point_Instance::Spread_Side_X(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);
		vMoveDir.m128_f32[2] = 0.f;

		_int iDir = 1; 
		if (i % 2 == 0)
			iDir = -1;

		_vector vRightDir = XMVectorSet((_float)iDir, 0.f, 0.f, 0.f); // Right 방향 벡터 (X축 방향)
		_vector vRightMovement = vRightDir * m_pSpeed[i] * fTimeDelta * 0.5f;


		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta + vRightMovement);





		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Point_Instance::Spread_Side_XZ(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		//_vector		vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);
		//vMoveDir.m128_f32[1] = 0.f;

		_float fAngle = (2 * XM_PI / m_iNumInstance) * i; // 360도를 인스턴스 수로 나눠 각 인스턴스마다 고유한 각도 부여

		_vector vMoveDir = XMVectorSet(cosf(fAngle), 0.f, sinf(fAngle), 0.f);

		_vector vMovement = XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta;


		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + vMovement);

		
	
		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Point_Instance::Spread_Side_XY(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		//_vector		vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);
		//vMoveDir.m128_f32[1] = 0.f;

		_float fAngle = (2 * XM_PI / m_iNumInstance) * i; // 360도를 인스턴스 수로 나눠 각 인스턴스마다 고유한 각도 부여

		_vector vMoveDir = XMVectorSet(cosf(fAngle), sinf(fAngle), 0.f, 0.f);

		_vector vMovement = XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta;


		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + vMovement);



		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Point_Instance::ForTrailLifeTimeMul(_float fTimeDelta, _fvector vDir, _float fTrailDuration)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector vFinalDir = XMVector3Normalize(vDir);

		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + vFinalDir * m_pSpeed[i] * fTimeDelta * 0.1f);


		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= fTrailDuration)
		{
			pVertices[i].vTranslation = static_cast<VTXPOINTINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Point_Instance::ResetTranslation()
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	VTXPOINTINSTANCE* pVertices = static_cast<VTXPOINTINSTANCE*>(SubResource.pData);

	memcpy(pVertices, m_pInstanceVertices, sizeof(VTXPOINTINSTANCE)* m_iNumInstance);

	m_pContext->Unmap(m_pVBInstance, 0);
}


CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CVIBuffer_Point_Instance::Clone(void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

}
