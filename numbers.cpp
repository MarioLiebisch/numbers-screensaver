#pragma warning(disable: 4996)

#include <SFML/Graphics.hpp>
#include <ctime>
#include <chrono>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Define to show minutes and seconds (for faster changes)
// #define TESTING
// Define to make the colon flip every second (twice)
// #define FLIP_COLON

const sf::Time flip_time = sf::seconds(.5f);
const sf::Time dots_time = sf::seconds(.5f);

const std::size_t card_x = 720;
const std::size_t card_y = 1080;
const std::size_t card_spacing = 32;

class Digit : public sf::Drawable, public sf::Transformable {
	bool busy = false;
	unsigned char current = 0;
	unsigned char next = 0;
	unsigned char count = 10;

	sf::Sprite scurrent_top;
	sf::Sprite snext_top;
	sf::Sprite scurrent_bottom;
	sf::Sprite snext_bottom;

	sf::Time time;

public:
	Digit(const sf::Texture &texture) {
		scurrent_top.setTexture(texture);
		scurrent_top.setOrigin(0, card_y / 2.f);
		snext_top.setTexture(texture);
		snext_top.setOrigin(0, card_y / 2.f);

		scurrent_bottom.setTexture(texture);
		snext_bottom.setTexture(texture);

		scurrent_top.setTextureRect({ 0, 0, card_x, card_y / 2 });
		scurrent_bottom.setTextureRect({ 0, card_y / 2, card_x, card_y / 2 });
	}

	virtual ~Digit() = default;

	auto active() const -> bool {
		return current != next;
	}

	auto set(const unsigned char n, bool instant = false) -> void {
		if (next == n) {
			return;
		}
		if (instant) {
			scurrent_top.setTextureRect({ (n % 5) * card_x, (n / 5) * card_y, card_x, card_y / 2 });
			scurrent_bottom.setTextureRect({ (n % 5) * card_x, (n / 5) * card_y + card_y / 2, card_x, card_y / 2 });
			snext_top.setTextureRect({ (n % 5) * card_x, (n / 5) * card_y, card_x, card_y / 2 });
			snext_bottom.setTextureRect({ (n % 5) * card_x, (n / 5) * card_y + card_y / 2, card_x, card_y / 2 });
			current = next = n;
		}
		else {
			next = n;
		}
	}

	auto setCount(const unsigned char n) -> void {
		count = n;
	}

	auto update(const sf::Time &dt) -> void {
#ifdef FLIP_COLON
		if (current == 10 || current == 11) {
			time += dt;
			if (current == next) {
				while (time > dots_time) {
					time -= dots_time;
					next = 10 + ((current & 1) ^ 1);
				}
			}
			else {
				time += dt;
				while (time > flip_time) {
					time -= flip_time;
					current = next;
					break;
				}
				const unsigned char other = current == 10 ? 11 : 10;
				snext_top.setTextureRect({ (other % 5) * card_x, (other / 5) * card_y, card_x, card_y / 2 });
				snext_bottom.setTextureRect({ (other % 5) * card_x, (other / 5) * card_y + card_y / 2, card_x, card_y / 2 });
				scurrent_top.setTextureRect({ (current % 5) * card_x, (current / 5) * card_y, card_x, card_y / 2 });
				scurrent_bottom.setTextureRect({ (current % 5) * card_x, (current / 5) * card_y + card_y / 2, card_x, card_y / 2 });
			}
		}
		else
#endif
		if (current != next) {
			time += dt;
			while (time > flip_time) {
				time -= flip_time;
				current = (current + 1) % count;
				if (current == next) {
					break;
				}
			}
			snext_top.setTextureRect({ (((current + 1) % count) % 5) * card_x, (((current + 1) % count) / 5) * card_y, card_x, card_y / 2 });
			snext_bottom.setTextureRect({ (((current + 1) % count) % 5) * card_x, (((current + 1) % count) / 5) * card_y + card_y / 2, card_x, card_y / 2 });
			scurrent_top.setTextureRect({ (current % 5) * card_x, (current / 5) * card_y, card_x, card_y / 2 });
			scurrent_bottom.setTextureRect({ (current % 5) * card_x, (current / 5) * card_y + card_y / 2, card_x, card_y / 2 });
		}
		else {
			time = sf::Time::Zero;
		}
	}

	auto sf::Drawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const -> void {
		states.transform *= getTransform();
		states.transform.translate(0, card_y / 2.f);

		if (current == next) {
			rt.draw(scurrent_top, states);
			rt.draw(scurrent_bottom, states);
		}
		else {
			const float fract = std::sin(.5f * 3.1415f * time / flip_time);
			if (fract < .5f) {
				rt.draw(snext_top, states);
				rt.draw(scurrent_bottom, states);
				states.transform.scale(1, 1 - 2 * fract);
				rt.draw(scurrent_top, states);
			}
			else {
				rt.draw(snext_top, states);
				rt.draw(scurrent_bottom, states);
				states.transform.scale(1, 2 * (fract - .5f));
				rt.draw(snext_bottom, states);
			}
		}
	}
};

