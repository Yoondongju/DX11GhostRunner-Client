#pragma once

#include "Base.h"


BEGIN(Engine)

class MyErrorCallback : public PxErrorCallback
{
public:
	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override
	{
		//MSG_BOX(TEXT("Phys �ʱ�ȭ ����"));
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
	PxFoundation*					m_pFoundation = nullptr;		// ����̽���������? physX ���� ��ü�� �갡 ���� �����Ǿ�� ��밡���ϴ�.

	PxPhysics*						m_pPhysics = nullptr;			// ���� �ùķ��̼��� �����ϴ� �� �ʿ��� ����� ����
	PxScene*						m_pScene = nullptr;				// ���� �ùķ��̼��� Ư�� ����� ��Ÿ����, ���� ��ü���� ��ȣ�ۿ��� ó���մϴ�. ���� �ùķ��̼��� ���¸� ������Ʈ�ϰ�, �ùķ��̼� ����� �������� ����
	PxMaterial*						m_pMaterial = nullptr;			// ���� ��ü�� ������ ������ �����ϴ� ������ ��Ÿ���ϴ�. ������, �ݹ߷� �� ������ ��ȣ�ۿ�

	PxDefaultCpuDispatcher*			m_pDispatcher = nullptr;		// ���� �ùķ��̼��� ��Ƽ�������� ���� ( �ϴ� ���� �ʿ���� )
	PxPvd*							m_pPvd = nullptr;

	PxPvdTransport*					m_pTransport = { nullptr };


	vector<PLAYER_WALKABLE_MESH>	m_Player_WalkAble_Mesh;	// �÷��̾ �ɾ�ٴϴ� �޽��� ������Ʈ��


private:
	HRESULT	Ready_PhysX();


	


public:
	static CPhysXManager* Create();
	virtual void Free() override;
};

END