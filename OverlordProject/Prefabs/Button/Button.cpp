#include "stdafx.h"
#include "Button.h"

Button::Button(const std::wstring& text, XMFLOAT2 position, XMFLOAT4 color, XMFLOAT2 bounds, int actionIdSelect)
	: m_pFont{ ContentManager::Load<SpriteFont>(L"SpriteFonts/Nightwarrior_64.fnt") }
	, m_Text{ text }
	, m_Position{ position }
	, m_Color{ color }
	, m_Bounds{ bounds }
	, m_ActionIdSelect(actionIdSelect)
{

}

void Button::SetPressedFunction(std::function<void()> newPressedFunction)
{
	m_PressedFunction = newPressedFunction;
}

void Button::Draw(const SceneContext& /*sceneContext*/)
{
	if (m_Selected == true)
	{
		TextRenderer::Get()->DrawText(m_pFont, m_Text, m_Position, XMFLOAT4{ 1.f, 0.604f, 0.078f, 1.f });
		return;
	}
	TextRenderer::Get()->DrawText(m_pFont, m_Text, m_Position, m_Color);
}

void Button::Update(const SceneContext& sceneContext)
{
	if (m_Selected == true)
	{
		if (sceneContext.pInput->IsActionTriggered(m_ActionIdSelect) && m_PressedFunction != nullptr)
		{
			m_PressedFunction();
		}
	}
}