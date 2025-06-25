#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;
texture2D g_Texture;


vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
texture2D g_NormalTexture;

texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_LightDepthTexture;
texture2D g_BackTexture;

texture2D g_FinalTexture;
texture2D g_PreFinalTexture;

texture2D g_BlurXTexture;
texture2D g_BlurYTexture;

texture2D g_BloomEffectTexture;
texture2D g_BloomOriginEffectTexture;


vector    g_CamLook;
texture2D g_RefractionTex;
texture2D g_BlockMaskTex;
texture2D g_DistortionTex;


float     g_RefractAmount;
float     g_SplitAmount;
float     g_fUVTime;


vector    g_vCamPosition;


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_WORLD
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPosition : TEXCOORD1;
};


VS_OUT VS_MAIN( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


VS_OUT_WORLD VS_MAIN_WORLD( /*����*/VS_IN In)
{
    VS_OUT_WORLD Out = (VS_OUT_WORLD) 0;

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vWorldPosition = vPosition;
    
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}




/* Triangle : ���� ������ �� vs_main�� ����Ҷ����� ��� */
/* ������ ��δ� ����Ǹ�. ���� ������ ����. */
/* ���ϵ� ������ w�� ������ xyzw�� ������. ���� */
/* ������ ��ġ�� ����Ʈ�� ��ȯ��. (��������ǥ�� ��ȯ�Ѵ�)*/
/* �����Ͷ����� : ���������� ��������Ͽ� �ȼ��� ���������. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;	
};

struct PS_IN_WORLD
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPosition : TEXCOORD1;
};


struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

/* 1. �ȼ��� �������� ������ �����Ѵ�. */
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 4000.f;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));

    vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));

    vector vPosition = (vector) 0;

    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

    vPosition = mul(vPosition, g_ViewMatrixInv);
    vector vLook = vPosition - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);

    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 4000.f;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vPosition = (vector) 0;

    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLightDir = vPosition - g_vLightPos;   

    float fAtt = saturate((g_fLightRange - length(vLightDir)) / g_fLightRange);
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

    vector vReflect = reflect(normalize(vLightDir), normalize(vNormal));
    vector vLook = vPosition - g_vCamPosition;
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f) * 0.5f;

    return Out;
}

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a == 0.f)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vDiffuse * vShade + vSpecular;
    
    
    
    //vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    //float fViewZ = vDepthDesc.y * 4000.f;
    //
	///* 1. ���� �׷����� �ȼ��� ���������� ��ġ�� ��ȯ�ϱ����ؼ� �켱 ����� ��ġȯ�Ͽ� ������ġ�� ���Ѵ�. */
    //vector vPosition = (vector) 0;
    //
	///* ������������ ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�. */
	///* ������ġ * ������� * ����� * ������� / w */
    //vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    //vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    //vPosition.z = vDepthDesc.x;
    //vPosition.w = 1.f;
    //
	///* �佺���̽� ���� ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�.*/
	///* ������ġ * ������� * �����  */
    //vPosition = vPosition * fViewZ;
    //vPosition = mul(vPosition, g_ProjMatrixInv);
    //
	///* ���� ���� ȭ�鿡 �׷����� �ȼ��� ��ġ�� ���Ѵ�.*/
    //vPosition = mul(vPosition, g_ViewMatrixInv);
    //
	///* 2. ������� �ȼ� ��ġ���ٰ� ������������ ������� ������� ���Ͽ� ���������� �����̽��� ��ȯ�Ѵ�. */
    //vector vOldPos = mul(vPosition, g_LightViewMatrix);
    //vOldPos = mul(vOldPos, g_LightProjMatrix);
	//
    //float fLightDepth = vOldPos.w;
    //
    //float2 vTexcoord;
    //vTexcoord.x = (vOldPos.x / vOldPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (vOldPos.y / vOldPos.w) * -0.5f + 0.5f;
    //
    //float fOldLightDepth = g_LightDepthTexture.Sample(LinearSampler, vTexcoord).r * 4000.f;
    //
    //if (fLightDepth - 0.1f > fOldLightDepth)
    //    Out.vColor = vector(Out.vColor.rgb * 0.6f, Out.vColor.a) +  1.f;    

  
    return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vBaseColor = g_BackTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vBaseColor; 
    
    float4 vBloomColor = g_BloomEffectTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor += vBloomColor;

    return Out;

}

