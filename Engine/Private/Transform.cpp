#include "../Public/Transform.h"
#include "../Public/Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& Prototype)
	: CComponent(Prototype)
	, m_WorldMatrix(Prototype.m_WorldMatrix)
{

}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	XMStoreFloat4x4(&m_WorldMatrix, pDesc->InitWorldMatrix);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fOriginSpeed = m_fSpeedPerSec;

	m_fRotationPerSec = pDesc->fRotationPerSec;


	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}



void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fScaleZ);
}

void CTransform::Go_Straight_TargetObject(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fOriginSpeed * fTimeDelta;

	vPosition += vCompute_Result;


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	

	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition.m128_f32[0] = vPosition.m128_f32[0] + vCompute_Result.m128_f32[0];		// x
	vPosition.m128_f32[2] = vPosition.m128_f32[2] + vCompute_Result.m128_f32[2];		// z


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Straight_All(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition += vCompute_Result;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition.m128_f32[0] = vPosition.m128_f32[0] - vCompute_Result.m128_f32[0];		// x
	vPosition.m128_f32[2] = vPosition.m128_f32[2] - vCompute_Result.m128_f32[2];		// z


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward_All(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition -= vCompute_Result;


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_RIGHT);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition.m128_f32[0] = vPosition.m128_f32[0] + vCompute_Result.m128_f32[0];		// x
	vPosition.m128_f32[2] = vPosition.m128_f32[2] + vCompute_Result.m128_f32[2];		// z


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right_All(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_RIGHT);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition += vCompute_Result;


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_RIGHT);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition.m128_f32[0] = vPosition.m128_f32[0] - vCompute_Result.m128_f32[0];		// x
	vPosition.m128_f32[2] = vPosition.m128_f32[2] - vCompute_Result.m128_f32[2];		// z


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left_All(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_RIGHT);


	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition -= vCompute_Result;

	Set_State(STATE_POSITION, vPosition);

}

void CTransform::Go_Dir(_fvector vDirection, _float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);


	_vector vCompute_Result = XMVector3Normalize(vDirection) * m_fSpeedPerSec * fTimeDelta;


	vPosition += vCompute_Result;

	Set_State(STATE_POSITION, vPosition);
}


void CTransform::Go_Dir_XZ(_fvector vDirection, _float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);


	_vector vCompute_Result = XMVector3Normalize(vDirection) * m_fSpeedPerSec * fTimeDelta;


	vPosition.m128_f32[0] += vCompute_Result.m128_f32[0];
	vPosition.m128_f32[2] += vCompute_Result.m128_f32[2];

	Set_State(STATE_POSITION, vPosition);
}


void CTransform::Go_Dir_Wave(_fvector vDirection, _float fTimeDelta , _float fAmplitude , _float fFrequency ,  _fvector vWaveAxis,_float* pAccTime)
{
	_vector vPosition = Get_State(STATE_POSITION);


	_vector vCompute_Result = XMVector3Normalize(vDirection) * m_fSpeedPerSec * fTimeDelta;


	vPosition += vCompute_Result;


	// 시간을 기반으로 수직으로 파동 추가 (sin 함수 사용)
	_float fWaveOffset = fAmplitude * sinf(fFrequency * (*pAccTime));

	
	vPosition += XMVector3Normalize(vWaveAxis) * fWaveOffset;


	Set_State(STATE_POSITION, vPosition);

	*pAccTime += fTimeDelta;
}


void CTransform::Go_Dir_Curve(_fvector vTargetPos, _fvector vStartPos ,_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vDirection = XMVectorSubtract(vTargetPos, vPosition);
	

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRightNor = XMVector3Normalize(XMVector3Cross(vUp, vDirection));





	_vector vCompute_Result = {};
	vCompute_Result += XMVector3Normalize(vDirection)* m_fSpeedPerSec * fTimeDelta;


	_float fCurveFactor = sinf(fTimeDelta * 2.f);
	vCompute_Result += vRightNor * fCurveFactor * m_fSpeedPerSec * fTimeDelta;


	vPosition += vCompute_Result;
	

	Set_State(STATE_POSITION, vPosition);
}


void CTransform::Go_Straight_FreeWalk(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * 2;

	vPosition += vCompute_Result;
	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward_FreeWalk(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * 2;

	vPosition -= vCompute_Result;


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right_FreeWalk(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_RIGHT);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * 2;

	vPosition += vCompute_Result;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left_FreeWalk(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_RIGHT);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * 2;

	vPosition -= vCompute_Result;


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Turn(const _fvector& vAxis, _float fTimeDelta, _float4x4* RotationMatrix)
{
	// 이건 나의 월드좌표에서의 정보  현재 내 월드좌표의 위치에서 턴
	_vector vRight = Get_State(STATE_RIGHT);				// 지가 지금 잇는 월드행렬 성분을 토대로
	_vector	vUp = Get_State(STATE_UP);
	_vector	vLook = Get_State(STATE_LOOK);


	// 프레임에서의 회전 업데이트
	_matrix vRotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	// 누적 회전 행렬 계산
	if (RotationMatrix)
		XMStoreFloat4x4(RotationMatrix, XMMatrixMultiply(XMLoadFloat4x4(RotationMatrix), vRotationMatrix));



	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, vRotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, vRotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, vRotationMatrix));
}


