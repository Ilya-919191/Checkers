#include <iterator>
#include <string>
#include <format>
#include <fstream>
#include <iostream>
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "interface.hpp"
#include "board.hpp"
#include "checker_board.hpp"
#include "figure.hpp"

using namespace std;

Interface::Interface(CheckerBoard& checkerBoard) : checker(checkerBoard) {}

void Interface::fillBoardFromFile(std::string fileName) {
	std::ifstream file(fileName);
	if (!file.is_open())
		throw "File was not open.";

	std::string sideStr, stateStr, position;
	while (file >> sideStr >> stateStr >> position) {
		Figure figure;

		FigureSide side = (sideStr == "white") ? FigureSide::white : FigureSide::black;
		figure.setSide(side);

		FigureState state = (stateStr == "piece") ? FigureState::piece : FigureState::king;
		figure.setState(state);

		std::string color = (side == FigureSide::white) ? colorWhite : colorBlack;
		std::string symbol = (state == FigureState::piece) ? symbolPiece : symbolKing;

		figure.setSymbol(format("{}{}{}", color, symbol, colorEnd));
		figure.setPosition(position);
		checker.getFigures()[figure.getX()][figure.getY()] = figure;
	}

	bool isWhite = false;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			Board **board = checker.getBoard();
			Figure **figures = checker.getFigures();

			if (isWhite) {
				board[i][j].setSymbol(colorBackgroundWhite);
				isWhite = false;
			} else {
				board[i][j].setSymbol(colorBackgroundBlack);
				isWhite = true;
			}

			if (board[i][j].getTile() == TileMoment::none) {
				board[i][j].setPositionByNums(i, j);
				figures[i][j].setPositionByNums(i, j);
			}
		}
		isWhite = !isWhite;
	}
}

void Interface::drowBoard()
{
	system("clear");

	for (int i = 7; i >= 0; i--)
	{
		cout << "\033[092m " << i + 1 << " " << colorEnd;
		for (int j = 0; j < 8; j++) {
			const string background = checker.getBoard()[i][j].getSymbol();
			const string figure = checker.getFigures()[i][j].getSymbol();

			cout << background << figure << background << colorEnd;
		}
		cout << endl;
	}
	cout << "\033[092m    ";
	for (char j = 'A'; j < 'I'; j++)
		cout << j << "  ";
	cout << "\033[0m\n";
}

inline bool inBounds(int i, int j) {
    return i >= 0 && i < 8 && j >= 0 && j < 8;
}

inline bool onBorder(int i, int j) {
	return (i == 0 && j == 0) || (i == 7 && j == 7) || (i == 0 && j == 6)
		|| (i == 1 && j == 7) || (i == 6 && j == 0) || (i == 7 && j == 1);
}

Direction Interface::reversDir(Direction direction) {
    switch (direction) {
        case Direction::topRight:    return Direction::bottomLeft;
        case Direction::bottomRight: return Direction::topLeft;
        case Direction::topLeft:     return Direction::bottomRight;
        case Direction::bottomLeft:  return Direction::topRight;
        default:                     return Direction::topRight;
    }
}

pair<int,int> Interface::getPosByDir(int i, int j, Direction direction) {
    switch (direction) {
        case Direction::topRight:    return {i + 1, j + 1};
        case Direction::bottomRight: return {i - 1, j + 1};
        case Direction::topLeft:     return {i + 1, j - 1};
        case Direction::bottomLeft:  return {i - 1, j - 1};
        default:                     return {-1, -1};
    }
}

Figure& Interface::getFigureByDir(int i, int j, Direction direction) {
    pair<int,int> pos = getPosByDir(i, j, direction);

    static Figure empty;
    if (!inBounds(pos.first, pos.second))
			return empty;

    return checker.getFigures()[pos.first][pos.second];
}

void Interface::setBackgroudOfTile(int i, int j, TileMoment moment) {
    if (!inBounds(i, j)) return;

    if (moment == TileMoment::attack)
        checker.getBoard()[i][j].setSymbol(colorAttack);
    else if (moment == TileMoment::active)
        checker.getBoard()[i][j].setSymbol(colorActive);
    else if (moment == TileMoment::move)
        checker.getBoard()[i][j].setSymbol(colorMove);
    else if (moment == TileMoment::finish)
        checker.getBoard()[i][j].setSymbol(colorFinish);
    else if (moment == TileMoment::none)
        checker.getBoard()[i][j].setSymbol( ( (i + j) % 2 != 0 ) ? colorBackgroundWhite : colorBackgroundBlack );

    checker.getBoard()[i][j].setTile(moment);
}

bool Interface::checkFigureForAttack(int i, int j, Direction direction, Figure& startFigure) {
    pair<int,int> pos = getPosByDir(i, j, direction);

    if (!inBounds(pos.first, pos.second))
			return false;

    Figure& figure = checker.getFigures()[pos.first][pos.second];
    return (figure.getState() != FigureState::none && !figure.isPass() && figure.getSide() != startFigure.getSide());
}

