#pragma once
class CharacterHUD;
class Pedestal;
class Player;

class Team : public GameObject
{
public:
	Team();
	~Team() override = default;

	Team(const Team& other) = delete;
	Team(Team&& other) noexcept = delete;
	Team& operator=(const Team& other) = delete;
	Team& operator=(Team&& other) noexcept = delete;

	void AddPlayer(Player* pNewPlayer);
	void SetPedestal(Pedestal* pPedestal);

	void PassToNextPlayer();
	void PassToPreviousPlayer();
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
	void Draw(const SceneContext&) override;

private:
	std::vector<Player*> m_pPlayers{};
	int m_ActivePlayer{};
	CharacterHUD* m_pCharacterHUD{};
};