auto main(int argc, char **argv) -> int {
	sf::WindowHandle hwnd = nullptr;
	sf::VideoMode size;
	sf::RenderWindow window;

	if (argc > 1) {
		// Run
		if (strcmp(argv[1], "/s") == 0 || strcmp(argv[1], "/S") == 0) {

		}
		// Preview
		else if ((strcmp(argv[1], "/p") == 0 || strcmp(argv[1], "/P") == 0) && argc > 2) {
			hwnd = reinterpret_cast<sf::WindowHandle>(std::atoll(argv[2]));
		}
		// Config
		else {
			MessageBoxA(0, "There is no configuration for this screensaver.", "Numbers", 0);
			return 0;
		}
	}
	// Config
#ifdef NDEBUG
	else {
		MessageBoxA(0, "There is no configuration for this screensaver.", "Numbers", 0);
		return 0;
	}
#endif

	if (hwnd != nullptr) {
		window.create(hwnd);
		size = sf::VideoMode(window.getSize().x, window.getSize().y);
	}
	else {
#ifndef TESTING
		size = sf::VideoMode::getDesktopMode();
		window.create(size, "Numbers", sf::Style::None);
#else
		size = sf::VideoMode(640, 480);
		window.create(size, "Numbers", sf::Style::Titlebar | sf::Style::Close);
#endif
		window.requestFocus();
		window.setMouseCursorVisible(false);
	}

	
	window.setFramerateLimit(30);

	float width_ratio = static_cast<float>(size.width) / (card_x * 5 + card_spacing * 4);
	float height_ratio = static_cast<float>(size.height) / card_y;

	sf::View view({ {2.5f * card_x + 2 * card_spacing, card_y / 2.f}, {5 * card_x + 4 * card_spacing, card_y} });

	if (width_ratio > height_ratio) {
		height_ratio /= width_ratio;
		width_ratio = 1;
	}
	else {
		width_ratio /= height_ratio;
		height_ratio = 1;
	}

	// Border
	width_ratio *= .75f;
	height_ratio *= .75f;

	// Adjust the viewport to keep the proper aspect ratio
	view.setViewport({ .5f - height_ratio / 2.f, .5f - width_ratio / 2.f, height_ratio, width_ratio });
	window.setView(view);

	sf::Texture texture;
	// Try to load the texture
	// If this doesn't work, try looking for it right next to the executable
	if (!texture.loadFromFile("numbers.png")) {

		std::string texfile(argv[0]);
		const std::size_t o = texfile.find_last_of('.');
		if (o == std::string::npos) {
			texfile += ".png";
		}
		else {
			texfile.replace(o, -1, ".png");
		}

		// If this fails, too, we'll just show white blocks for now
		texture.loadFromFile(texfile);
	}
	Digit digits[5] = { {texture}, { texture }, { texture }, { texture }, { texture } };
#ifdef TESTING
	digits[0].setCount(6);
#else
	digits[0].setCount(3);
#endif
	digits[3].setCount(6);

	for (unsigned char i = 0; i < 5; ++i) {
		digits[i].setPosition(static_cast<float>(card_x + card_spacing) * i, 0);
	}
	digits[0].set(2);
	digits[1].set(0);
	digits[2].set(10, true);
	digits[3].set(1);
	digits[4].set(5);

	std::chrono::system_clock clock;
	
#ifdef TESTING
	std::size_t value = 0;
#endif

	sf::Vector2i mouse = sf::Mouse::getPosition();

	bool startup = true;
	sf::Clock timer, starttimer;
	while (window.isOpen()) {
		if (hwnd != nullptr && !IsWindowVisible(hwnd)) {
			window.close();
			break;
		}

		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
#ifndef TESTING
			case sf::Event::MouseMoved:
				// Avoid mouse move events that don't actually move the cursor (window creation, popup notifications, etc.)
				if (mouse.x == event.mouseMove.x && mouse.y == event.mouseMove.y) {
					break;
				}
				window.close();
				break;
			case sf::Event::KeyPressed:
			case sf::Event::MouseButtonPressed:
				if (hwnd) {
					break;
				}
#else
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Up:
					value = (value + 1) % 1440;
					break;
				case sf::Keyboard::Down:
					value = (1440 + value - 1) % 1440;
					break;
				case sf::Keyboard::Left:
					value = (value - 10) % 1440;
					break;
				case sf::Keyboard::Right:
					value = (value + 10) % 1440;
					break;
				}
				break;
#endif
			case sf::Event::Closed:
				window.close();
				break;
			}
		}

		std::time_t now = std::chrono::system_clock::to_time_t(clock.now());
		std::tm tnow = *localtime(&now);

#ifdef TESTING
		digits[0].set((value / 600) % 6);
		digits[1].set((value / 60) % 10);
		digits[3].set((value / 10) % 6);
		digits[4].set(value % 10);
#else
		digits[0].set(tnow.tm_hour / 10);
		digits[1].set(tnow.tm_hour % 10);
		digits[3].set(tnow.tm_min / 10);
		digits[4].set(tnow.tm_min % 10);
#endif

		window.clear();
		const sf::Time dt = timer.restart();
		bool active = false;
		for (unsigned char i = 0; i < 5; ++i) {
			digits[i].update(dt);
			window.draw(digits[i]);
			active |= digits[i].active();
		}
		// window.setFramerateLimit(active ? 30 : 10);
		window.display();
	}
	return 0;
}