float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};


PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord + float2(1.f / 1280.f * i, 0.f);
        Out.vColor += g_fWeights[i + 6] * g_FinalTexture.Sample(LinearSampler, vBlurUV);
		
    }

    Out.vColor /= 6.21f;

    return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord + float2(0.f, 1.f / 720.f * i);
        Out.vColor += g_fWeights[i + 6] * g_BlurXTexture.Sample(LinearSampler, vBlurUV);

    }

    Out.vColor /= 6.21f;

    return Out;
}



PS_OUT PS_MAIN_THRESHOLD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float3 color = g_BloomEffectTexture.Sample(LinearSampler, In.vTexcoord).rgb;
     
    float brightness = dot(color, float3(0.299, 0.587, 0.114));
    if (brightness > 1.0f)
    {
        Out.vColor.rgb = color;
    }
    else
    {
        Out.vColor.rgb = 0; 
    }

    return Out;
}


PS_OUT PS_MAIN_BLOOM_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord + float2(1.f / 1280.f * i, 0.f);
        Out.vColor += g_fWeights[i + 6] * g_BloomEffectTexture.Sample(LinearClampSampler, vBlurUV * 4.f);
    }

    
    Out.vColor /= 2.123f;

    return Out;
}

PS_OUT PS_MAIN_BLOOM_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord + float2(0.f, 1.f / 720.f * i);
        Out.vColor += g_fWeights[i + 6] * g_BlurXTexture.Sample(LinearClampSampler, vBlurUV * 4.f);
    }

    Out.vColor /= 2.123f;

    return Out;
}

PS_OUT PS_MAIN_BLOOM_UPSAMPLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

   
    float2 vUpsampleUV = In.vTexcoord;    
    Out.vColor = g_BlurYTexture.Sample(LinearClampSampler, vUpsampleUV);

    return Out;
}

PS_OUT PS_MAIN_BLOOM_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vBloomColor = g_BloomEffectTexture.Sample(LinearClampSampler, In.vTexcoord); // X, Y ������ �����ø��� 1280,720�� ����Ÿ��
    Out.vColor = g_BloomOriginEffectTexture.Sample(LinearClampSampler, In.vTexcoord); // ���� ���ȿ������ ����Ʈ
    
    
    Out.vColor += vBloomColor * 0.4f;
    
    return Out;
}

PS_OUT PS_MAIN_BLOOM_REALFINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vBaseColor = g_FinalTexture.Sample(LinearClampSampler, In.vTexcoord);
    Out.vColor = vBaseColor;
    
    return Out;
}







PS_OUT PS_MAIN_BLUR_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vBlur = g_BlurYTexture.Sample(LinearSampler, In.vTexcoord);
    vector vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vBlur + vFinal * 0.5f;

    return Out;
}

PS_OUT PS_MAIN_MOTION_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vCurrentColor = g_FinalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vPreviousColor = g_PreFinalTexture.Sample(LinearSampler, In.vTexcoord);

  
    Out.vColor = lerp(vPreviousColor, vCurrentColor, 0.5f); 
    return Out;
}

