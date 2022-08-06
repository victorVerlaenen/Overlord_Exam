float gVignetteRoundness = 7.5f;
float gVignetteOpacity = .5f;
float2 gCurve = float2(10.0f, 10.0f);
Texture2D gTexture;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

RasterizerState BackCulling
{
    CullMode = BACK;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD1;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    output.Position = float4(input.Position, 1.0);
    output.TexCoord = input.TexCoord;

    return output;
}

float2 remapUV(float2 uv)
{
    uv = uv * 2.0f - 1.0f;
    float2 offset = abs(uv.yx) / gCurve;
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
}

float4 vignetteIntensity(float2 uv, float2 resolution, float opacity, float roundness)
{
    float intensity = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);
    float power = saturate(pow((resolution.x / roundness) * intensity, opacity));
    return float4(power, power, power, 1.0);
}

float4 PS(PS_INPUT input) : SV_Target
{
    uint width, height;
    gTexture.GetDimensions(width, height);

    float2 remappedUv = remapUV(input.TexCoord);
    if (remappedUv.x < 0.0f
        || remappedUv.y < 0.0f
        || remappedUv.x > 1.0f
        || remappedUv.y > 1.0f)
    {
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    float4 returnColor = gTexture.Sample(samPoint, remappedUv);

    returnColor *= vignetteIntensity(remappedUv, float2(width, height), gVignetteOpacity, gVignetteRoundness);

    return returnColor;
}

technique11 Default
{
    pass P0
    {
        SetRasterizerState(BackCulling);
        SetDepthStencilState(EnableDepth, 0);
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}