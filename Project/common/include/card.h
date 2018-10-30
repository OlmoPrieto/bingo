#ifndef __CARD_H__
#define __CARD_H__

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

constexpr uint8_t NUMBERS_AMOUT_IN_CARD = 15;

class Card {
public:
	Card();
	Card(const sf::Vector2f& size, const sf::Vector2f& position);
	~Card();

	void draw(sf::RenderWindow* window);

private:
	sf::RectangleShape m_shape;
	sf::Font m_font;
	sf::Vector2f m_position;
	sf::Vector2f m_size;
	sf::Text m_texts[NUMBERS_AMOUT_IN_CARD];
	uint8_t m_number[NUMBERS_AMOUT_IN_CARD] = { 0 };
};

#endif // __CARD_H__