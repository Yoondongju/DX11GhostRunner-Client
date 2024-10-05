#include "../Public/PhysXManager.h"


CPhysXManager::CPhysXManager()
{

}

HRESULT CPhysXManager::Initialize()
{
	if (FAILED(Ready_PhysX()))
		return E_FAIL;
	
	m_Player_WalkAble_Mesh.reserve(150);	// �ϴ��ӽ�

	return S_OK;
}



HRESULT CPhysXManager::Ready_PhysX()
{
	// Foundation ��ü ����
	static PxDefaultAllocator Allocator;
	static MyErrorCallback	  CallBackError;

	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, Allocator, CallBackError);
	if (!m_pFoundation) {
		MSG_BOX(TEXT("PxCreateFoundation failed!"));
		return E_FAIL;
	}

	
	// PVD ����
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


	// Physics ��ü ����
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);
	if (!m_pPhysics) {
		MSG_BOX(TEXT("PxCreatePhysics  failed!"));

		m_pPvd->release();
		m_pFoundation->release();
		return E_FAIL;
	}

	PxInitExtensions(*m_pPhysics, m_pPvd);


	// Scene ��ü ����
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
		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true); //��ü ����

		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true); // �浹 ����

		pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true); // Scene ����
	}
	





	// Material ��ü ����
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




	//MSG_BOX(TEXT("PhysX�� ���������� �ʱ�ȭ�Ǿ����ϴ�."));
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
		// �� ������Ʈ�� ���� �浹 �˻� ����
		PxComputeTriangleMeshPenetration(
			*pOutDir,
			*pOutDepth,
			pPlayerShape->getGeometry(),
			*pPlayerTransform,
			m_Player_WalkAble_Mesh[i].MeshGeometry,
			m_Player_WalkAble_Mesh[i].PxTransform,
			4);

		


		if (*pOutDepth > 0.f)	// �÷��̾ ������ ����
		{
			*pCollTarget = m_Player_WalkAble_Mesh[i].pOnwer;
			return true;
		}
		

	}
	

	return false;
}


void CPhysXManager::Update(_float fTimeDelta)
{
	// Static ��ü : ���� �ùķ��̼ǿ��� ���������ݴ´�				 (����,�� , �ǹ��� �������� �ʴ� ������ ��ü)
	// Dynamic ��ü : ���� �ùķ��̼ǿ� ��ġ�� �ӵ��� ������Ʈ�ȴ�.  (ĳ����, �������� ��ü���)  ������� ũ��

	// �� ������Ʈ
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


	// ���� ���� rigid����ƽ, �ӽñ� �����ϰ�  ������Ʈ�� �����ϰ�

	// ���׸����� ����->attachShape��
	// shpae���鶧 �ʿ��ϴϰ�

	// ���͸� ����� ���׸����̻������ص� �������ﶧ ���� �����?
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
		m_pPvd->disconnect();  // PVD ���� ����
		m_pPvd->release();
	}
	m_pPvd = nullptr;


	if (m_pFoundation)
		m_pFoundation->release();
	m_pFoundation = nullptr;
}
