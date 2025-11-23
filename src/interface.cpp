#include <string>
#include <format>
#include <fstream>
#include <iostream>
#include "interface.hpp"
#include "figure.hpp"

using namespace std;

void Interface::fillBoardFromFile(CheckerBoard &checker, std::string fileName) {
	std::ifstream file(fileName);
	if (!file.is_open())
		throw "File is not open";

	std::string sideStr, stateStr, position;
	while (file >> sideStr >> stateStr >> position) {
		Figure figure;

		if (sideStr == "white") {
			figure.setSide(FigureSide::white);
			if (stateStr == "piece") {
				figure.setState(FigureState::piece);
				figure.setSymbol(format("{}{}{}", colorWhite, symbolPiece, colorEnd));
			} else {
				figure.setState(FigureState::king);
				figure.setSymbol(format("{}{}{}", colorWhite, symbolKing, colorEnd));
			}
		} else {
			figure.setSide(FigureSide::black);
			if (stateStr == "piece") {
				figure.setState(FigureState::piece);
				figure.setSymbol(format("{}{}{}", colorBlack, symbolPiece, colorEnd));
			} else {
				figure.setState(FigureState::king);
				figure.setSymbol(format("{}{}{}", colorBlack, symbolKing, colorEnd));
			}
		}
		figure.setPosition(position);

		checker.getBoard()[figure.getX()][figure.getY()] = figure;
	}
}

void Interface::drowBoard(CheckerBoard &checker)
{
	string b = format("{}{}{}", colorBorder, symbolBorder, colorEnd);
	bool isWhite = true;

	for (int i = 7; i >= 0; i--)
	{
		cout << "   \033[40m";
		for (int j = 0; j < 8; j++) cout << "+---";
		cout << "+\033[0m\n";

		cout << "\033[092m " << i + 1 << " \033[0m";
		for (int j = 0; j < 8; j++) {
			if (isWhite) {
				cout << b << "\033[107m \033[0m" << "\033[107m" << " " << "\033[0m" << "\033[107m \033[0m";
				isWhite = false;
			} else {
				cout << b << "\033[30m \033[0m" << "\033[30m" << " " << "\033[0m" << "\033[30m \033[0m";
				isWhite = true;
			}
		}
		cout << b << "\n";

		(isWhite) ? isWhite = false : isWhite = true;
	}
	cout << "   \033[40m";
	for (int j = 0; j < 8; j++) cout << "+---";
	cout << "+\033[0m\n";

	cout << "\033[092m     ";
	for (char j = 'A'; j < 'I'; j++)
		cout << j << "   ";
	cout << "\033[0m\n";
}