int Interface::figureAnalisis(int i, int j, Direction direction, Figure& startFigure, bool isDrow, bool isAttacked, int sumAtk)
{
	if (isDrow) {
		drowBoard();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	// Якщо клітка вийшла за межі поля.
	if (!inBounds(i, j)) return sumAtk;

	// Наступна клітка, за замовченням, слідує за попередньою.
	Direction nextDir = direction;

	// Попередня фігура.
	Figure& prevFigure = getFigureByDir(i, j, reversDir(direction));
	// Фігура у поточній клітці.
	Figure& currFigure = checker.getFigures()[i][j];

	// Позиція наступної клітки.
	pair<int,int> nextPos = getPosByDir(i, j, nextDir);
	// Позиція минулої клітки.
	pair<int,int> prevPos = getPosByDir(i, j, reversDir(direction));

	//--- Якщо поточна клітка не містить фігур.
	if (currFigure.getState() == FigureState::none)
	{
		// Якщо минула клітка містить вказану фігуру, або якщо вказана фігура ще ніразу не атакувала, тоді напрям не змінюється.
		if (prevFigure.getPosition() == startFigure.getPosition() || !isAttacked)
		{
			if (!inBounds(nextPos.first, nextPos.second)) {
				if (isDrow) setBackgroudOfTile(i, j, TileMoment::move);
				return sumAtk;
			}
			// Продовжуємо переміщення по діагоналі.
			if (isDrow) setBackgroudOfTile(i, j, TileMoment::move);
			return figureAnalisis(nextPos.first, nextPos.second, nextDir, startFigure, isDrow, isAttacked, sumAtk);
		}

		// Знаходження ворожої фігури біля себе.
		if (checkFigureForAttack(i, j, Direction::topRight, startFigure) && direction != Direction::bottomLeft)
				nextDir = Direction::topRight;
		else if (checkFigureForAttack(i, j, Direction::bottomRight, startFigure) && direction != Direction::topLeft)
				nextDir = Direction::bottomRight;
		else if (checkFigureForAttack(i, j, Direction::topLeft, startFigure) && direction != Direction::bottomRight)
				nextDir = Direction::topLeft;
		else if (checkFigureForAttack(i, j, Direction::bottomLeft, startFigure) && direction != Direction::topRight)
				nextDir = Direction::bottomLeft;
		else {
			if (isDrow) setBackgroudOfTile(i, j, TileMoment::finish);
			return sumAtk;
		}

		// Оновлення координат наступної клітки.
		nextPos = getPosByDir(i, j, nextDir);

		// Якщо наступна клітка виходить за межі поля, тоді зупиняємось.
		if (!inBounds(nextPos.first, nextPos.second)) {
			if (isDrow) setBackgroudOfTile(i, j, TileMoment::finish);
			return sumAtk;
		}

		// Атака на знайдену ворожу фігуру.
		if (isDrow) setBackgroudOfTile(i, j, TileMoment::move);
		return figureAnalisis(nextPos.first, nextPos.second, nextDir, startFigure, isDrow, isAttacked, sumAtk);

		//--- Якщо поточна клітка містить фігуру.
	} else
	{
		// Якщо після минулої ворожії фігури йде підряд наступна, тоді потрібно відступити та зупинитись.
		if (prevFigure.getState() != FigureState::none) {
			if (isDrow) setBackgroudOfTile(prevPos.first, prevPos.second, TileMoment::none);
			return sumAtk;
		}

		// Якщо поточна клітка містить нашу фігуру, тоді потрібно відступити та зупинитись.
		if (currFigure.getSide() == startFigure.getSide()) {
			if (isDrow) setBackgroudOfTile(prevPos.first, prevPos.second, TileMoment::move);
			return sumAtk;
		}

		// Якщо наступна клітка виходить за межі поля, тоді відступити та зупинитись.
		if (!inBounds(nextPos.first, nextPos.second)) {
			currFigure.setPass(true);
			return figureAnalisis(prevPos.first, prevPos.second, reversDir(nextDir), startFigure, isDrow, isAttacked, sumAtk);
		}

		// Атака та переміщення через ворожу клітку.
		if (isDrow) setBackgroudOfTile(i, j, TileMoment::attack);
		return figureAnalisis(nextPos.first, nextPos.second, nextDir, startFigure, isDrow, true, ++sumAtk);
	}
}

void Interface::showPossiblyAttack()
{
	string selectPos{"B8"};
	// cout << "Enter position: ";
	// cin >> selectPos;

	int row{0}, col{0};
	checker.transformPosition(selectPos, col, row);
	Figure& figure = checker.getFigures()[row][col];

	if (checker.isWhiteInDown()) {
		int allDirHits[4] = {
			figureAnalisis(row + 1, col + 1, Direction::topRight, figure, false),
			figureAnalisis(row - 1, col + 1, Direction::bottomRight, figure, false),
			figureAnalisis(row + 1, col - 1, Direction::topLeft, figure, false),
			figureAnalisis(row - 1, col - 1, Direction::bottomLeft, figure, false)
		};
		const int* maxHit = std::max_element(std::begin(allDirHits), std::end(allDirHits));

		if (allDirHits[0] == *maxHit) figureAnalisis(row + 1, col + 1, Direction::topRight, figure, true);
		else if (allDirHits[1] == *maxHit) figureAnalisis(row - 1, col + 1, Direction::bottomRight, figure, true);
		else if (allDirHits[2] == *maxHit) figureAnalisis(row + 1, col - 1, Direction::topLeft, figure, true);
		else if (allDirHits[3] == *maxHit) figureAnalisis(row - 1, col - 1, Direction::bottomLeft, figure, true);
	}
	
	setBackgroudOfTile(row, col, TileMoment::active);
	drowBoard();
}