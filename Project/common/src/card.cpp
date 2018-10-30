#include "card.h"

#include "utils.h"

Card::Card() {
	m_position.x = 0.0f;
	m_position.y = 0.0f;

	m_size.x = 50.0f;
	m_size.y = 50.0f;

	m_shape.setSize(m_size);
	m_shape.setFillColor(sf::Color::White);
	m_shape.setPosition(m_position);

	m_font.loadFromFile(GetResourcePath("fonts/arial.ttf"));
	
	for (uint8_t i = 0; i < NUMBERS_AMOUT_IN_CARD; ++i) {
		m_texts[i].setFont(m_font);
		m_texts[i].setCharacterSize(12.5f);
		m_texts[i].setColor(sf::Color::Black);
		m_texts[i].setString("00");
		sf::FloatRect text_bounds = m_texts[i].getGlobalBounds();
		m_texts[i].setPosition(0, 0);
	}
}

Card::Card(const sf::Vector2f& size, const sf::Vector2f& position) {
	m_position = position;

	m_size = size;

	m_shape.setSize(size);
	m_shape.setFillColor(sf::Color::White);
	m_shape.setPosition(position);

	m_font.loadFromFile(GetResourcePath("fonts/arial.ttf"));

	for (uint8_t i = 0; i < NUMBERS_AMOUT_IN_CARD; ++i) {
		m_texts[i].setFont(m_font);
		m_texts[i].setCharacterSize(((size.x + size.y) * 0.5f) * 0.5f);
		m_texts[i].setColor(sf::Color::Black);
		m_texts[i].setString("00");
		sf::FloatRect text_bounds = m_texts[i].getGlobalBounds();
		m_texts[i].setPosition((position.x + size.x * 0.5f) - (text_bounds.width * 0.5f),
		(position.y + size.y * 0.5f) - (text_bounds.height * 0.5f));
	}
}

Card::~Card() {
	
}

void Card::draw(sf::RenderWindow* window) {
	window->draw(m_shape);
	for (uint8_t i = 0; i < NUMBERS_AMOUT_IN_CARD; ++i) {
		window->draw(m_texts[i]);
	}
}