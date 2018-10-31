#include "button.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "utils.h"

Button::Button() {
	// TODO: Should use a parametrized function

	m_position.x = 0.0f;
	m_position.y = 0.0f;

	m_size.x = 50.0f;
	m_size.y = 50.0f;

	m_shape.setSize(m_size);
	m_shape.setFillColor(sf::Color::White);
	m_shape.setOrigin(m_size * 0.5f);
	m_shape.setPosition(m_position);

	m_font.loadFromFile(GetResourcePath("fonts/arial.ttf"));

	m_text.setFont(m_font);
	m_text.setCharacterSize(14);
	m_text.setColor(sf::Color::Black);
	sf::FloatRect bounds = m_text.getGlobalBounds();
	m_text.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
	m_text.setPosition(m_position);
}

Button::Button(const std::string& text, const sf::Vector2f& size, const sf::Color& color,
	const sf::Vector2f& position, uint8_t character_size) {

	m_position = position;
	m_size = size;

	m_shape.setSize(size);
	m_shape.setFillColor(color);
	m_shape.setOrigin(size * 0.5f);
	m_shape.setPosition(position);

	m_font.loadFromFile(GetResourcePath("fonts/arial.ttf"));

	m_text.setFont(m_font);
	m_text.setString(text);
	m_text.setCharacterSize(((size.x / text.size()) + size.y) * 0.5f);
	m_text.setColor(sf::Color::Black);
	sf::FloatRect text_bounds = m_text.getLocalBounds();
	m_text.setOrigin(text_bounds.width * 0.5f, text_bounds.height * 0.5f);
	// m_text.setPosition(position.x - text_bounds.width * 0.25f,
	// 	position.y - text_bounds.height * 0.5f);
	float extra_x_offset = 0.0f;
	float extra_y_offset = 0.0f;
	if (text.size() == 1) {
		switch (text[0]) {
			case '+': {
				extra_x_offset = text_bounds.width * 0.15f;
				extra_y_offset = text_bounds.height * 0.5f;
				
				break;
			}
			case '-': {
				extra_x_offset = text_bounds.width * 0.15f;
				extra_y_offset = text_bounds.height * 7.5f;

				break;
			}
		}
	}
	m_text.setPosition(position.x - extra_x_offset, 
		position.y - extra_y_offset - text_bounds.height * 0.33f);
}

Button::~Button() {

}

void Button::update(sf::RenderWindow* window) {
	sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
	bool mouse_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	bool in_bounds = mouse_pos.x >= m_position.x - m_size.x * 0.5f && mouse_pos.x <= m_position.x + m_size.x * 0.5f &&
		mouse_pos.y >= m_position.y - m_size.y * 0.5f && mouse_pos.y <= m_position.y + m_size.y * 0.5f;

	if (mouse_pressed == true) {
		if (in_bounds == true) {
			if (m_can_be_pressed == true && m_mouse_was_pressed == false) {
				m_is_pressed = true;
				m_can_be_pressed = false;
				m_shape.setScale(0.85f, 0.85f);
				m_text.setScale(0.85f, 0.85f);
			}
		}
		else {
			m_is_pressed = false;
			m_can_be_pressed = true;
			m_mouse_was_pressed = false;
			m_shape.setScale(1.0f, 1.0f);
			m_text.setScale(1.0f, 1.0f);
		}
		
		m_mouse_was_pressed = true;
	}
	else {
		m_is_pressed = false;
		m_can_be_pressed = true;
		m_mouse_was_pressed = false;
		m_shape.setScale(1.0f, 1.0f);
		m_text.setScale(1.0f, 1.0f);
	}

	// Not the best solution by no reusing code, but didn't want to lose more time
} 

void Button::draw(sf::RenderWindow* window) {
	window->draw(m_shape);
	window->draw(m_text);
}

bool Button::isPressed() const {
	return m_is_pressed;
}