PS_OUT PS_MAIN_TIMESTOP_REFRACTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float2 NewTexcoord = { In.vTexcoord.x + cos(g_RefractAmount), In.vTexcoord.y + sin(g_RefractAmount) };
    float3 vNormal = g_RefractionTex.Sample(LinearSampler, NewTexcoord).rgb;

    
    // ���� ���� (���� ����)
    float fRefractionIndex = 0.06f;    // 0.3�϶� ���� ��ָʸ�� �״�γ��� 

   // ī�޶� �躤��
    float3 vViewDir = normalize(g_CamLook.xyz);

    // ���� ���� ���
    float3 vRefractionDirection = refract(vViewDir, vNormal, fRefractionIndex);

    // ������ �ؽ�ó ��ǥ ���
    float2 vRefractionTexCoord = saturate(In.vTexcoord + (vRefractionDirection.xy * fRefractionIndex));

    float2 vOffset = abs(vRefractionTexCoord - 0.5) * 2.0;
    float fEdgeAttenuation = saturate(1.0 - dot(vOffset, vOffset));

    // ������ �ؽ�ó ���ø�
    vector vRefraction = g_FinalTexture.Sample(LinearSampler, vRefractionTexCoord);

    // ��迡�� ���� ������ �ٿ��� ����
    Out.vColor = lerp(vFinal, vRefraction, g_RefractAmount * fEdgeAttenuation); // ��迡�� ���� ���� ����
    Out.vColor.b = 0.32f;
    
    return Out;    
}

PS_OUT PS_MAIN_MINDCONTROL_REFRACTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float2 NewTexcoord = { In.vTexcoord.x + cos(g_RefractAmount), In.vTexcoord.y + sin(g_RefractAmount) };
    float3 vNormal = g_RefractionTex.Sample(LinearSampler, NewTexcoord).rgb;

    
    // ���� ���� (���� ����)
    float fRefractionIndex = 0.075f; // 0.3�϶� ���� ��ָʸ�� �״�γ��� 

   // ī�޶� �躤��
    float3 vViewDir = normalize(g_CamLook.xyz);

    // ���� ���� ���
    float3 vRefractionDirection = refract(vViewDir, vNormal, fRefractionIndex);

    // ������ �ؽ�ó ��ǥ ���
    float2 vRefractionTexCoord = saturate(In.vTexcoord + (vRefractionDirection.xy * fRefractionIndex));
    
    float2 vOffset = abs(vRefractionTexCoord - 0.5) * 2.0;
    float fEdgeAttenuation = saturate(1.0 - dot(vOffset, vOffset)); 

    // ������ �ؽ�ó ���ø�
    vector vRefraction = g_FinalTexture.Sample(LinearSampler, vRefractionTexCoord);

    // ��迡�� ���� ������ �ٿ��� ����
    Out.vColor = lerp(vFinal, vRefraction, g_RefractAmount * fEdgeAttenuation); // ��迡�� ���� ���� ����
    Out.vColor.r = 0.44f;
    
    return Out;
}

PS_OUT PS_MAIN_BLOCK_REFRACTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    float2 NewTexcoord = { In.vTexcoord.x + cos(g_RefractAmount), In.vTexcoord.y + sin(g_RefractAmount) };
    float3 vNormal = g_RefractionTex.Sample(LinearSampler, NewTexcoord).rgb;
    
    vector vBlockMask = g_BlockMaskTex.Sample(LinearSampler, NewTexcoord);
    
    
    float fMaskLuminance = dot(vBlockMask.rgb, float3(0.299, 0.587, 0.114)); // ����ġ �ջ� �� ���
    if (fMaskLuminance < 0.35f)
    {
        Out.vColor = vFinal;
        return Out;
    }
    
    
   float fRefractionIndex = 0.08f; // 0.3�϶� ���� ��ָʸ�� �״�γ��� 

   // ī�޶� �躤��
    float3 vViewDir = normalize(g_CamLook.xyz);

    // ���� ���� ���
    float3 vRefractionDirection = refract(vViewDir, vNormal, fRefractionIndex);

    // ������ �ؽ�ó ��ǥ ���
    float2 vRefractionTexCoord = saturate(In.vTexcoord + (vRefractionDirection.xy * fRefractionIndex));
    
    float2 vOffset = abs(vRefractionTexCoord - 0.5) * 2.0;
    float fEdgeAttenuation = saturate(1.0 - dot(vOffset, vOffset));

    // ������ �ؽ�ó ���ø�
    vector vRefraction = g_FinalTexture.Sample(LinearSampler, vRefractionTexCoord);

    // ��迡�� ���� ������ �ٿ��� ����
    Out.vColor = lerp(vFinal, vRefraction, g_RefractAmount * fEdgeAttenuation * fMaskLuminance); // ��迡�� ���� ���� ����
    
    return Out;
}

