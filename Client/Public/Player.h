#pragma once


#include "Client_Defines.h"
#include "ContainerObject.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CFsm;
class CRigidBody;
END


BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	enum PARTID { PART_BODY, PART_WEAPON, PART_END };
	enum PLAYER_ANIMATIONID
	{
		CLIMB,
		DASH_B,
		DASH_L,
		DASH_R,
		BLOCK_1,
		BLOCK_2,
		BLOCK_3,
		HOOK_UP,
		HOOK_DOWN,
		ATTACK_1,
		ATTACK_2,
		ATTACK_3,
		DASH_F,
		IDLE,
		RUN,
		RUN_WALL_L,
		RUN_WALL_R,
		SLIDE,
		WALK,
		SH_ATTACK,
		SH_BLOCK,
		SH_DASH_B,
		SH_DASH_F,
		SH_DASH_L,
		SH_DASH_R,
		SH_IDLE,
		SH_RUN,
		SH_WALK,
		SH_RUN_WALL_L,
		SH_RUN_WALL_R,
		SH_KAT_TO_SHUR,				// īŸ������ ������
		SH_SHUR_TO_KAT,				// �����˿��� īŸ��
		SCAN_START,
		SCAN_END,
		DEATH_1,
		DEATH_2,
		JUMP_START,
		JUMP_LOOP,
		JUMP_END,
		FURR_AIM_LOOP,				// ��������
		FURR_AIM_TO_IDLE,
		FURR_IDLE_TO_AIM,
		FURR_AIM_TRICUT,			// ���������ϰ� ����
		NAMI_AIM_ATTACK_TO_IDLE,
		NAMI_IDLE_TO_AIM,			// �޼� �� ��°� 
		SH_ZIPLINE_ATTACK,
		SH_ZIPLINE_END,				// źâ ���� �ִϸ��̼�  
		SH_ZIPLINE_LOOP,			// źâ ���� �ִϸ��̼�  
		SH_ZIPLINE_START,			// źâ ���� �ִϸ��̼�  

		PLAYER_ANIMATION_END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	class CFsm* Get_Fsm() override { return m_pFsm; }
	class CRigidBody* Get_RigidBody() override { return m_pRigidBody; }

	PLAYER_ANIMATIONID	Get_CurAnimationID() { return m_eCurAnimationID; }

	const _float4x4& Get_RotationMatrix() { return m_RotationMatrix; }
	_float4x4*		 Get_RotationMatrixPtr() { return &m_RotationMatrix; }


public:
	_float				Get_OffsetY() { return m_fOffsetY; }

	_float				Get_LandPosY() { return m_fLandPosY; }
	void				Set_LandPosY(_float fLandY) { m_fLandPosY = fLandY; }

	_bool				Get_Shaking() { return m_bShaking; }

	void				Set_JumpProgressTime(_float fTime) { m_fJumpProgressTime = fTime; }
	_float				Get_JumpProgressTime() { return m_fJumpProgressTime; }


	class CGrapplingPointUI*		Get_GrapplingPoint()  { return m_pGrapplingPoint; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


	void    Set_Shake(_uint iShakeCount, _float fForce, _fvector vAsix)
	{
		if (iShakeCount % 2 != 0)
		{
			iShakeCount += 1;
		}
		else if (iShakeCount == 0)
			iShakeCount = 2;


		m_iShakingCount = iShakeCount;
		m_iOriginShakingCount = m_iShakingCount;

		m_bShaking = true;
		m_fForce = fForce;
		XMStoreFloat3(&m_ShakeAsix, vAsix);
	}
	void	Active_Shake(_float fForce, _fvector vAsix);



private:
	CFsm*		m_pFsm = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

	PLAYER_ANIMATIONID		m_eCurAnimationID = { PLAYER_ANIMATION_END };

	POINT					m_ptOldMousePos = {};


	_float4x4				m_RotationMatrix = {};


private:
	_float					m_fOffsetY = { 20.f };
	_float					m_fLandPosY = { 0.f };		// ���� �����ؾ��� Y��ġ�� ����? �޽��� �� �߾Ʒ������� �޽��� Y��ġ�� ������ 0 



private:
	_bool				m_bShaking = false;
	_uint				m_iOriginShakingCount = { 0 };
	_int				m_iShakingCount = 0;

	_float				m_fForce = { 0.f };			// ����ŷ���� ����� ȸ������ ����
	_float				m_fCurRotation = { 0.f };	// ����ŷ���� ����� ȸ����
	_float				m_fPreRotation = { 0.f };	// ����ŷ���� ����� ȸ����

	_float3				m_ShakeAsix = {};

private:
	_float				m_fJumpProgressTime = { 0.f };	// ���� ����ð�


private:
	class CGrapplingPointUI*		m_pGrapplingPoint = { nullptr };


	_bool							m_bFreeWalk = { false };


private:
	HRESULT		Ready_Component();

	HRESULT		Ready_PartObjects();

	HRESULT		Ready_State();

	HRESULT		Ready_PlayerUI();




public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END