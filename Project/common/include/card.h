#ifndef __CARD_H__
#define __CARD_H__

#include <random>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

constexpr uint8_t NUMBERS_AMOUNT_IN_CARD = 15;

class Card {
public:
	Card();
	Card(const sf::Vector2f& size, const sf::Vector2f& position);
	~Card();

	void create(const sf::Vector2f& size, const sf::Vector2f& position);

	void draw(sf::RenderWindow* window);

	uint8_t* getNumbers();

private:
	sf::RectangleShape m_shape;
	sf::Font m_font;
	sf::Text m_texts[NUMBERS_AMOUNT_IN_CARD];
	sf::Vector2f m_position;
	sf::Vector2f m_size;
	static std::mt19937 m_random_generator;
	uint8_t m_numbers[NUMBERS_AMOUNT_IN_CARD] = { 0 };
	static bool m_random_generator_seeded;
};

#endif // __CARD_H__