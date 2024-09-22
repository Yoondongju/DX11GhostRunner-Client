#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END


BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{

	}BODY_DESC;



private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

public:
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;
	virtual CModel* Get_Model() override { return m_pModelCom; }

	virtual CCollider* Get_Collider() { return m_pColliderCom; }
	CCollider*		   Get_Collider_OBB() { return m_pColliderCom_OBB; }




	_bool	IsLandingWall() { return m_bLandingWall; }
	_bool	IsClimbing() { return m_bClimbing; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };	    // 피직스
	
private:
	CCollider*		m_pColliderCom_OBB = { nullptr };   // 내가만든 OBB


private:
	CGameObject*	m_pCollisionDestObject = { nullptr };		// 내가 충돌한 옵젝


private: 
	_bool			m_bRotationChange = { false };				// 내 회전 변화가 있었니?

	_float			m_fAccRotationSpeed = {0.f};				// 내 회전누적값 (벽타기용)
	_float			m_fFinalRotation = { 0.f };					// 최종 회전값


	_float			m_fPreLandingDistance = { 0.f };			// 거리 벽타기전 거리 벌리기용
	_bool			m_bLandingWall = { false };					// 벽탔니
	_int			m_iLandWallDir = { 1 };						// 벽을 왼,오 어디서 탔어

private:
	_bool			m_bClimbing = { false };
	_float			m_fCanClimbDistance = { 40.f };


private:
	PxTransform				m_PxTransform = {};
	PxRigidDynamic*			m_pPxRigidDynamic = { nullptr };
	PxShape*				m_pShape = { nullptr };

	PxVec3					m_vDir = {};
	PxReal					m_vDepth = {};



private:
	HRESULT Ready_Components();
	void	Ready_Modify_Animation();
	HRESULT	Ready_PhysX();


private:
	void	ProcessCollision();
	void	PhysXComputeCollision();


	void	ProcessInRange();


	void	LandWall();



public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END