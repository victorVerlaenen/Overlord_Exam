#pragma once

class Player;

class CharacterHUD : public GameObject
{
public:
	CharacterHUD(std::vector<Player*> pTeam);
	~CharacterHUD() override;

	CharacterHUD(const CharacterHUD& other) = delete;
	CharacterHUD(CharacterHUD&& other) noexcept = delete;
	CharacterHUD& operator=(const CharacterHUD& other) = delete;
	CharacterHUD& operator=(CharacterHUD&& other) noexcept = delete;

	void NextPlayerActivate(const std::vector<Player*>& pStillActivePlayers);
	void PreviousPlayerActivate(const std::vector<Player*>& pStillActivePlayers);
	void SetActivePlayer(Player* pActivePlayer);

	void ShowDeadTimer(const std::pair<Player*, float>& pKilledPlayer);
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
	void Draw(const SceneContext&) override;

private:
	std::vector<XMFLOAT3> m_PlayerSelectLocations{};
	std::vector<std::pair<XMFLOAT3, float>> m_pRespawnTimers{};
	SpriteFont* m_pFont{};
	const std::vector<Player*> m_pTeam;
	GameObject* m_pPlayerSelectCircle{};
	int m_SelectedPlayer{ 0 };
};

