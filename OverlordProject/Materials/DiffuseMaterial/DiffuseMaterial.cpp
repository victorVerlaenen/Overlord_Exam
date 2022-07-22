#include "stdafx.h"
#include "DiffuseMaterial.h"

DiffuseMaterial::DiffuseMaterial()
	:Material<DiffuseMaterial>(L"Effects/PosNormTex3D.fx")
{

}

void DiffuseMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	SetVariable_Texture(L"gDiffuseMap", ContentManager::Load<TextureData>(assetFile));
}

void DiffuseMaterial::InitializeEffectVariables()
{

}