void CTransform::Turn(const _fmatrix& vRotationMatrix, _float4x4* RotationMatrix)
{
	_vector vRight = Get_State(STATE_RIGHT);				// 지가 지금 잇는 월드행렬 성분을 토대로
	_vector	vUp = Get_State(STATE_UP);
	_vector	vLook = Get_State(STATE_LOOK);


	if (RotationMatrix)
		XMStoreFloat4x4(RotationMatrix, XMMatrixMultiply(XMLoadFloat4x4(RotationMatrix), vRotationMatrix));


	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, vRotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, vRotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, vRotationMatrix));
}



void CTransform::Rotation(const _fvector& vAxis, _float fRadian)	// 1회용이네 자기가 회전되기 이전 처음 시점에만
{
	// 항등에서 지정한각도만큼 돌기
	_float3 vScaled = Get_Scaled();

	_vector vRight = _vector{ 1.f,0.f,0.f };
	_vector vUp = _vector{ 0.0f,1.f,0.f };
	_vector vLook = _vector{ 0.0f,0.f,1.f };



	_float4x4 RotationMatrix{};

	_matrix vRotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	// 회전은 내가 스케일을 받기전 상태 1.f,1.f,1.f에서 회전시키고 사이즈늘려

	XMVector3TransformNormal(vRight, vRotationMatrix);
	XMVector3TransformNormal(vUp, vRotationMatrix);
	XMVector3TransformNormal(vLook, vRotationMatrix);


	Set_State(STATE_RIGHT, vRight * vScaled.x);
	Set_State(STATE_UP, vUp * vScaled.y);
	Set_State(STATE_LOOK, vLook * vScaled.z);
}


void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}


void CTransform::LookAt(const _fvector& vAt ,_float4x4* RotationMatrix)
{
	_float3 vScaled = Get_Scaled();

	_vector vPosition = Get_State(STATE_POSITION);



	_vector vLook = vAt - vPosition;
	_vector vRight{}, vUp{};


	_float3 vUpSample = { 0.f, 1.f, 0.f };

	vRight = XMVector3Cross(XMLoadFloat3(&vUpSample), vLook);
	vUp = XMVector3Cross(vLook, vRight);


	vRight = XMVector3Normalize(vRight);
	vUp = XMVector3Normalize(vUp);
	vLook = XMVector3Normalize(vLook);


	Set_State(STATE_RIGHT, vRight * vScaled.x);
	Set_State(STATE_UP, vUp * vScaled.y);
	Set_State(STATE_LOOK, vLook * vScaled.z);

	if (nullptr != RotationMatrix)
	{
		_matrix mRotationMatrix = XMMatrixIdentity();

		// RotationMatrix는 회전 행렬의 성분이므로 Right, Up, Look을 행렬의 각 열에 설정
		mRotationMatrix.r[0] = XMVectorSetW(vRight, 0.f);  // Right 벡터
		mRotationMatrix.r[1] = XMVectorSetW(vUp, 0.f);     // Up 벡터
		mRotationMatrix.r[2] = XMVectorSetW(vLook, 0.f);   // Look 벡터

		// 누적 회전 행렬로 저장
		XMStoreFloat4x4(RotationMatrix, mRotationMatrix);
	}

}

void CTransform::LookAt_Smooth(const _fvector& vAt, _float fTimeDelta ,_float4x4* RotationMatrix ,_float* pOutAngleGap)
{
	_float3 vScaled = Get_Scaled();


	_vector vPosition = Get_State(STATE_POSITION);
	_vector vTargetLook = vAt - vPosition;

	// 현재 플레이어의 Look 벡터
	_vector vCurrentLook = Get_State(STATE_LOOK);

	// 벡터 정규화
	vTargetLook = XMVector3Normalize(vTargetLook);
	vCurrentLook = XMVector3Normalize(vCurrentLook);

	// 현재 Look 벡터와 목표 Look 벡터 사이의 회전 각도 계산
	_float fDot = XMVectorGetX(XMVector3Dot(vCurrentLook, vTargetLook));
	_float fAngle = acosf(fDot); // 각도 (라디안)

	if(nullptr != pOutAngleGap)
		*pOutAngleGap = XMConvertToDegrees(fAngle);		// 각도 차이얼마 나니  디그리로 ?
	
	_float fRotationSpeed = 5.0f; // 기본 회전 속도
	_float fRotationFactor = fAngle / XM_PIDIV2;  
	
	_float fRotationAmount = fRotationSpeed * fRotationFactor * fTimeDelta; // 각도에 따라 조절된 회전 양

	if (fRotationAmount > fAngle)	// 목표까지의 각도보다 회전 양이 크면
	{
		fRotationAmount = fAngle; 
	}


	// 회전 축 계산 (현재 Look 벡터와 목표 Look 벡터의 외적)
	_vector vRotationAxis = XMVector3Cross(vCurrentLook, vTargetLook);

	// 회전 축이 0인 경우는 이미 타겟을 바라보고 있는 경우 (회전 불필요)
	if (XMVector3Equal(vRotationAxis, XMVectorZero())) {
		return;
	}

	// 회전 행렬 생성
	_matrix vRotationMatrix = XMMatrixRotationAxis(XMVector3Normalize(vRotationAxis), fRotationAmount);

	// 새로운 Look 벡터 계산
	_vector vNewLook = XMVector3TransformNormal(vCurrentLook, vRotationMatrix);

	// Right 벡터 및 Up 벡터 재계산
	_float3 vUpSample = { 0.f, 1.f, 0.f };
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vUpSample), vNewLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

	// 스케일을 고려하여 설정
	Set_State(STATE_RIGHT, vRight * vScaled.x);
	Set_State(STATE_UP, vUp * vScaled.y);
	Set_State(STATE_LOOK, vNewLook * vScaled.z);

	// 회전 행렬을 갱신하는 경우
	if (RotationMatrix)
	{
		_matrix mRotationMatrix = XMMatrixIdentity();
		mRotationMatrix.r[0] = XMVectorSetW(vRight, 0.f);  // Right 벡터
		mRotationMatrix.r[1] = XMVectorSetW(vUp, 0.f);     // Up 벡터
		mRotationMatrix.r[2] = XMVectorSetW(vNewLook, 0.f);   // Look 벡터
		XMStoreFloat4x4(RotationMatrix, mRotationMatrix);
	}
}

