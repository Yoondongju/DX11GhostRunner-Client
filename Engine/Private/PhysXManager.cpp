#include "../Public/PhysXManager.h"


CPhysXManager::CPhysXManager()
{

}

HRESULT CPhysXManager::Initialize()
{
	if (FAILED(Ready_PhysX()))
		return E_FAIL;
	
	m_Player_WalkAble_Mesh.reserve(150);	// 일단임시

	return S_OK;
}



HRESULT CPhysXManager::Ready_PhysX()
{
	// Foundation 객체 생성
	static PxDefaultAllocator Allocator;
	static MyErrorCallback	  CallBackError;

	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, Allocator, CallBackError);
	if (!m_pFoundation) {
		MSG_BOX(TEXT("PxCreateFoundation failed!"));
		return E_FAIL;
	}

	
	// PVD 설정
	m_pPvd = PxCreatePvd(*m_pFoundation);
	if (!m_pPvd) {
		MSG_BOX(TEXT("PxCreatePvd failed!"));

		m_pFoundation->release();
		return E_FAIL;
	}
	
	m_pTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	if (!m_pTransport) {
		MSG_BOX(TEXT("PxDefaultPvdSocketTransportCreate failed!"));
		
		m_pPvd->release();
		m_pFoundation->release();
		return E_FAIL;
	}
	
	m_pPvd->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);

	//transport->release();


	// Physics 객체 생성
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);
	if (!m_pPhysics) {
		MSG_BOX(TEXT("PxCreatePhysics  failed!"));

		m_pPvd->release();
		m_pFoundation->release();
		return E_FAIL;
	}

	PxInitExtensions(*m_pPhysics, m_pPvd);


	// Scene 객체 생성
	PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, 0.f, 0.0f);
	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
	if (!m_pDispatcher) {
		MSG_BOX(TEXT("PxDefaultCpuDispatcherCreate failed!"));

		m_pPhysics->release();
		m_pPvd->release();
		m_pFoundation->release();
		return E_FAIL;
	}

	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	m_pScene = m_pPhysics->createScene(sceneDesc);

	if (!m_pScene) {
		MSG_BOX(TEXT("createScene failed!"));

		m_pDispatcher->release();
		m_pPhysics->release();
		m_pPvd->release();
		m_pFoundation->release();
		return E_FAIL;
	}

	


	PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient();
	

	if (pPvdClient)
	{
		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true); //강체 관련

		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true); // 충돌 관련

		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true); // Scene 쿼리
	}
	





	// Material 객체 생성
	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	if (!m_pMaterial) {
		MSG_BOX(TEXT("createMaterial failed!"));

		m_pScene->release();
		m_pDispatcher->release();
		m_pPhysics->release();
		m_pPvd->release();
		m_pFoundation->release();
		return E_FAIL;
	}




	//MSG_BOX(TEXT("PhysX가 성공적으로 초기화되었습니다."));
	return S_OK;
}

HRESULT	CPhysXManager::Add_WalkAble_Mesh(const PLAYER_WALKABLE_MESH& WalkAbleMesh)
{
	m_Player_WalkAble_Mesh.emplace_back(WalkAbleMesh);

	return S_OK;
}

_bool CPhysXManager::CollisionUpdate_PlayerToTriangleMeshGeometry(PxVec3* pOutDir, PxReal* pOutDepth, PxShape* pPlayerShape, PxTransform* pPlayerTransform, CGameObject** pCollTarget)
{
	for (_uint i = 0; i < m_Player_WalkAble_Mesh.size(); i++)
	{
		// 두 지오메트리 간의 충돌 검사 수행
		PxComputeTriangleMeshPenetration(
			*pOutDir,
			*pOutDepth,
			pPlayerShape->getGeometry(),
			*pPlayerTransform,
			m_Player_WalkAble_Mesh[i].MeshGeometry,
			m_Player_WalkAble_Mesh[i].PxTransform,
			4);

		


		if (*pOutDepth > 0.f)	// 플레이어가 안으로 들어갔니
		{
			*pCollTarget = m_Player_WalkAble_Mesh[i].pOnwer;
			return true;
		}
		

	}
	

	return false;
}


void CPhysXManager::Update(_float fTimeDelta)
{
	// Static 물체 : 물리 시뮬레이션에서 움직이이잖는다				 (지면,벽 , 건물등 움직이지 않는 고정된 물체)
	// Dynamic 물체 : 물리 시뮬레이션에 위치나 속도가 업데이트된다.  (캐릭터, 떨어지는 물체등등)  연산부하 크다

	// 씬 업데이트
	if (m_pScene)
		m_pScene->simulate(fTimeDelta); // 60 FPS
	if (m_pScene)
		m_pScene->fetchResults(true);

}

void CPhysXManager::Clear()
{
	m_Player_WalkAble_Mesh.clear();
}


CPhysXManager* CPhysXManager::Create()
{
	CPhysXManager* pInstance = new CPhysXManager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CPhysXManager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysXManager::Free()
{
	__super::Free();


	m_Player_WalkAble_Mesh.clear();


	// 내가 씌운 rigid스태틱, 머시기 삭제하고  지오메트리 삭제하고

	// 머테리얼이 엑터->attachShape에
	// shpae만들때 필요하니가

	// 액터를 지우면 머테리얼이삭제안해도 액터지울때 지가 지운다?
	// 


	if (m_pMaterial)
	{
		m_pMaterial->release();
		m_pMaterial = nullptr;
	}
		

	if (m_pScene)
		m_pScene->release();
	m_pScene = nullptr;
	

	PxCloseExtensions();


	if (m_pDispatcher)
		m_pDispatcher->release();
	m_pDispatcher = nullptr;

	
	if (m_pPhysics)
	{
		m_pPhysics->release();
		
	}
		
	m_pPhysics = nullptr;


	if (m_pTransport)
	{
		m_pTransport->release();
		m_pTransport = nullptr;
	}

	if (m_pPvd)
	{
		m_pPvd->disconnect();  // PVD 연결 해제
		m_pPvd->release();
	}
	m_pPvd = nullptr;


	if (m_pFoundation)
		m_pFoundation->release();
	m_pFoundation = nullptr;
}