PS_OUT PS_MAIN_SCREENSPLIT_REFRACTION(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float3 vNormal = g_RefractionTex.Sample(LinearSampler, In.vTexcoord).rgb; 

    float2 newTexCoord = In.vTexcoord;
    if (In.vTexcoord.y > In.vTexcoord.x) //- (1.f / 1280.f))            
    {
        newTexCoord.x += g_SplitAmount * 0.1f * (1.0 + vNormal.rgb);
    }
    else
    {
        newTexCoord.x -= g_SplitAmount * 0.1f * (1.0 + vNormal.rgb);
    }
    
    float noise = frac(sin(dot(In.vTexcoord, float2(12.9898, 78.233))) * 43758.5453 + g_SplitAmount * 10.0); 
    
    float3 color = g_FinalTexture.Sample(LinearClampSampler, newTexCoord).rgb;

    float rShift = noise * g_SplitAmount; 
    float bShift = -noise * g_SplitAmount;

    float3 glitchEffect = float3(
    color.r + rShift, 
    color.g * (0.8 + noise), 
    color.b + bShift);  

    Out.vColor = float4(glitchEffect * (0.9 + 0.1 * noise), 1.0);
    return Out;
}


PS_OUT PS_MAIN_WATERPUDDLE_REFRACTION(PS_IN_WORLD In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DistortionTex.Sample(LinearSampler, In.vTexcoord);
    return Out;
}


PS_OUT PS_MAIN_DISTORTION(PS_IN_WORLD In)
{
    PS_OUT Out = (PS_OUT) 0;

     // ������� ���� �ؽ�ó�� ����� �ؽ�ó ���ø�
    //float4 vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexcoord); // ����� ����
    
    
    float2 NewTexCoord = { In.vTexcoord.x + cos(g_fUVTime * 0.3f), In.vTexcoord.y + sin(g_fUVTime * 0.3f) };
    
    float3 distortionColor = g_DistortionTex.Sample(LinearSampler, NewTexCoord).rgb;
    float2 distortedUV = { 0.f , 0.f };
    
    if (all(distortionColor) == 0.f)
    {
        distortedUV = In.vTexcoord;
    }
    else
    {    
        float2 vDistortion = distortionColor.rg;
        distortedUV = In.vTexcoord + (vDistortion - 0.5f) * 0.05f;
    }
    
  

    Out.vColor = g_FinalTexture.Sample(LinearSampler, distortedUV);
    return Out;
}




technique11 DefaultTechnique
{
    pass Debug       // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Deferred // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

    pass Final // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

    pass Blur_X // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass Blur_Y // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    pass BlurFinal // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_FINAL();
    }


    pass TimeStopRefraction // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TIMESTOP_REFRACTION();
    }

    pass MotionBlur // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOTION_BLUR();
    }

    pass MindControlRefraction // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MINDCONTROL_REFRACTION();
    }



    pass Bloom_Thireshold   // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_THRESHOLD();
    }

    pass Bloom_X // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_BLUR_X();
    }

    pass Bloom_Y // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_BLUR_Y();
    }

    pass Bloom_UpSample // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_UPSAMPLE();
    }

    pass Bloom_Final // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_FINAL();
    }

    pass Bloom_RealFinal // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM_REALFINAL();
    }

    pass BlockRefraction // 17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLOCK_REFRACTION();
    }

    pass ScreenSplit // 18
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SCREENSPLIT_REFRACTION();
    }

    pass WaterPuddle // 19
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_WORLD();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WATERPUDDLE_REFRACTION();
    }

    pass Distortion // 20
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_WORLD();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
    }

}