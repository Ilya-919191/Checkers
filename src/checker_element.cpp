#include <string>
#include "checker_element.hpp"

CheckerElement::CheckerElement(std::string position) {
  setPosition(position);
}

void CheckerElement::setPositionByNums(int x_, int y_) {
  x = x_;
  y = y_;
}

void CheckerElement::setPosition(std::string position) {
  if (position.length() != 2) {
    throw "The position of figures must consist of one letter and one number. Example: A6";
  }
  char col = position[0];
  char row = position[1];

  if (row % 2 == 0) {
    if ((col % 'A') % 2 == 0)
      throw "The position of figures must be on the black tiles.\nWrong your position: " + position;
  } else {
    if ((col % 'A') % 2 != 0)
      throw "The position of figures must be on the black tiles.\nWrong your position: " + position;
  }

  if (col < 'A' || col > 'H' || row < '1' || row > '8') {
    throw "Position out of board range. Example: A6";
  }

  pos = position;
  x = row - '1';
  y = col - 'A';
}