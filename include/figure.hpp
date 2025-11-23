#ifndef FIGURE_HPP
#define FIGURE_HPP

#include <string>

enum class FigureSide {
	white,
	black
};

enum class FigureState {
	none,
	piece,
	king
};

class Figure {
	FigureSide side{FigureSide::white};
	FigureState state{FigureState::none};
	int x{1}, y{1};
	std::string symbol{" "};
	
public:
	Figure() = default;
	Figure(FigureSide side_, FigureState state_, std::string position, std::string symbol_);

	FigureSide getSide() const { return side; }
	FigureState getState() const { return state; }
	int getX() const { return x; }
	int getY() const { return y; }
	std::string getSymbol() const { return symbol; }

	void setSide(FigureSide side_);
	void setState(FigureState state_);
	void setPosition(std::string position_);
	void setSymbol(std::string symbol_);
};

#endif // FIGURE_HPP