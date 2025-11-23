#include "figure.hpp"

Figure::Figure(FigureSide side_, FigureState state_, std::string position, std::string symbol_)
	: side(side_), state(state_), symbol(symbol_)
{
  setPosition(position);
}

void Figure::setSide(FigureSide side_) {
  side = side_;
}

void Figure::setState(FigureState state_) {
  state = state_;
}

void Figure::setPosition(std::string position) {
  if (position.length() != 2) {
    throw "The position of figures must consist of one letter and one number. Example: A6";
  }
  char col = position[0];
  char row = position[1];

  if (col < 'A' || col > 'H' || row < '1' || row > '8') {
    throw "Position out of board range. Example: A6";
  }

  x = row - '1';
  y = col - 'A';
}

void Figure::setSymbol(std::string symbol_) {
  symbol = symbol_;
}