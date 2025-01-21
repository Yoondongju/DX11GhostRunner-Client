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
	// direct���� �ð������ �ո��̶� ���� �ݽð踦 �ĸ��̶󺸴µ�
	// FrontCounterClockwise�� false��� ���� �ݽð踦 �ո������ҰŴ�? ��°Ŵϱ� false�����ϰ�
	// �ո��� �ð�����̶�� ���̰� 
	// ���� ������ �� ��尡 Back�̸� �ݽð�������� �׷����¾ֵ��� �����ϰڴ� ?
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

	// �����׽�Ʈ �ҰŴ�?
	// ���̸� ����ҰŴ�?
	// ���� ������ ���� �ȼ��� ���� ���� ���� ���ۿ� ����� ������ �۰ų� ���� �� �ش� �ȼ��� ȭ�鿡 �׷������� �մϴ�.
	// ȭ���� ���̰� 0 ~ 1 ���̴ϱ� �̹� �׷����ź��� �۰ų� ���ٸ� �� ������ �ִٴ� ���̴ϱ� �׷������
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
