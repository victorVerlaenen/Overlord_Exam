#include "stdafx.h"
#include "CharacterHUD.h"

CharacterHUD::CharacterHUD()
{
	m_PlayerSelectLocations = { {-140.f, 0.f, .9f}, {0.f, 0.f, .9f}, {130.f, 0.f, .9f} };
}

void CharacterHUD::NextPlayerActivate()
{
	if(m_SelectedPlayer == 2)
	{
		m_SelectedPlayer = 0;
		m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[0]);
	}
	else
	{
		m_SelectedPlayer++;
		m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[m_SelectedPlayer]);
	}
}

void CharacterHUD::PreviousPlayerActivate()
{
	if (m_SelectedPlayer == 0)
	{
		m_SelectedPlayer = 2;
		m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[2]);
	}
	else
	{
		m_SelectedPlayer--;
		m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[m_SelectedPlayer]);
	}
}

void CharacterHUD::Initialize(const SceneContext& /*sceneContext*/)
{
	AddComponent(new SpriteComponent(L"Textures/Character_HUD.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	GetTransform()->Scale(.5f, .5f, 1);

	m_pPlayerSelectCircle = AddChild(new GameObject());
	m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[0]);
	m_pPlayerSelectCircle->AddComponent(new SpriteComponent(L"Textures/Aura.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	m_pPlayerSelectCircle->GetTransform()->Scale(.15f,.15f, 1);
}

void CharacterHUD::Update(const SceneContext& /*sceneContext*/)
{

}
