#include "figure.hpp"
#include "checker_element.hpp"

Figure::Figure(FigureSide side_, FigureState state_, std::string position, std::string symbol_)
	: CheckerElement(position), side(side_), state(state_), symbol(symbol_)
{
  setPosition(position);
}

void Figure::setSide(FigureSide side_) {
  side = side_;
}

void Figure::setState(FigureState state_) {
  state = state_;
}

void Figure::setSymbol(std::string symbol_) {
  symbol = symbol_;
}

void Figure::setPass(bool isPass) {
  isPassFigure = isPass;
}