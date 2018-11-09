#include "card.h"

#include <chrono>

#include "utils.h"

std::mt19937 Card::m_random_generator;
bool Card::m_random_generator_seeded = false;

Card::Card() {
	m_position.x = 0.0f;
	m_position.y = 0.0f;

	m_size.x = 50.0f;
	m_size.y = 50.0f;

	m_shape.setSize(m_size);
	m_shape.setPosition(m_position);
	m_shape.setFillColor(sf::Color::White);
	m_shape.setOutlineColor(sf::Color::Black);
	m_shape.setOutlineThickness(-1.0f);

	m_font.loadFromFile(GetResourcePath("fonts/arial.ttf"));
	
	for (uint8_t i = 0; i < NUMBERS_AMOUNT_IN_CARD; ++i) {
		m_texts[i].setFont(m_font);
		m_texts[i].setCharacterSize(12.5f);
		m_texts[i].setColor(sf::Color::Black);
		m_texts[i].setString("00");
		sf::FloatRect text_bounds = m_texts[i].getGlobalBounds();
		m_texts[i].setPosition(0, 0);
	}
}

Card::Card(const sf::Vector2f& size, const sf::Vector2f& position) {
	m_shape.setFillColor(sf::Color::White);
	m_shape.setOutlineColor(sf::Color::Black);
	m_shape.setOutlineThickness(-(size.x * 5.0f / 400.0f));

	m_font.loadFromFile(GetResourcePath("fonts/arial.ttf"));

	for (uint8_t i = 0; i < NUMBERS_AMOUNT_IN_CARD; ++i) {
		m_texts[i].setFont(m_font);
	}

	Card::create(size, position);
}

Card::~Card() {
	
}

void Card::create(const sf::Vector2f& size, 
	const sf::Vector2f& position) {

	m_position = position;

	m_size = size;

	m_shape.setSize(size);
	m_shape.setPosition(position);

	if (m_random_generator_seeded == false) {
		m_random_generator.seed(std::chrono::duration_cast<std::chrono::duration<int32_t > >(std::chrono::high_resolution_clock::now().time_since_epoch()).count());

		m_random_generator_seeded = true;
	}

	for (uint8_t i = 0; i < 3; ++i) {
		for (uint8_t j = 0; j < 5; ++j) {
			uint8_t index = j + i * 5;
			uint8_t random = 0;
			// Draw random numbers many times so it doesn't produce similar numbers
			for (uint8_t k = 0; k < 50; ++k) {
				random = (uint8_t)m_random_generator() % 90 + 1;
			}
			for (uint8_t k = 0; k < index; ++k) {
				if (random == m_numbers[k]) {
					random = (uint8_t)m_random_generator() % 90 + 1;
					k = 0;
				}
			}
			m_numbers[index] = random;
			
			float h_size = (size.x * 0.9f) / 7.0f;	// 7 = 5 numbers + 0.5 number size as a gap between numbers per number
			float v_size = (size.y * 0.9f) / 4.0f;	// 4 = 3 numbers + 0.5 number size as a gap between numbers per number
			m_texts[index].setCharacterSize((h_size + v_size) * 0.5f);
			m_texts[index].setColor(sf::Color::Black);
			m_texts[index].setString("00");
			sf::FloatRect zerozero_bounds = m_texts[index].getGlobalBounds();
			m_texts[index].setString(std::to_string(m_numbers[index]));
			sf::FloatRect text_bounds = m_texts[index].getGlobalBounds();
			float horizontal_gap = zerozero_bounds.width * 0.5f;
			float vertical_gap = zerozero_bounds.height * 0.5f;
			float extra_horizontal_gap = 0.0f;
			float extra_vertical_gap = 0.0f;
			if (m_numbers[index] < 10) {
				extra_horizontal_gap = zerozero_bounds.width * 0.25f;
				extra_vertical_gap = (h_size + v_size) * 0.25f;
			}
			m_texts[index].setPosition((position.x + extra_horizontal_gap + (size.x * 0.1f * 0.5f)) + j * (h_size + horizontal_gap),
				(position.y + (size.y * 0.1f * 0.5f)) + i * (v_size + vertical_gap));
		}
	}
}

void Card::draw(sf::RenderWindow* window) {
	window->draw(m_shape);
	for (uint8_t i = 0; i < NUMBERS_AMOUNT_IN_CARD; ++i) {
		window->draw(m_texts[i]);
	}
}

uint8_t* Card::getNumbers() {
	return m_numbers;
}