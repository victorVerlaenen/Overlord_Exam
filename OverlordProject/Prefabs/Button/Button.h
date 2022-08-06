#pragma once
class Button : public GameObject
{
public:
	Button(const std::wstring& text, XMFLOAT2 position, XMFLOAT4 color, int actionIdSelect);
	~Button() override = default;
	Button(const Button& other) = delete;
	Button(Button&& other) noexcept = delete;
	Button& operator=(const Button& other) = delete;
	Button& operator=(Button&& other) noexcept = delete;

	void SetPressedFunction(std::function<void()> newPressedFunction);
	void SetSelected(bool value) { m_Selected = value; }

	bool GetSelected() const { return m_Selected; }
protected:
	void Draw(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	SpriteFont* m_pFont{};
	std::function<void()> m_PressedFunction{};

	std::wstring m_Text{};
	XMFLOAT2 m_Position{};
	XMFLOAT4 m_Color{};

	bool m_Selected{ false };

	int m_ActionIdSelect{ -1 };
};

