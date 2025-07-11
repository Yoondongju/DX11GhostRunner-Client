#include "stdafx.h"
#include "Static_Object.h"
#include "GameInstance.h"


#include "Terrain.h"
#include "Mesh.h"
#include "GrapplingPointUI.h"

CStatic_Object::CStatic_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CStatic_Object::CStatic_Object(const CStatic_Object& Prototype)
    : CGameObject(Prototype)
{

}

HRESULT CStatic_Object::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStatic_Object::Initialize(void* pArg)
{
    __super::Initialize(pArg);

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PhysX()))
        return E_FAIL;

    if (FAILED(Ready_HandleModelTypeTasks()))
        return E_FAIL;

    return S_OK;
}

void CStatic_Object::Priority_Update(_float fTimeDelta)
{

}

void CStatic_Object::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
   
    m_PxTransform.p = { XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos) };

    m_pPxRigidStatic->setGlobalPose(m_PxTransform);

    m_fTextCoordTime += fTimeDelta;
   
}

void CStatic_Object::Late_Update(_float fTimeDelta)
{  
     m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
     //m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);    // 렉이 너무많이 걸려 그림자
}

HRESULT CStatic_Object::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4((CPipeLine::D3DTS_VIEW)))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


 
    _uint iNumMeshes = m_pModel->Get_NumMeshes();
    _uint iPassNum = 0;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (E_FAIL == m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i))
        {
            if(LEVEL_STAGE2_BOSS != g_CurLevel)
                iPassNum = 6;
            else
            {
                iPassNum = 8;
            
                if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTextCoordTime, sizeof(_float))))
                    return E_FAIL; 
            
            
                _vector vCamPos = m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY)->Get_Transform()->Get_State(CTransform::STATE_POSITION);
                if (FAILED(m_pShaderCom->Bind_RawValue("g_CameraPosition", &vCamPos , sizeof(_vector))))
                    return E_FAIL;  
            
            
                if (nullptr != m_pCircuitTex)
                {
                    if (FAILED(m_pCircuitTex->Bind_ShadeResource(m_pShaderCom, "g_CircuitTex", 0)))
                        return E_FAIL;
                }
                if (nullptr != m_pMossaicTex)
                {
                    if (FAILED(m_pMossaicTex->Bind_ShadeResource(m_pShaderCom, "g_MossaicTex", 0)))
                        return E_FAIL;
                }          
            }               
        }      
        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_HEIGHT, i)))
        {
            int test;
        }
        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_SpecularTexture", aiTextureType_SHININESS, i)))
        {
            int test;
        }
    
        if (true == m_isChangeTexCoord)     // 기차
        {
            iPassNum = 5;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTextCoordTime, sizeof(_float))))
                return E_FAIL;

            _float fSpeed = 0.5f;
            if (FAILED(m_pShaderCom->Bind_RawValue("g_fScrollSpeed", &fSpeed, sizeof(_float))))
                return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Begin(iPassNum)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CStatic_Object::Render_LightDepth()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4		ViewMatrix;
    XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 20.f, -15.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModel->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pModel->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_HEIGHT, i)))
        {
            int test;
        }

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        if (FAILED(m_pModel->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CStatic_Object::Ready_Component()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Model*/
    if (FAILED(__super::Add_Component(g_CurLevel, m_strModelPrototypeName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel), nullptr)))
        return E_FAIL;


    if (LEVEL_STAGE2_BOSS == g_CurLevel)
    {
        if (FAILED(__super::Add_Component(LEVEL_STAGE2_BOSS, TEXT("Prototype_Component_Texture_Circuit"),
            TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pCircuitTex))))
            return E_FAIL;

        if (FAILED(__super::Add_Component(LEVEL_STAGE2_BOSS, TEXT("Prototype_Component_Texture_Mossaic"),
            TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pMossaicTex))))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CStatic_Object::Ready_PhysX()
{
    const _float4x4* pWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    PxVec3 position(pWorldMatrix->_41, pWorldMatrix->_42, pWorldMatrix->_43);

    // 스케일 값을 추출
    PxVec3 scale;
    scale.x = PxVec3(pWorldMatrix->_11, pWorldMatrix->_12, pWorldMatrix->_13).magnitude();
    scale.y = PxVec3(pWorldMatrix->_21, pWorldMatrix->_22, pWorldMatrix->_23).magnitude();
    scale.z = PxVec3(pWorldMatrix->_31, pWorldMatrix->_32, pWorldMatrix->_33).magnitude();

    // 스케일을 제거한 회전 벡터
    PxVec3 rotationX = PxVec3(pWorldMatrix->_11, pWorldMatrix->_12, pWorldMatrix->_13) / scale.x;
    PxVec3 rotationY = PxVec3(pWorldMatrix->_21, pWorldMatrix->_22, pWorldMatrix->_23) / scale.y;
    PxVec3 rotationZ = PxVec3(pWorldMatrix->_31, pWorldMatrix->_32, pWorldMatrix->_33) / scale.z;

    // 정규화된 회전 벡터로 회전 행렬을 생성.
    PxMat33 rotationMatrix(rotationX, rotationY, rotationZ);

    PxQuat rotation = PxQuat(rotationMatrix);

    // PxTransform을 위치와 회전으로 초기화합니다.
    m_PxTransform = PxTransform(position, rotation);

    m_pPxRigidStatic = m_pGameInstance->Get_Physics()->createRigidStatic(m_PxTransform);

    PxMaterial* Material = m_pGameInstance->Get_Material();
    vector<CMesh*>& Meshs = m_pModel->Get_Meshes();

    m_MeshGeometry.resize(Meshs.size());
  

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

        m_MeshGeometry[i] = PxTriangleMeshGeometry(pTriangleMesh);

        m_pShape = m_pGameInstance->Get_Physics()->createShape(m_MeshGeometry[i], *Material);
        if (!m_pShape) {
            MSG_BOX(TEXT("createShape failed!"));
            return E_FAIL;
        }


    



        m_pPxRigidStatic->attachShape(*m_pShape);

        CPhysXManager::PLAYER_WALKABLE_MESH Desc = { m_MeshGeometry[i] , m_PxTransform, this };
  
        m_pGameInstance->Add_WalkAble_Mesh(Desc);
    }

    m_pGameInstance->Get_Scene()->addActor(*m_pPxRigidStatic);

    return S_OK;
}

