#pragma once


#include "Client_Defines.h"
#include "GameObject.h"




BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END


BEGIN(Client)

class CStatic_Object final : public CGameObject
{
private:
	CStatic_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatic_Object(const CStatic_Object& Prototype);
	virtual ~CStatic_Object() = default;


public:
	class CModel* Get_Model() override { return m_pModel; }

	_bool	IsCanLandWall() { return m_bLandWall; }
	_bool	IsCanClimbing() { return m_bClimbing; }

public:
	const PxRigidStatic* Get_RigidStatic() const { return m_pPxRigidStatic; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel*  m_pModel = { nullptr };


private:
	PxTransform							m_PxTransform = {};
	PxRigidStatic*						m_pPxRigidStatic = { nullptr };		
	PxShape*							m_pShape = { nullptr };
	vector<PxTriangleMeshGeometry>		m_MeshGeometry;

	_bool								m_bLandWall = { false };	// ���� Ż���ִ�?
	_bool								m_bClimbing = { false };    // Ŭ���̹� �Ҽ��ִ�
	
	_bool								m_isChangeTexCoord = { false };		// �ؽ���带 �ٲ���ϴ� ������Ʈ��

	_float								m_fTextCoordTime = {};


private:
	HRESULT		Ready_Component();
	HRESULT		Ready_HandleModelTypeTasks();	// �� Ÿ�Կ� �°� ������ �ֳ� üũ


	HRESULT		Ready_PhysX();

public:
	static CStatic_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END