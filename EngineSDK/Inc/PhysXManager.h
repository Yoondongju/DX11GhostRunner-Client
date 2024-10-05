#pragma once

#include "Base.h"


BEGIN(Engine)

class MyErrorCallback : public PxErrorCallback
{
public:
	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override
	{
		//MSG_BOX(TEXT("Phys 초기화 에러"));
	}
};

END

BEGIN(Engine)

class CPhysXManager final : public CBase
{
public:
	typedef struct
	{
		const PxTriangleMeshGeometry&			MeshGeometry;
		const PxTransform&						PxTransform;

		class CGameObject*						pOnwer = {nullptr};

	}PLAYER_WALKABLE_MESH;


private:
	CPhysXManager();
	virtual ~CPhysXManager() = default; 
	
public:
	PxPhysics*	Get_Physics() { return m_pPhysics; }
	PxScene*	Get_Scene() { return m_pScene; }
	PxMaterial* Get_Material() { return m_pMaterial; }




public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);

	

public:
	void	Clear();
	HRESULT	Add_WalkAble_Mesh(const PLAYER_WALKABLE_MESH& WalkAbleMesh);
	


	_bool CollisionUpdate_PlayerToTriangleMeshGeometry(PxVec3* pOutDir, PxReal* pOutDepth, PxShape* pPlayerShape, PxTransform* pPlayerTransform, class CGameObject** pCollTarget);


private:
	PxFoundation*					m_pFoundation = nullptr;		// 디바이스같은느낌? physX 관렬 객체는 얘가 먼저 생성되어야 사용가능하다.

	PxPhysics*						m_pPhysics = nullptr;			// 물리 시뮬레이션을 수행하는 데 필요한 기능을 제공
	PxScene*						m_pScene = nullptr;				// 물리 시뮬레이션의 특정 장면을 나타내며, 물리 객체들의 상호작용을 처리합니다. 물리 시뮬레이션의 상태를 업데이트하고, 시뮬레이션 결과를 가져오는 역할
	PxMaterial*						m_pMaterial = nullptr;			// 물리 객체의 물리적 성질을 정의하는 재질을 나타냅니다. 마찰력, 반발력 등 물리적 상호작용

	PxDefaultCpuDispatcher*			m_pDispatcher = nullptr;		// 물리 시뮬레이션의 멀티스레딩을 관리 ( 일단 딱히 필요없음 )
	PxPvd*							m_pPvd = nullptr;

	PxPvdTransport*					m_pTransport = { nullptr };


	vector<PLAYER_WALKABLE_MESH>	m_Player_WalkAble_Mesh;	// 플레이어가 걸어다니는 메쉬의 지오메트리


private:
	HRESULT	Ready_PhysX();


	


public:
	static CPhysXManager* Create();
	virtual void Free() override;
};

END