HRESULT CStatic_Object::Ready_HandleModelTypeTasks()
{
    switch (m_iObjectType)
    {
    case MODEL_CHECK_LIST::BILLBOARD:
    {
        m_bLandWall = true;
        m_strChangeLayerName = L"Layer_LandWall_Object";
    }
        break;

    case MODEL_CHECK_LIST::CRANE:
    {
        if (TEXT("Prototype_Component_Model_Crane2") == m_strModelPrototypeName)
        {
            CGrapplingPointUI::GRAPPLINGPOINT_DESC Desc = { CGrapplingPointUI::GRAPPLINGPOINT_TYPE::TYPE_OUTRING , this };
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(g_CurLevel, TEXT("Layer_GrapplingPointUI"),
                TEXT("Prototype_GameObject_GrapplingPointUI"), TEXT("안 넣어도댐"), &Desc)))
                return E_FAIL;
        }
    }
        break;

    case MODEL_CHECK_LIST::CLIMBOBJECT:
    {
        m_bClimbing = true;
        m_strChangeLayerName = L"Layer_Climb_Object";
    }
    break;

    case MODEL_CHECK_LIST::TRAIN:
    {
        m_isChangeTexCoord = true;
    }
    break;


    default:
        break;
    }


    return S_OK;
}

CStatic_Object* CStatic_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStatic_Object* pInstance = new CStatic_Object(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CStatic_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CStatic_Object::Clone(void* pArg)
{
    CStatic_Object* pInstance = new CStatic_Object(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CStatic_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStatic_Object::Free()
{
    __super::Free();

    if(m_pPxRigidStatic)
        m_pPxRigidStatic->release();

    if (m_pShape)
        m_pShape->release();


    for (_uint i = 0; i < m_MeshGeometry.size(); i++)
    {
        m_MeshGeometry[i].triangleMesh->release();
    }
    m_MeshGeometry.clear();

        
    Safe_Release(m_pCircuitTex);
    Safe_Release(m_pMossaicTex);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModel);
}
