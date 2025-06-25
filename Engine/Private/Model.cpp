#include "..\Public\Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

#include "Animation.h"


CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{

}

CModel::CModel(const CModel & Prototype)
	: CComponent{ Prototype }
	, m_iNumMeshes { Prototype.m_iNumMeshes } 
	, m_Meshes{ Prototype.m_Meshes }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials { Prototype.m_Materials}

	//, m_Bones{ Prototype.m_Bones }
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }

	, m_iNextAnimIndex{ Prototype.m_iNextAnimIndex }
	, m_iCurrentAnimIndex{ Prototype.m_iCurrentAnimIndex }

	, m_iNumAnimations{ Prototype.m_iNumAnimations }
	, m_Animations{ Prototype.m_Animations }

	, m_KeyFrameIndices {Prototype.m_KeyFrameIndices}
{

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_AddRef(pTexture);
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);
	
}

_uint CModel::Get_NumAffectBone(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_NumAffectBone();
}

_uint CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_uint	iBoneIndex = { 0 };
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (0 == strcmp(pBone->Get_Name(), pBoneName))
				return true;
			++iBoneIndex;
			return false;
		});

	if (iter == m_Bones.end())
		MSG_BOX(TEXT("없는데?"));

	return iBoneIndex;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _char * pModelFilePath,  _wstring* pMaterialTexturesPath, _uint iNumMeshes, _uint iMaterialsCount  ,_fmatrix PreTransformMatrix, void* pInitMeshArg , void* pInitBoneArg, void* pInitAnimationArg)
{
	_uint		iFlag = { 0 };	
	
	/* 이전 : 모든 메시가 다 원점을 기준으로 그렺니다. */
	/* 이후 : 모델을 구성하는 모든 메시들을 각각 정해진 상태(메시를 배치하기위한 뼈대의 위치에 맞춰서)대로 미리 변환해준다.*/

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;	

	/*aiProcess_GlobalScale*/

	//if (eType == TYPE_NONANIM)
	//	iFlag |= aiProcess_PreTransformVertices;	
	//
	//m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	//if (nullptr == m_pAIScene)
	//	return E_FAIL;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	m_eType = eType;
	m_iNumMaterials = iMaterialsCount;

	
	if (TYPE_ANIM == m_eType)
	{
		if (FAILED(Ready_Bones(static_cast<CBone::INIT_BONE_DESC*>(pInitBoneArg), -1)))
			return E_FAIL;
	}
	
	if (FAILED(Ready_Meshes(iNumMeshes, m_eType ,pInitMeshArg)))
		return E_FAIL;	

	if (FAILED(Ready_Materials(pModelFilePath, pMaterialTexturesPath)))
		return E_FAIL;

	if (TYPE_ANIM == m_eType)
	{
		if (FAILED(Ready_Animation(static_cast<CAnimation::ANIMATION_DESC*>(pInitAnimationArg))))
			return E_FAIL;

	}


	m_KeyFrameIndices.resize(m_iNumAnimations);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		_uint iNumChannel = m_Animations[i]->Get_NumChannel();
		m_KeyFrameIndices[i].resize(iNumChannel);
	}

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();	

	return S_OK;
}

void CModel::SetUp_Animation(_uint iAnimationIndex, _bool isLoop,  _float fNextLerp)
{
	if (-1.f != fNextLerp)
	{
		m_Animations[m_iCurrentAnimIndex]->Set_NextAnimLerpDuration(fNextLerp);
	}
	
	m_iNextAnimIndex = iAnimationIndex;

	m_isLoop = isLoop;
}


