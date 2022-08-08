#include "stdafx.h"
#include "Team.h"
#include "Prefabs/Player/Player.h"
#include "Prefabs/CharacterHUD/CharacterHUD.h"
#include "Prefabs/Orb/Orb.h"
#include "Prefabs/Pedestal/Pedestal.h"

Team::Team(int teamNumber, Orb* pOrb,const std::wstring& teamTexturePath,const std::wstring& teamAuraTexturePath)
	:m_pOrb(pOrb)
	, m_TeamTexturePath(teamTexturePath)
	, m_TeamAuraTexturePath(teamAuraTexturePath)
	, m_TeamNumber(teamNumber)
{
	
}

void Team::AddPlayer(Player* pNewPlayer, const XMFLOAT3& position, bool isActive)
{
	ASSERT_IF_(m_pPlayers.size() == 3);
	pNewPlayer->SetActive(isActive);
	AddChild(static_cast<GameObject*>(pNewPlayer));
	m_pPlayers.emplace_back(pNewPlayer);
	pNewPlayer->GetTransform()->Translate(position);
	m_ResetPositions.emplace_back(position);
}

void Team::SetPedestal(Pedestal* pPedestal, const XMFLOAT3& position)
{
	ASSERT_IF_(m_pPedestal != nullptr);
	AddChild(pPedestal);
	m_pPedestal = pPedestal;
	m_pPedestal->GetTransform()->Translate(position);
}

void Team::SetHud(CharacterHUD* pHUD, const XMFLOAT3& position)
{
	ASSERT_IF_(m_pCharacterHUD != nullptr);
	AddChild(pHUD);
	m_pCharacterHUD = pHUD;
	m_pCharacterHUD->GetTransform()->Translate(position);
	const auto pHealth = m_pCharacterHUD->AddChild(new GameObject());
	if (m_TeamNumber == 1)
	{
		pHealth->AddComponent(new SpriteComponent(L"Textures/Health_blue.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
		pHealth->GetTransform()->Translate(-190, -130, 0);
		pHealth->GetTransform()->Scale(m_pCharacterHUD->GetTransform()->GetScale());
	}
	else if (m_TeamNumber == 2)
	{
		pHealth->AddComponent(new SpriteComponent(L"Textures/Health_red.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
		pHealth->GetTransform()->Translate(+190, -130, 0);
		pHealth->GetTransform()->Scale(m_pCharacterHUD->GetTransform()->GetScale());
	}
}

void Team::PassToNextPlayer()
{
	m_pPlayers[m_ActivePlayer]->SetActive(false);
	m_pPlayers[m_ActivePlayer]->SetHasOrb(false);
	if (m_ActivePlayer == m_pPlayers.size()-1)
	{
		m_ActivePlayer = 0;
	}
	else
	{
		m_ActivePlayer++;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(true);
	m_pPlayers[m_ActivePlayer]->SetHasOrb(true);
	m_pOrb->SetPlayer(m_pPlayers[m_ActivePlayer]);
	m_JustPassed = true;
	m_pCharacterHUD->NextPlayerActivate();
}

void Team::PassToPreviousPlayer()
{
	m_pPlayers[m_ActivePlayer]->SetActive(false);
	m_pPlayers[m_ActivePlayer]->SetHasOrb(false);
	if (m_ActivePlayer == 0)
	{
		m_ActivePlayer = m_pPlayers.size() - 1;
	}
	else
	{
		m_ActivePlayer--;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(true);
	m_pPlayers[m_ActivePlayer]->SetHasOrb(true);
	m_pOrb->SetPlayer(m_pPlayers[m_ActivePlayer]);
	m_JustPassed = true;
	m_pCharacterHUD->PreviousPlayerActivate();
}

void Team::ActiveNextPlayer()
{
	m_pPlayers[m_ActivePlayer]->SetActive(false);
	if (m_ActivePlayer == m_pPlayers.size() - 1)
	{
		m_ActivePlayer = 0;
	}
	else
	{
		m_ActivePlayer++;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(true);
	m_JustPassed = true;
	m_pCharacterHUD->NextPlayerActivate();
}

void Team::ActivePreviousPlayer()
{
	m_pPlayers[m_ActivePlayer]->SetActive(false);
	if (m_ActivePlayer == 0)
	{
		m_ActivePlayer = m_pPlayers.size() - 1;
	}
	else
	{
		m_ActivePlayer--;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(true);
	m_JustPassed = true;
	m_pCharacterHUD->PreviousPlayerActivate();
}

void Team::ResetTeam(Player* pScoredPlayer) 
{
	for (int i{}; i < m_pPlayers.size(); ++i)
	{
		m_pPlayers[i]->GetTransform()->Translate(m_ResetPositions[i]);
		m_pPlayers[i]->SetHasOrb(false);
		m_pPlayers[i]->SetActive(false);
	}

	if (pScoredPlayer->GetParent() == this)
	{
		m_pBanishedPlayer = pScoredPlayer;
		m_pPlayers.erase(std::ranges::remove(m_pPlayers, m_pBanishedPlayer).begin(), m_pPlayers.end());
		RemoveChild(m_pBanishedPlayer);
	}
	m_pPlayers[0]->SetActive(true);
}

const XMFLOAT3& Team::GetActivePlayerPosition() const
{
	return m_pPlayers[m_ActivePlayer]->GetTransform()->GetPosition();
}

void Team::Initialize(const SceneContext& /*sceneContext*/)
{

}

void Team::Update(const SceneContext& /*sceneContext*/)
{

}

void Team::Draw(const SceneContext& /*sceneContext*/)
{

}
