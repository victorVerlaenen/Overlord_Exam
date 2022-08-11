#pragma once
class Orb;
class CharacterHUD;
class Pedestal;
class Player;

class Team : public GameObject
{
public:
	Team(int teamNumber, Orb* pOrb,const std::wstring& teamTexturePath,const std::wstring& teamAuraTexturePath);
	~Team() override;

	Team(const Team& other) = delete;
	Team(Team&& other) noexcept = delete;
	Team& operator=(const Team& other) = delete;
	Team& operator=(Team&& other) noexcept = delete;

	void AddPlayer(Player* pNewPlayer, const XMFLOAT3& position, bool isActive);
	void SetPedestal(Pedestal* pPedestal, const XMFLOAT3& position);
	void SetHud(CharacterHUD* pHUD, const XMFLOAT3& position);
	const std::wstring& GetTexturePath() { return m_TeamTexturePath; }
	const std::wstring& GetAuraTexturePath() { return m_TeamAuraTexturePath; }

	void PassToNextPlayer();
	void PassToPreviousPlayer();
	void ActiveNextPlayer();
	void ActivePreviousPlayer();

	bool GetJustPassed() const { return m_JustPassed; }
	void ResetPass() { m_JustPassed = false; }

	int GetTeamNumber() const { return m_TeamNumber; }

	void ResetTeam(Player* pScoredPlayer = nullptr);
	void Kill(Player* pPlayerToDie);

	XMFLOAT3 GetActivePlayerPosition() const;
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
	void Draw(const SceneContext&) override;

private:
	void UpdateKilledPlayers(const SceneContext& sceneContext);
	void SetToNextFreePosition(Player* pPlayer);
	void FreeRespawnPosition(Player* pPlayer);

	std::vector<Player*> m_pPlayers{};
	std::vector<std::pair<Player*, float>> m_pKilledPlayers{};
	Player* m_pBanishedPlayer{nullptr};
	Player* m_pNewBanishedPlayer{nullptr};
	Player* m_pNewKilledPlayer{nullptr};
	size_t m_ActivePlayer{0};
	XMFLOAT3 m_ActivePlayerLocation{};

	std::vector<std::pair<XMFLOAT3, Player*>> m_ResetPositions{};

	FMOD::Sound* m_pDeadSound{};
	FMOD::ChannelGroup* m_pSoundsGroup;

	Pedestal* m_pPedestal{};
	CharacterHUD* m_pCharacterHUD{};
	Orb* m_pOrb{};
	std::wstring m_TeamTexturePath{};
	std::wstring m_TeamAuraTexturePath{};

	bool m_JustPassed{ false };
	bool m_PlayerHasToRejoin{ false };

	int m_TeamNumber{};
	float m_DeadTimer{};
};