void CTransform::LookAt_XZ(const _fvector& vAt)
{
	_float3 vScaled = Get_Scaled();

	_vector vPosition = Get_State(STATE_POSITION);

	_vector vLook = vAt - vPosition;
	_vector vRight{}, vUp{};


	vLook = XMVectorSetY(vLook, 0.f);

	_float3 vUpSample = { 0.f, 1.f, 0.f };

	vRight = XMVector3Cross(XMLoadFloat3(&vUpSample), vLook);
	vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::LookAt_XZSmooth(const _fvector& vAt, _float fTimeDelta, _float4x4* RotationMatrix)
{
	_float3 vScaled = Get_Scaled();


	_vector vPosition = Get_State(STATE_POSITION);
	_vector vTargetLook = vAt - vPosition;
	vTargetLook = XMVectorSetY(vTargetLook, 0.0f);

	
	_vector vCurrentLook = Get_State(STATE_LOOK);
	vCurrentLook = XMVectorSetY(vCurrentLook, 0.0f);


	vTargetLook = XMVector3Normalize(vTargetLook);
	vCurrentLook = XMVector3Normalize(vCurrentLook);

	// 현재 Look 벡터와 목표 Look 벡터 사이의 회전 각도 계산
	_float fDot = XMVectorGetX(XMVector3Dot(vCurrentLook, vTargetLook));
	_float fAngle = acosf(fDot); // 각도 (라디안)


	_float fRotationSpeed = 5.0f; // 기본 회전 속도
	_float fRotationFactor = fAngle / XM_PIDIV2;  // 90도(XM_PIDIV2)에서 최대 속도, 0도에서 최소 속도

	_float fRotationAmount = fRotationSpeed * fRotationFactor * fTimeDelta; // 각도에 따라 조절된 회전 양

	if (fRotationAmount > fAngle) // 목표까지의 각도보다 회전 양이 크면
	{
		fRotationAmount = fAngle;
	}


	// 회전 축 계산 (현재 Look 벡터와 목표 Look 벡터의 외적)
	_vector vRotationAxis = XMVector3Cross(vCurrentLook, vTargetLook);

	// 회전 축이 0인 경우는 이미 타겟을 바라보고 있는 경우 (회전 불필요)
	if (XMVector3Equal(vRotationAxis, XMVectorZero())) {
		return;
	}

	// 회전 행렬 생성
	_matrix vRotationMatrix = XMMatrixRotationAxis(XMVector3Normalize(vRotationAxis), fRotationAmount);

	// 새로운 Look 벡터 계산
	_vector vNewLook = XMVector3TransformNormal(vCurrentLook, vRotationMatrix);


	// Right 벡터 및 Up 벡터 재계산
	_float3 vUpSample = { 0.f, 1.f, 0.f };
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vUpSample), vNewLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

	// 스케일을 고려하여 설정
	Set_State(STATE_RIGHT, vRight * vScaled.x);
	Set_State(STATE_UP, vUp * vScaled.y);
	Set_State(STATE_LOOK, vNewLook * vScaled.z);

	// 회전 행렬을 갱신하는 경우
	if (RotationMatrix)
	{
		_matrix mRotationMatrix = XMMatrixIdentity();
		mRotationMatrix.r[0] = XMVectorSetW(vRight, 0.f);	  // Right 벡터
		mRotationMatrix.r[1] = XMVectorSetW(vUp, 0.f);		  // Up 벡터
		mRotationMatrix.r[2] = XMVectorSetW(vNewLook, 0.f);   // Look 벡터
		XMStoreFloat4x4(RotationMatrix, mRotationMatrix);
	}
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTransform::Free()
{
	__super::Free();


}
