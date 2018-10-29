#include "button.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "utils.h"

Button::Button() {
	m_position.x = 0.0f;
	m_position.y = 0.0f;

	m_size.x = 50.0f;
	m_size.y = 50.0f;

	m_shape.setSize(m_size);
	m_shape.setFillColor(sf::Color::White);
	m_shape.setPosition(m_position);
	m_font.loadFromFile(GetResourcePath("fonts/arial.ttf"));
	m_text = new sf::Text();
	m_text->setFont(m_font);
	m_text->setCharacterSize(14);
	m_text->setColor(sf::Color::Black);
	m_text->setPosition(m_position);
}

Button::Button(const std::string& text, const sf::Vector2f& size, const sf::Color& color,
	const sf::Vector2f& position, uint8_t character_size) {

	m_position = position;
	m_size = size;

	m_shape.setSize(size);
	m_shape.setFillColor(color);
	m_shape.setPosition(position);
	m_font.loadFromFile(GetResourcePath("fonts/arial.ttf"));
	m_text = new sf::Text();
	m_text->setFont(m_font);
	m_text->setString(text);
	m_text->setCharacterSize(character_size);
	m_text->setColor(sf::Color::Black);
	sf::FloatRect text_bounds = m_text->getGlobalBounds();
	m_text->setPosition((position.x + size.x * 0.5f) - (text_bounds.width * 0.5f),
		(position.y + size.y * 0.5f) - (text_bounds.height * 0.5f));
}

Button::~Button() {
	delete m_text;
}

void Button::update(sf::RenderWindow* window) {
	sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) &&
		(mouse_pos.x >= m_position.x && mouse_pos.x <= m_position.x + m_size.x &&
		mouse_pos.y >= m_position.y && mouse_pos.y <= m_position.y + m_size.y)) {

		if (m_can_be_pressed == true) {
			m_can_be_pressed = false;
			m_is_pressed = true;
			m_shape.setScale(0.85f, 0.85f);
			m_text->setScale(0.85f, 0.85f);
		}
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) == false) {
		if (m_can_be_pressed == false) {
			m_can_be_pressed = true;
			m_is_pressed = false;
			m_shape.setScale(1.0f, 1.0f);
			m_text->setScale(1.0f, 1.0f);
		}
	}
} 

void Button::draw(sf::RenderWindow* window) {
	window->draw(m_shape);
	window->draw(*m_text);
}

bool Button::isPressed() const {
	return m_is_pressed;
}