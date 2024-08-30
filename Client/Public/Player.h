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
		SH_KAT_TO_SHUR,				// 카타나에서 수리켄
		SH_SHUR_TO_KAT,				// 수리켄에서 카타나
		SCAN_START,
		SCAN_END,
		DEATH_1,
		DEATH_2,
		JUMP_START,
		JUMP_LOOP,
		JUMP_END,
		FURR_AIM_LOOP,				// 정신집중
		FURR_AIM_TO_IDLE,
		FURR_IDLE_TO_AIM,
		FURR_AIM_TRICUT,			// 정신집중하고 슥삭
		NAMI_AIM_ATTACK_TO_IDLE,
		NAMI_IDLE_TO_AIM,			// 왼손 쫙 펴는거 
		SH_ZIPLINE_ATTACK,
		SH_ZIPLINE_END,				// 탄창 갈기 애니메이션  
		SH_ZIPLINE_LOOP,			// 탄창 갈기 애니메이션  
		SH_ZIPLINE_START,			// 탄창 갈기 애니메이션  

		PLAYER_ANIMATION_END
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	class CFsm*			Get_Fsm() override { return m_pFsm; }
	class CRigidBody*   Get_RigidBody() override { return m_pRigidBody; }

	PLAYER_ANIMATIONID	Get_CurAnimationID() { return m_eCurAnimationID; }

	const _float4x4&	Get_RotationMatrix() { return m_RotationMatrix; }


public:
	_float				Get_OffsetY() { return m_fOffsetY; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CFsm*					m_pFsm = { nullptr };
	CRigidBody*				m_pRigidBody = { nullptr };

	PLAYER_ANIMATIONID		m_eCurAnimationID = { PLAYER_ANIMATION_END };

	POINT					m_ptOldMousePos = {};
	_float4x4				m_RotationMatrix = {};


private:
	_float					m_fOffsetY = { 20.f };


private:
	HRESULT		Ready_Component();
	HRESULT		Ready_PartObjects();

	HRESULT		Ready_State();






public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END