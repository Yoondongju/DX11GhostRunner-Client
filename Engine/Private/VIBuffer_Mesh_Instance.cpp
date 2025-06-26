#include "../Public/VIBuffer_Mesh_Instance.h"

#include "GameInstance.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype)
	: CVIBuffer_Instancing{ Prototype }
{
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Prototype(const CVIBuffer_Mesh_Instance::MESHINSTANCE_DESC& Desc)
{
	if (FAILED(__super::Initialize_Prototype(Desc)))
		return E_FAIL;

	m_iNumVertexBuffers = 2;								// 필요
	//m_iNumVertices = 4;									// 버텍스 몇개니 ? 이건 버텍스버퍼 만들때 필요하니 필없다
	//m_iVertexStride = sizeof(VTXMESH);					// 마찬가지네
	//m_iNumIndices = 6;									// 마찬가지네
	//m_iIndexStride = 2;									// 마찬가지네

	m_eIndexFormat = Desc.eIndexFormat;
	m_eTopology = Desc.eTopology;
	m_iInstanceStride = sizeof(VTXMESHINSTANCE);			// 필요 
	m_iIndexCountPerInstance = Desc.iIndexCountPerInstance;
	m_iVertexStride = sizeof(VTXMESH);

	m_pVB = Desc.pVB;
	m_pIB = Desc.pIB;
	
	Safe_AddRef(m_pVB);		// 아직 잘모르겟어 레퍼런스 올려야하는지.. 일단 미정
	Safe_AddRef(m_pIB);		// 아직 잘모르겟어 레퍼런스 올려야하는지.. 일단 미정


#pragma region INSTANCE_BUFFER

	// 필요한것 
	// m_iInstanceStride,

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXMESHINSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXMESHINSTANCE) * m_iNumInstance);

	VTXMESHINSTANCE* pInstanceVertices = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float	fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);

		pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f),
			m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f),
			m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f),
			1.f);

		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

#pragma endregion



	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Initialize(void* pArg)
{
#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

void CVIBuffer_Mesh_Instance::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta * 2.f);

		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Mesh_Instance::Spread2(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);

		_vector vMoveStep = (XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta * 2);

		_vector vGravity = XMVectorSet(-2.f * fTimeDelta, -5.f * fTimeDelta, -2.f * fTimeDelta, 0.f); // 중력 적용, 9.8은 가속도 값
		vMoveStep += vGravity;

		// 위치 업데이트
		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveStep);

		// 생명 주기 업데이트
		pVertices[i].vLifeTime.y += fTimeDelta;

		// 루프 처리 (피가 튀는 효과가 반복될 경우)
		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Mesh_Instance::Spread_Side_XZ(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float fAngle = (2 * XM_PI / m_iNumInstance) * i; // 360도를 인스턴스 수로 나눠 각 인스턴스마다 고유한 각도 부여

		_vector vMoveDir = XMVectorSet(cosf(fAngle), 0.f, sinf(fAngle), 0.f);

		_vector vMovement = XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta;


		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + vMovement);



		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Mesh_Instance::Spread_Side_XY(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{

		_float fAngle = (2 * XM_PI / m_iNumInstance) * i; // 360도를 인스턴스 수로 나눠 각 인스턴스마다 고유한 각도 부여

		_vector vMoveDir = XMVectorSet(cosf(fAngle), sinf(fAngle), 0.f, 0.f);

		_vector vMovement = XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta;


		XMStoreFloat4(&pVertices[i].vTranslation,
			XMLoadFloat4(&pVertices[i].vTranslation) + vMovement);



		pVertices[i].vLifeTime.y += fTimeDelta;

		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}



void CVIBuffer_Mesh_Instance::Spread3(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector vMoveDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivotPos), 0.f);
		
		_vector vMoveStep = (XMVector3Normalize(vMoveDir) * m_pSpeed[i] * fTimeDelta * 3);

		
		// 중력 효과 추가 (Y 축으로 중력값 빼기, Y 축이 위쪽 방향이라고 가정)
		_vector vGravity = XMVectorSet(0.f, -9.8f * fTimeDelta, 0.f, 0.f); // 중력 적용, 9.8은 가속도 값
		vMoveStep += vGravity;

		// 위치 업데이트
		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveStep);

		// 생명 주기 업데이트
		pVertices[i].vLifeTime.y += fTimeDelta;

		// 루프 처리 (피가 튀는 효과가 반복될 경우)
		if (true == m_isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i].vTranslation = static_cast<VTXMESHINSTANCE*>(m_pInstanceVertices)[i].vTranslation;
			pVertices[i].vLifeTime.y = 0.f;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}


void CVIBuffer_Mesh_Instance::Drop(_float fTimeDelta)
{
	
}




CVIBuffer_Mesh_Instance* CVIBuffer_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Mesh_Instance::MESHINSTANCE_DESC& Desc)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Mesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CVIBuffer_Mesh_Instance::Clone(void* pArg)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Mesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Mesh_Instance::Free()
{
	__super::Free();

}

void CVIBuffer_Mesh_Instance::ResetTranslation()
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	VTXMESHINSTANCE* pVertices = static_cast<VTXMESHINSTANCE*>(SubResource.pData);

	memcpy(pVertices, m_pInstanceVertices, sizeof(VTXMESHINSTANCE) * m_iNumInstance);

	m_pContext->Unmap(m_pVBInstance, 0);
}
