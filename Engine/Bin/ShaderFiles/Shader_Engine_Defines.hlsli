sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};

sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

sampler LinearClampSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = clamp;
	AddressV = clamp;
};

///////////////////////////////////////////////////////

// Clockwise, CW
// CounterClockwise, CCW

RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;	
};

RasterizerState RS_Wireframe
{
	FillMode = Wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;  
	// direct에서 시계방향을 앞면이라 보고 반시계를 후면이라보는데
	// FrontCounterClockwise가 false라는 말은 반시계를 앞면으로할거니? 라는거니까 false여야하고
	// 앞면이 시계방향이라는 말이고 
	// 내가 제외할 컬 모드가 Back이면 반시계방향으로 그려지는애들을 삭제하겠다 ?
};

RasterizerState RS_Cull_CW
{
	FillMode = solid;
	CullMode = front;
	FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
	FillMode = solid;
	CullMode = none;
	FrontCounterClockwise = false;
};


DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;

	// 깊이테스트 할거니?
	// 깊이를 기록할거니?
	// 현재 렌더링 중인 픽셀의 깊이 값이 깊이 버퍼에 저장된 값보다 작거나 같을 때 해당 픽셀이 화면에 그려지도록 합니다.
	// 화면은 깊이가 0 ~ 1 사이니까 이미 그려진거보다 작거나 같다면 더 가까이 있다는 말이니까 그려줘야지
};

DepthStencilState DSS_None
{
	DepthEnable = false;
	DepthWriteMask = zero;
};


BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;

	/*

	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;

	SrcBlend[1] = one;
	DestBlend[1] = one;
	BlendOp[1] = Add;*/
};

BlendState BS_OnebyOne
{
	BlendEnable[0] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = Add;
};
