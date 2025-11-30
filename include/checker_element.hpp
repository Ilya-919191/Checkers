#ifndef CHECKER_ELEMENT_HPP
#define CHECKER_ELEMENT_HPP

#include <string>

class CheckerElement
{
  std::string pos{""};
  int x{-1}, y{-1};

public:
  CheckerElement() = default;
  CheckerElement(std::string position);

  int getX() const { return x; }
	int getY() const { return y; }
  std::string getPosition() const { return pos; }
  void setPositionByNums(int x_, int y_);
  void setPosition(std::string position);
};

#endif // CHECKER_ELEMENT_HPP