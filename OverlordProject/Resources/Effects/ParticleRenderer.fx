float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gViewInverse : ViewInverse;
Texture2D gParticleTexture;

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

//STATES
//******
BlendState AlphaBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0f;
};

DepthStencilState DisableDepthWriting
{
	//Enable Depth Rendering
	DepthEnable = true;
	//Disable Depth Writing
	DepthWriteMask = 0;
};

RasterizerState BackCulling
{
	CullMode = BACK;
};


//SHADER STRUCTS
//**************
struct VS_DATA
{
	float3 Position : POSITION;
	float4 Color: COLOR;
	float Size : TEXCOORD0;
	float Rotation : TEXCOORD1;
};

struct GS_DATA
{
	float4 Position : SV_POSITION;
	float2 TexCoord: TEXCOORD0;
	float2 TexCoordRot: TEXCOORD1;
	float4 Color : COLOR;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float2 texCoord, float4 col, float2x2 uvRotation)
{
	//Step 1. Create a GS_DATA object
	GS_DATA output = (GS_DATA)0;
	//Step 2. Transform the position using the WVP Matrix and assign it to (GS_DATA object).Position (Keep in mind: float3 -> float4, Homogeneous Coordinates)
	output.Position = mul(float4(pos, 1.0f), gWorldViewProj);
	//Step 3. Assign texCoord to (GS_DATA object).TexCoord
	
	//Fixed the rotation artifacts (only works for circular textures)
	float2 uv2 = texCoord - float2(0.5f, 0.5f);
	uv2 = mul(uv2, uvRotation);
	uv2 += float2(0.5f, 0.5f);
	output.TexCoord = texCoord;
	output.TexCoordRot = uv2;
	//Step 4. Assign color to (GS_DATA object).Color
	output.Color = col;
	//Step 5. Append (GS_DATA object) to the TriangleStream parameter (TriangleStream::Append(...))
	triStream.Append(output);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Use these variable names
	float3 topLeft, topRight, bottomLeft, bottomRight;
	float size = vertex[0].Size;
	float3 origin = vertex[0].Position;

	//This is the 2x2 rotation matrix we need to transform our TextureCoordinates (Texture Rotation)
	float3x3 uvRotation = { cos(vertex[0].Rotation), -sin(vertex[0].Rotation), 0, sin(vertex[0].Rotation), cos(vertex[0].Rotation), 0, 0, 0, 1 };

	//Vertices
	topLeft = mul(float3(-size, size, 0), uvRotation);
	topRight = mul(float3(size, size, 0), uvRotation);
	bottomLeft = mul(float3(-size, -size, 0), uvRotation);
	bottomRight = mul(float3(size, -size, 0), uvRotation);

	//Transform the vertices using the ViewInverse (Rotational Part Only!!! (~ normal transformation)), this will force them to always point towards the camera (cfr. BillBoarding)
	topLeft = mul(topLeft, (float3x3)gViewInverse);
	topRight = mul(topRight, (float3x3)gViewInverse);
	bottomLeft = mul(bottomLeft, (float3x3)gViewInverse);
	bottomRight = mul(bottomRight, (float3x3)gViewInverse);

	//Create Geometry (Trianglestrip) (Keep in mind that 'origin' contains the center of the quad)
	CreateVertex(triStream, bottomLeft + origin, float2(0, 1), vertex[0].Color, uvRotation);
	CreateVertex(triStream, topLeft + origin, float2(0, 0), vertex[0].Color, uvRotation);
	CreateVertex(triStream, bottomRight + origin, float2(1, 1), vertex[0].Color, uvRotation);
	CreateVertex(triStream, topRight + origin, float2(1, 0), vertex[0].Color, uvRotation);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET{

	//Simple Texture Sampling
	float2 uv = input.TexCoord;
	float2 uvRot = input.TexCoordRot;

	float4 colOriginal = gParticleTexture.Sample(samPoint, uv);
	float4 colRotated = gParticleTexture.Sample(samPoint, uvRot);

	float inCircle = .5f - distance(input.TexCoord.xy, float2(.5f, .5f));

	colOriginal *= 1 - ceil(inCircle);
	colRotated *= ceil(inCircle);

	//float4 result = gParticleTexture.Sample(samPoint,input.TexCoord);
	float4 result = colOriginal + colRotated;
	return input.Color * result;
}

// Default Technique
technique10 Default {

	pass p0 {
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));

		SetRasterizerState(BackCulling);
		SetDepthStencilState(DisableDepthWriting, 0);
		SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}
