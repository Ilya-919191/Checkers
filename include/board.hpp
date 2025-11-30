#ifndef BOARD_HPP
#define BOARD_HPP

#include <string>
#include "checker_element.hpp"

enum class TileMoment {
  none,
  attack,
  active,
  move,
  finish
};

class Board : public CheckerElement
{
	TileMoment tile{TileMoment::none};
  std::string symbol{"\033[107m "};

public:
  Board() = default;
  Board(TileMoment tile_, std::string position_);

  TileMoment getTile() const { return tile; }
  std::string getSymbol() const { return symbol; }
  void setSymbol(std::string newSymbol);
  void setTile(TileMoment newTile);
};

#endif // BOARD_HPP