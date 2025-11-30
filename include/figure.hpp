#ifndef FIGURE_HPP
#define FIGURE_HPP

#include <string>
#include "checker_element.hpp"

enum class FigureSide {
	white,
	black
};

enum class FigureState {
	none,
	piece,
	king
};

class Figure : public CheckerElement {
	FigureSide side{FigureSide::white};
	FigureState state{FigureState::none};
	std::string symbol{" "};
	bool isPassFigure{false};
	
public:
	Figure() = default;
	Figure(FigureSide side_, FigureState state_, std::string position, std::string symbol_);

	FigureSide getSide() const { return side; }
	FigureState getState() const { return state; }
	std::string getSymbol() const { return symbol; }
	bool isPass() const { return isPassFigure; }

	void setSide(FigureSide side_);
	void setState(FigureState state_);
	void setSymbol(std::string symbol_);
	void setPass(bool isPass);
};

#endif // FIGURE_HPP