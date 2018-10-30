#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Button {
public:
	Button();
	Button(const std::string& text, const sf::Vector2f& size, const sf::Color& color,
		const sf::Vector2f& position, uint8_t character_size);
	~Button();

	void update(sf::RenderWindow* window);
	void draw(sf::RenderWindow* window);

	bool isPressed() const;

private:
	sf::RectangleShape m_shape;
	sf::Text m_text;
	sf::Font m_font;
	sf::Vector2f m_position;
	sf::Vector2f m_size;
	bool m_can_be_pressed = true;
	bool m_is_pressed = false;
	bool m_mouse_was_pressed = false;
};

#endif // __BUTTON_H__