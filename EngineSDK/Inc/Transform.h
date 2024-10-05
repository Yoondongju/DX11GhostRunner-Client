#pragma once
#include "Component.h"

/* 객체의 상태(월드스페이스 상의)를 가지고 있는다. */
/* 상태 : 크기, 회전, 위치 -> WorldMatrix */
/* WorldMatrix를 통해서 월드 공간안에서의 다양한 기능을 구현한다. */
/* WorldMatrix -> 로컬스페이스에 존재하는 정점의 위치를 월드 스페이스 변환하낟. */
/* -> 월드행렬의 각 행(Right, Up, Look, Position)이 월드스페이스에서의 객체정보를 의미한다. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT,STATE_UP,STATE_LOOK,STATE_POSITION, STATE_END };

public:
	typedef struct tagTransform_Desc
	{
		_matrix InitWorldMatrix = { XMMatrixIdentity() };

		_float fSpeedPerSec{};
		_float fRotationPerSec{};

	}TRANSFORM_DESC;


private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& Prototype);
	virtual ~CTransform() = default;


public:
	_vector Get_State(STATE eState)
	{
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float3 Get_Scaled()
	{
		_vector vRight = Get_State(STATE_RIGHT);
		_vector vUp = Get_State(STATE_UP);
		_vector vLook = Get_State(STATE_LOOK);


		return _float3(
			XMVectorGetX(XMVector3Length(vRight)),
			XMVectorGetY(XMVector3Length(vUp)),
			XMVectorGetZ(XMVector3Length(vLook))
		);

	}

	void    Set_WorldMatrix(const _float4x4 & WorldMatrix) { m_WorldMatrix = WorldMatrix; }

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4* Get_WorldMatrix_Ptr() {
		return &m_WorldMatrix;
	}

	_matrix Get_WorldMatrix_Inverse() {
		XMVECTOR determinant;

		return XMMatrixInverse(&determinant, XMLoadFloat4x4(&m_WorldMatrix));
	}

	void Set_State(STATE eState, _fvector vState)
	{
		XMStoreFloat3((_float3*)&m_WorldMatrix.m[eState][0], vState);
	}

	void Set_SpeedPerSec(_float fSpeed) { m_fSpeedPerSec = fSpeed; }

	_float Get_OriginSpeedPerSec() { return m_fOriginSpeed; }
	_float Get_SpeedPerSec() { return m_fSpeedPerSec; }

	_float Get_RotationPerSec() { return m_fRotationPerSec; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char * pConstantName);



public:
	void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);

	void Go_Straight_TargetObject(_float fTimeDelta);

	void Go_Straight(_float fTimeDelta);
	void Go_Straight_All(_float fTimeDelta);

	void Go_Backward(_float fTimeDelta);
	void Go_Backward_All(_float fTimeDelta);

	void Go_Right(_float fTimeDelta);
	void Go_Right_All(_float fTimeDelta);

	void Go_Left(_float fTimeDelta);
	void Go_Left_All(_float fTimeDelta);





	void Go_Dir(_fvector vDirection , _float fTimeDelta);
	void Go_Dir_XZ(_fvector vDirection, _float fTimeDelta);

	void Go_Dir_Wave(_fvector vDirection, _float fTimeDelta , _float fAmplitude, _float fFrequency , _fvector vWaveAxis, _float* pAccTime);		// 파동높이, 파동주기
	void Go_Dir_Curve(_fvector vTargetPos, _fvector vStartPos , _float fTimeDelta);



	void Go_Straight_FreeWalk(_float fTimeDelta);
	void Go_Backward_FreeWalk(_float fTimeDelta);
	void Go_Right_FreeWalk(_float fTimeDelta);
	void Go_Left_FreeWalk(_float fTimeDelta);




	void Turn(const _fvector & vAxis, _float fTimeDelta, _float4x4 * RotationMatrix = nullptr);
	void Turn(const _fmatrix & vRotationMatrix, _float4x4* RotationMatrix = nullptr);

	void Rotation(const _fvector & vAxis, _float fRadian);
	void Rotation(_float fX, _float fY, _float fZ);

	void LookAt(const _fvector & vAt, _float4x4* RotationMatrix);
	void LookAt_Smooth(const _fvector& vAt, _float fTimeDelta, _float4x4* RotationMatrix , _float* pOutAngleGap = nullptr );

	void LookAt_XZ(const _fvector& vAt);
	void LookAt_XZSmooth(const _fvector& vAt, _float fTimeDelta, _float4x4* RotationMatrix);

private:
	_float4x4			m_WorldMatrix = {};

	_float				m_fOriginSpeed = {};
	_float				m_fSpeedPerSec = {};

	_float				m_fRotationPerSec = {};



public:
	static CTransform* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext , void* pArg);
	virtual CComponent* Clone(void* pArg) { return nullptr; };
	virtual void Free() override;

};

END

