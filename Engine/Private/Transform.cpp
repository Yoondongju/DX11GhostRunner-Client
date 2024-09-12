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

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition.m128_f32[0] = vPosition.m128_f32[0] - vCompute_Result.m128_f32[0];		// x
	vPosition.m128_f32[2] = vPosition.m128_f32[2] - vCompute_Result.m128_f32[2];		// z


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

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_RIGHT);



	_vector vCompute_Result = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	vPosition.m128_f32[0] = vPosition.m128_f32[0] - vCompute_Result.m128_f32[0];		// x
	vPosition.m128_f32[2] = vPosition.m128_f32[2] - vCompute_Result.m128_f32[2];		// z


	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Dir(_fvector vDirection, _float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);


	_vector vCompute_Result = XMVector3Normalize(vDirection) * m_fSpeedPerSec * fTimeDelta;


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


void CTransform::LookAt(const _fvector& vAt)
{
	_float3 vScaled = Get_Scaled();

	_vector vPosition = Get_State(STATE_POSITION);



	_vector vLook = vAt - vPosition;
	_vector vRight{}, vUp{};

	


	//_float3 vMyup = Get_State(STATE_UP);
	//D3DXVec3Cross(&vRight, &vMyup, &vLook);// << 여기의 업벡터가 0,1,0이 아니라 본인의 업벡터로 하면?
	// 확인해봐야함 아직해결 X

	_float3 vUpSample = { 0.f, 1.f, 0.f };

	vRight = XMVector3Cross(XMLoadFloat3(&vUpSample), vLook);
	vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
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
