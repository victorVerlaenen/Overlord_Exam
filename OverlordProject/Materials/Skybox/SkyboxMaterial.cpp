#include "stdafx.h"
#include "SkyboxMaterial.h"

SkyboxMaterial::SkyboxMaterial()
	:Material<SkyboxMaterial>(L"Effects/Skybox.fx")
{
}

void SkyboxMaterial::SetSkyboxTexture(const std::wstring& assetFile)
{
	SetVariable_Texture(L"gCubeTexture", ContentManager::Load<TextureData>(assetFile));
}

void SkyboxMaterial::InitializeEffectVariables()
{
	//SetVariable_Texture(L"gCubeTexture", ContentManager::Load<TextureData>(L"Textures/MenuScene/MenuBackground.jpg"));
}