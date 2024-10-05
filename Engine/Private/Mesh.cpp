#include "..\Public\Mesh.h"

#include "GameInstance.h"


CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh & Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(CModel* pModel, CModel::TYPE eModelType, _fmatrix PreTransformMatrix, void* pArg)
{

	CMesh::MESH_DESC* pDesc = static_cast<CMesh::MESH_DESC*>(pArg);

	strcpy_s(m_szName, pDesc->pName);

	switch (eModelType)
	{
	case Engine::CModel::TYPE_NONANIM:
		Ready_VertexBuffer_NonAnim(pArg, PreTransformMatrix);
		break;
	case Engine::CModel::TYPE_ANIM:
		Ready_VertexBuffer_Anim(pArg,pModel);
		break;
	case Engine::CModel::TYPE_END:
		break;
	default:
		break;
	}
	


#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pDesc->pIndices;

	m_pIndices = new _uint[m_iNumIndices];
	memcpy(m_pIndices, pDesc->pIndices, sizeof(_uint) * m_iNumIndices);

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion


	// NonAnimModel Instancing
	if (true == pDesc->isInstanceObject)
	{
		// 음.. 여기서 인스턴싱 걔네들 만들까
		CVIBuffer_Mesh_Instance::MESHINSTANCE_DESC			Desc{};
		Desc.iNumInstance = 60;
		Desc.vCenter = _float3(0.f, 0.f, 0.f);
		Desc.vRange = _float3(1.f, 2.5f, 1.f);
		Desc.vSize = _float2(0.5f, 2.f);
		Desc.vPivot = _float3(0.f, 0.f, 0.f);
		Desc.vSpeed = _float2(10.f, 15.f);
		Desc.vLifeTime = _float2(0.5f, 1.5f);
		Desc.isLoop = false;

		Desc.pVB = m_pVB;
		Desc.pIB = m_pIB;

		Desc.eIndexFormat = m_eIndexFormat;
		Desc.eTopology = m_eTopology;
		Desc.iIndexCountPerInstance = m_iNumIndices;

		// 3이 LEVEL_GAMEPLAY임 일단
		if (FAILED(m_pGameInstance->Add_Prototype(3, pDesc->InstanceBufferPrototypeTag,
			CVIBuffer_Mesh_Instance::Create(m_pDevice, m_pContext, Desc))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(const CModel* pModel, CShader* pShader, const _char* pConstantName)
{
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * g_iMaxMeshBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * pModel->Get_BoneCombindTransformationMatrix(m_BoneIndices[i]));
	}

	return  pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(void* pArg , _fmatrix PreTransformMatrix)
{
	MESH_DESC* pDesc = static_cast<MESH_DESC*>(pArg);

	m_iMaterialIndex = pDesc->iMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pDesc->iNumVertices;
	m_iVertexStride = sizeof(VTXMESH);
	m_iNumIndices = pDesc->iNumIndices;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	m_pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&m_pVertices[i].vPosition, &pDesc->pVertices[i].vPosition, sizeof(_float3));
		//XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
		// PreTransformMatrix는 맵툴에서 이미 다 세팅 해놨음
		
		memcpy(&m_pVertices[i].vNormal, &pDesc->pVertices[i].vNormal, sizeof(_float3));
		memcpy(&m_pVertices[i].vTexcoord, &pDesc->pVertices[i].vTexcoord, sizeof(_float2));
		memcpy(&m_pVertices[i].vTangent, &pDesc->pVertices[i].vTangent, sizeof(_float3));
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	
	XMStoreFloat3(&m_vMinPos, XMLoadFloat3(&pDesc->vMinPos));
	XMStoreFloat3(&m_vMaxPos, XMLoadFloat3(&pDesc->vMaxPos));



	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(void* pArg ,const CModel* pModel)
{
	MESH_DESC* pDesc = static_cast<MESH_DESC*>(pArg);

	m_iMaterialIndex = pDesc->iMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pDesc->iNumVertices;
	m_iVertexStride = sizeof(VTXANIMMESH);
	m_iNumIndices = pDesc->iNumIndices;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;
	
	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXANIMMESH* pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);
	
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pAnimVertices[i].vPosition, &pDesc->pAnimVertices[i].vPosition, sizeof(_float3));
		memcpy(&pAnimVertices[i].vNormal, &pDesc->pAnimVertices[i].vNormal, sizeof(_float3));
		memcpy(&pAnimVertices[i].vTexcoord, &pDesc->pAnimVertices[i].vTexcoord, sizeof(_float2));
		memcpy(&pAnimVertices[i].vTangent, &pDesc->pAnimVertices[i].vTangent, sizeof(_float3));

		memcpy(&pAnimVertices[i].vBlendWeights, &pDesc->pAnimVertices[i].vBlendWeights, sizeof(_float4));
		memcpy(&pAnimVertices[i].vBlendIndices, &pDesc->pAnimVertices[i].vBlendIndices, sizeof(XMUINT4));

	}
	
	/* 모델 전체의 뼈가 아닌 이 메시하나에 영향을 주는 뼈의 갯수. */
	m_iNumBones = pDesc->iNumAffectBones;
	
	m_OffsetMatrices.resize(m_iNumBones);
	m_BoneIndices.resize(m_iNumBones);
	
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		//aiBone* pAIBone = pAIMesh->mBones[i];
	
		/* pAIBone->mOffsetMatrix : 여러개의 모델이 하나의 뼈대를 공유하는 경우, 모델의 형태에 따라서 스키닝(정점의 위치 * 뼈행렬)작업 시, 추가적인 보정이 필요할 수 있다.  */
		memcpy(&m_OffsetMatrices[i], &pDesc->vecOffsetMatrix[i], sizeof(_float4x4));



		//XMStoreFloat4x4(&m_OffsetMatrices[i], XMMatrixTranspose(XMLoadFloat4x4(&m_OffsetMatrices[i])));
	
		/* 내 모델이 들고 있는 전체 뼈들중에 이 메시가 사용하는 뼈와이름이 같은 뼈의 인덱스를 저장해둔다. */
		m_BoneIndices[i] = pDesc->vecIndices[i];
	
	
		// pAIBone 란 이 메쉬에 영향을 주는 뼈들중에 하나 !!
		/* 이 뼈는 몇개(mNumWeights) 정점에게 영향을 주는가? */
		
		/* 이 뼈가 영향을 주는 j번째 정점의 인덱스(pAIBone->mWeights[j].mVertexId)*/
		// 아직 이 정점에 대해 특정 뼈의 가중치가 할당되지 않았다는 것을 확인하기 위해서입니다.
		// 이 정점이 영향을 받을수있는 뼈 4개중 첫번째가 0이라면 영향을 받을수잇는공간이 잇다는거니까 ! 
		// 받을수 있는 공간에 내 메쉬에 영향을 주는 뼈들중 하나인 pAIBone ! 그녀석은  영향을 주는 뼈들중에서 몇번째? i 번째 !
			
	}
	
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;
	
		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(m_szName));
	
		_float4x4	OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}
	
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pAnimVertices;
	
	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
	
	
	XMStoreFloat3(&m_vMinPos, XMLoadFloat3(&pDesc->vMinPos));
	XMStoreFloat3(&m_vMaxPos, XMLoadFloat3(&pDesc->vMaxPos));

	Safe_Delete_Array(pAnimVertices);
	
	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel* pModel, CModel::TYPE eModelType, _fmatrix PreTransformMatrix, void* pArg)
{
	CMesh*		pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModel,eModelType ,PreTransformMatrix, pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent * CMesh::Clone(void * pArg)
{
	CMesh*		pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pIndices);
}
