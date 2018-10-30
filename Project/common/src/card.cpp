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

	for (uint8_t i = 0; i < 3; ++i) {
		for (uint8_t j = 0; j < 5; ++j) {
			uint8_t index = j + i * 5;
			
			m_texts[index].setFont(m_font);
			float h_size = (size.x * 0.9f) / 7.0f;	// 7 = 5 numbers + 0.5 number size as a gap between numbers per number
			float v_size = (size.y * 0.9f) / 4.0f;	// 4 = 3 numbers + 0.5 number size as a gap between numbers per number
			m_texts[index].setCharacterSize((h_size + v_size) * 0.5f);
			m_texts[index].setColor(sf::Color::Black);
			m_texts[index].setString("00");
			sf::FloatRect zerozero_bounds = m_texts[index].getGlobalBounds();
			m_texts[index].setString(std::to_string(index));
			sf::FloatRect text_bounds = m_texts[index].getGlobalBounds();
			// float horizontal_gap = text_bounds.width * 0.5f;
			// float vertical_gap = text_bounds.height * 0.5f;
			float horizontal_gap = zerozero_bounds.width * 0.5f;
			float vertical_gap = zerozero_bounds.height * 0.5f;
			float extra_horizontal_gap = 0.0f;
			float extra_vertical_gap = 0.0f;
			if (index < 10) {
				extra_horizontal_gap = zerozero_bounds.width * 0.25f;
				extra_vertical_gap = (h_size + v_size) * 0.25f;
			}
			m_texts[index].setPosition((position.x + extra_horizontal_gap + (size.x * 0.1f * 0.5f)) + j * (h_size + horizontal_gap),
				(position.y + (size.y * 0.1f * 0.5f)) + i * (v_size + vertical_gap));
		}
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