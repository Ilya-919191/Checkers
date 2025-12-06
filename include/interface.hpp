#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <string>
#include <utility>
#include <vector>
#include "board.hpp"
#include "checker_board.hpp"
#include "figure.hpp"

using namespace std;

enum class Direction {
	none,
  topRight,
  bottomRight,
  bottomLeft,
  topLeft
};

struct AttackPath {
	std::vector<std::pair<int, int>> path;
	std::vector<std::pair<int, int>> captured;
	int attackCount{0};
	
	void addPosition(int i, int j);
	void addCaptured(int i, int j);
	bool isAlreadyCaptured(int i, int j) const;
};

class Interface
{
	CheckerBoard& checker;
	bool whiteInDown{true};

	void clearTiles();
	void whereWhiteSide();
	pair<int, int> getNextPosByDir(int i, int j, Direction direction);
	Figure& getNextFigureByDir(int i, int j, Direction direction);
	Direction reversDir(Direction direction);
	void setBackgroudOfTile(int i, int j, TileMoment moment);

	bool canAttackTarget(int attackerRow, int attackerCol, int targetRow, int targetCol, Figure& attacker);
  void visualizeThreats(int targetRow, int targetCol, const vector<pair<string, pair<int, int>>>& threats);
  Direction findAttackDirection(int fromRow, int fromCol, int toRow, int toCol);
  void drawAttackLine(int fromRow, int fromCol, int toRow, int toCol, Direction dir);
	int figureAnalisis(int i, int j, Direction direction, Figure& startFigure, bool isDrawing,
										bool isAttacking, int attackCount, AttackPath& currentPath, AttackPath& bestPath,
										vector<pair<int, int>>& visitedPositions);
	void showWhoCanAttackFigure();

public:
	Interface(CheckerBoard& checkerBoard);

	void setWhiteSide();
	bool isWhiteInDown() const { return whiteInDown; }
	void fillBoardFromFile(std::string fileName);
	void showPossibleMovesAndAttacks();
	void drowBoard(string text = "");
	void showThreatsToFigure();
	void showMenu();
};

#endif // INTERFACE_HPP