_bool CModel::Play_Animation(_float fTimeDelta, _bool isFrustumCulling)
{
	/* 뼈를 움직인다.(CBone`s m_TransformationMatrix행렬을 갱신한다.) */
	_bool isFinished = false;

	_bool isTransitioning = false;
	if (m_iNextAnimIndex != m_iCurrentAnimIndex)
		isTransitioning = true;


	isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(isFrustumCulling , m_Bones,
		&m_CurrentTrackPosition,
		m_KeyFrameIndices[m_iCurrentAnimIndex],
		m_isLoop,
		fTimeDelta,
		isTransitioning,
		m_Animations[m_iNextAnimIndex]);

	if (m_CurrentTrackPosition == 0.0)	// 다음 애니로 변환 끗!
	{
		m_iCurrentAnimIndex = m_iNextAnimIndex;
	}


	XMStoreFloat3(&m_AnimLocalTranslationAmount , m_Bones[1]->Get_TransformationMatrix().r[3]);
	m_Bones[1]->Set_TransformationMatrix(XMMatrixIdentity());
	

	//if (true == isFrustumCulling)
	//	return false;

	for (auto& pBone : m_Bones)
	{
		/* 내 뼈의 행렬 * 부모의 컴바인드 */
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}


	return isFinished;
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iMeshIndex)
{
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType])
		return m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType]->Bind_ShadeResource(pShader, pConstantName, 0);
	else
		return E_FAIL;
}

HRESULT CModel::Bind_MeshBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _float4x4* pOutBoneMatrix)
{
	m_Meshes[iMeshIndex]->Bind_BoneMatrices(this, pShader, pConstantName, pOutBoneMatrix);

	return S_OK;
}

HRESULT CModel::Ready_Meshes(_uint iNumMeshes, TYPE eModelType, void* pArg)
{
	m_iNumMeshes = iNumMeshes;
	CMesh::MESH_DESC* pDesc = static_cast<CMesh::MESH_DESC*>(pArg);
	if (nullptr == pDesc)
	{
		MSG_BOX(L"모델:메쉬 생성에 문제가 있습니다.");
		return E_FAIL;
	}
		
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
 		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, this ,eModelType ,XMLoadFloat4x4(&m_PreTransformMatrix), (void*)&pDesc[i]);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char * pModelFilePath,  _wstring* pMaterialTexturesPath)
{	
	for (size_t i = 0; i < m_iNumMaterials; i++)
	{

		MESH_MATERIAL		MeshMaterial{};
		_wstring strSrc = L"NO_PATH";

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{				
			if (pMaterialTexturesPath[i * AI_TEXTURE_TYPE_MAX + j] == strSrc)
				continue;

			MeshMaterial.pMaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, pMaterialTexturesPath[i * AI_TEXTURE_TYPE_MAX + j].c_str(), 1);
			if (nullptr == MeshMaterial.pMaterialTextures[j])
			{
				int  a = 1;
			}
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(void* pArg, _int iParentBoneIndex)
{
	CBone::INIT_BONE_DESC* Desc = static_cast<CBone::INIT_BONE_DESC*>(pArg);

	// 각각 뼈들은 자기의 자식뼈갯수를 저장해놧어 

	_uint iIndex = m_Bones.size();


	CBone* pBone = CBone::Create(Desc->vecChildBones[iIndex].iNumChildBone, Desc->vecChildBones[iIndex].szName, Desc->vecChildBones[iIndex].TransformationMatrix , iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;
		
	
	m_Bones.emplace_back(pBone);
	
	_int		iParentIndex = m_Bones.size() - 1;
	
	for (size_t i = 0; i < Desc->vecChildBones[iIndex].iNumChildBone; i++)
	{
		//처음시점기준 pBone은 루트이고 이 루트의 자식은 자식이 몇개니 ?
	
		Ready_Bones(Desc, iParentIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation(void* pArg)
{
	CAnimation::ANIMATION_DESC* pDesc = static_cast<CAnimation::ANIMATION_DESC*>(pArg);

	m_iNumAnimations = pDesc->vecAnimationDesc.size();
	
	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(&pDesc->vecAnimationDesc[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;
	
		m_Animations.emplace_back(pAnimation);
	}
	return S_OK;
}


CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const _char * pModelFilePath,  _wstring* pMaterialTexturesPath, _uint iNumMeshes, _uint iMaterialsCount ,  _fmatrix PreTransformMatrix , void* pInitMeshArg, void* pInitBoneArg, void* pInitAnimationArg)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, pMaterialTexturesPath, iNumMeshes, iMaterialsCount, PreTransformMatrix, pInitMeshArg, pInitBoneArg, pInitAnimationArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CModel::Free()
{
	__super::Free();


	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();


	for (auto& Bone : m_Bones)
	{
		Safe_Release(Bone);
	}
	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();
}
