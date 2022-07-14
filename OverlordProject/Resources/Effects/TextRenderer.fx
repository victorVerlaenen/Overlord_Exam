//INCOMPLETE!

float4x4 gTransform : WORLDVIEWPROJECTION;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

BlendState EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

RasterizerState BackCulling
{
	CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
	int Channel : TEXCOORD2; //Texture Channel
	float3 Position : POSITION; //Left-Top Character Quad Starting Position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Left-Top Character Texture Coordinate on Texture
	float2 CharSize: TEXCOORD1; //Size of the character (in screenspace)
};

struct GS_DATA
{
	float4 Position : SV_POSITION; //HOMOGENEOUS clipping space position
	float4 Color: COLOR; //Color of the vertex
	float2 TexCoord: TEXCOORD0; //Texcoord of the vertex
	int Channel : TEXCOORD1; //Channel of the vertex
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
	return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, int channel)
{
	//Create a new GS_DATA object
	GS_DATA geomData = (GS_DATA) 0;
	//Fill in all the fields
	geomData.Position = mul(float4(pos, 1.0f), gTransform);
    geomData.Color = col;
    geomData.TexCoord = texCoord;
	geomData.Channel = channel;
	//Append it to the TriangleStream
	triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Create a Quad using the character information of the given vertex
	//Note that the Vertex.CharSize is in screenspace, TextureCoordinates aren't ;) [Range 0 > 1]
	int channel = vertex[0].Channel;
	float3 position = vertex[0].Position;
	float4 color = vertex[0].Color;
	float2 texCoord = vertex[0].TexCoord;
	float2 charSizeScreenSpace = vertex[0].CharSize;
	float2 charSizeTextureSpace = charSizeScreenSpace / gTextureSize;


	//1. Vertex Left-Top
	float3 vertexPositionLT = position;
	float2 texCoordLT = texCoord;
	CreateVertex(triStream, vertexPositionLT, color, texCoordLT, channel);

	//2. Vertex Right-Top
	float3 vertexPositionRT = vertexPositionLT + float3(charSizeScreenSpace.x, 0, 0);
	float2 texCoordRT = texCoordLT + float2(charSizeTextureSpace.x, 0);
	CreateVertex(triStream, vertexPositionRT, color, texCoordRT, channel);

	//3. Vertex Left-Bottom
	float3 vertexPositionLB = vertexPositionLT + float3(0, charSizeScreenSpace.y, 0);
	float2 texCoordLB = texCoordLT + float2(0, charSizeTextureSpace.y);
	CreateVertex(triStream, vertexPositionLB, color, texCoordLB, channel);

	//4. Vertex Right-Bottom
	float3 vertexPositionRB = vertexPositionLT + float3(charSizeScreenSpace.x, charSizeScreenSpace.y, 0);
	float2 texCoordRB = texCoordLT + float2(charSizeTextureSpace.x, charSizeTextureSpace.y);
	CreateVertex(triStream, vertexPositionRB, color, texCoordRB, channel);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET{

	//Sample the texture and return the correct channel [Vertex.Channel]
	//You can iterate a float4 just like an array, using the index operator
	//Also, don't forget to colorize ;) [Vertex.Color]
	float4 sampledTexture = gSpriteTexture.Sample(samPoint, input.TexCoord)[input.Channel] * input.Color;

	return sampledTexture;
}

// Default Technique
technique10 Default {

	pass p0 {
		SetRasterizerState(BackCulling);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_4_0, MainVS()));
		SetGeometryShader(CompileShader(gs_4_0, MainGS()));
		SetPixelShader(CompileShader(ps_4_0, MainPS()));
	}
}
