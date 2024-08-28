#include "stdafx.h"
#include "Building.h"
#include "GameInstance.h"


#include "Terrain.h"
#include "Mesh.h"

CBuilding::CBuilding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CBuilding::CBuilding(const CBuilding& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CBuilding::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBuilding::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PhysX()))
        return E_FAIL;


    return S_OK;
}

void CBuilding::Priority_Update(_float fTimeDelta)
{

}

void CBuilding::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    _vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
    _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    
    PxVec3 vStartPos = { XMVectorGetX(vUp), XMVectorGetY(vUp), XMVectorGetZ(vUp) };
    PxVec3 vEndPos = { -XMVectorGetX(vUp), -XMVectorGetY(vUp), -XMVectorGetZ(vUp) };
    
    m_PxTransform = PxTransformFromSegment(vStartPos, vEndPos);

    m_PxTransform.p = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };

    m_pPxRigidStatic->setGlobalPose(m_PxTransform);

}

void CBuilding::Late_Update(_float fTimeDelta)
{
      
     m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBuilding::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4((CPipeLine::D3DTS_VIEW)))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


 
    _uint iNumMeshes = m_pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CBuilding::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


  

    /* For.Com_Model*/
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelPrototypeName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CBuilding::Ready_PhysX()
{
    m_PxTransform = PxTransform(PxMat44(m_pTransformCom->Get_WorldMatrix_Ptr()->_11));

    m_pPxRigidStatic = m_pGameInstance->Get_Physics()->createRigidStatic(m_PxTransform);

    PxMaterial* Material = m_pGameInstance->Get_Material();

    vector<CMesh*>& Meshs = m_pModel->Get_Meshes();

    for (_uint i = 0; i < Meshs.size(); i++)
    {
        _uint iNumVertices = Meshs[i]->Get_NumVertices();
        VTXMESH* pVertices = Meshs[i]->Get_Vertices();


        _uint iNumIndices = Meshs[i]->Get_NumIndices();
        _uint* pIndices = Meshs[i]->Get_Indices();

        vector<PxVec3> vertices;
        vertices.reserve(iNumVertices);

        vector<PxU32> indices;
        indices.reserve(iNumIndices);



        for (_uint i = 0; i < iNumVertices; i++)
        {
            vertices.emplace_back(PxVec3(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z));
        }

        for (_uint i = 0; i < iNumIndices; i++)
        {
            indices.emplace_back(pIndices[i]);
        }

        // 1. Triangle Mesh 설명 (Desc) 생성
        PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = vertices.size();
        meshDesc.points.stride = sizeof(PxVec3);
        meshDesc.points.data = vertices.data();

        meshDesc.triangles.count = indices.size() / 3;
        meshDesc.triangles.stride = 3 * sizeof(PxU32);
        meshDesc.triangles.data = indices.data();


        // 2. Cooking 파라미터 설정
        PxCookingParams cookingParams(m_pGameInstance->Get_Physics()->getTolerancesScale());
        cookingParams.meshPreprocessParams = PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;  // Optional: 최적화

        // 3. Mesh 생성
        PxTriangleMesh* pTriangleMesh = PxCreateTriangleMesh(cookingParams, meshDesc);

        // 4. Triangle Mesh 기반 Shape 생성
        m_pShape = m_pGameInstance->Get_Physics()->createShape(PxTriangleMeshGeometry(pTriangleMesh), *Material);
        if (!m_pShape) {
            MSG_BOX(TEXT("createShape failed!"));
            return E_FAIL;
        }

        m_pPxRigidStatic->attachShape(*m_pShape);
    }

    m_pGameInstance->Get_Scene()->addActor(*m_pPxRigidStatic);

    return S_OK;
}

CBuilding* CBuilding::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBuilding* pInstance = new CBuilding(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBuilding"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBuilding::Clone(void* pArg)
{
    CBuilding* pInstance = new CBuilding(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CBuilding"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBuilding::Free()
{
    __super::Free();

    if(m_pPxRigidStatic)
        m_pPxRigidStatic->release();

    if (m_pShape)
        m_pShape->release();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
