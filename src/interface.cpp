#include <cstddef>
#include <string>
#include <format>
#include <fstream>
#include <iostream>
#include <utility>
#include <cstdlib>
#include "interface.hpp"
#include "board.hpp"
#include "checker_board.hpp"
#include "figure.hpp"
#include "variables.hpp"
#include "utils.hpp"

using namespace std;

void AttackPath::addPosition(int i, int j) { path.push_back({i, j}); } // Додає позицію до шляху атаки
void AttackPath::addCaptured(int i, int j) { captured.push_back({i, j}); attackCount++; } // Додає захоплену фігуру та збільшує лічильник
bool AttackPath::isAlreadyCaptured(int i, int j) const {
  for (const pair<int, int>& cap : captured)
	if (cap.first == i && cap.second == j) return true; // Перевіряє, чи фігура вже захоплена
  return false;
}

Interface::Interface(CheckerBoard& checkerBoard) : checker(checkerBoard) {} // Конструктор інтерфейсу

void Interface::fillBoardFromFile(string fileName) { // Завантаження інформації з файлу
  ifstream file(fileName);
  if (!file.is_open()) throw "File was not open."; // Перевірка відкриття файлу

  string sideStr, stateStr, position;
  while (file >> sideStr >> stateStr >> position) { // Читання даних з файлу
    Figure figure;
    figure.setSide(sideStr == "white" ? FigureSide::white : FigureSide::black); // Встановлення сторони
    figure.setState(stateStr == "piece" ? FigureState::piece : FigureState::king); // Встановлення типу
    figure.setSymbol(format("{}{}{}", // Форматування символу для відображення
        figure.getSide() == FigureSide::white ? colorWhite : colorBlack,
        figure.getState() == FigureState::piece ? symbolPiece : symbolKing,
        colorEnd));
    figure.setPosition(position); // Встановлення позиції
    checker.getFigures()[figure.getX()][figure.getY()] = figure; // Розміщення фігури на дошці
	}
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++) {
      checker.getBoard()[i][j].setSymbol(((i + j) % 2 != 0) ? colorBackgroundWhite : colorBackgroundBlack); // Чергування кольорів
      if (checker.getBoard()[i][j].getTile() == TileMoment::none) { // Ініціалізація порожніх клітин
      checker.getBoard()[i][j].setPositionByNums(i, j); // Позиція клітинки
        checker.getFigures()[i][j].setPositionByNums(i, j); // Позиція фігури
      }
		}
}

void Interface::clearTiles() {
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			setBackgroudOfTile(i, j, TileMoment::none); // Очищення всіх виділень
}

void Interface::drowBoard(string text) { // Відображення дошки
	CLS(); // Очищення екрану
	cout << text << "\n\n";
	for (int i = 7; i >= 0; i--) { // Вивід дошки зверху вниз
		cout << "\033[092m " << i + 1 << " " << colorEnd; // Номер рядка
		for (int j = 0; j < 8; j++)
			cout << checker.getBoard()[i][j].getSymbol() << // Фон клітинки
				  checker.getFigures()[i][j].getSymbol() << // Символ фігури
				  checker.getBoard()[i][j].getSymbol() << colorEnd; // Закриття кольору
		cout << endl;
	}
	cout << "\033[092m    "; // Колір для літер стовпців
	for (char j = 'A'; j < 'I'; j++) cout << j << "  "; // Літери стовпців A-H
	cout << "\033[0m\n"; // Скидання кольору
}

bool inBounds(int i, int j) { return i >= 0 && i < 8 && j >= 0 && j < 8; } // Перевірка чи позиція в межах дошки

Direction Interface::reversDir(Direction d) {
	switch (d) { // Повертає зворотній напрямок
		case Direction::topRight: return Direction::bottomLeft;
		case Direction::bottomRight: return Direction::topLeft;
		case Direction::topLeft: return Direction::bottomRight;
		case Direction::bottomLeft: return Direction::topRight;
		default: return Direction::none;
	}
}

pair<int,int> Interface::getNextPosByDir(int i, int j, Direction d) {
	switch (d) { // Отримує наступну позицію у заданому напрямку
		case Direction::topRight: return {i + 1, j + 1}; // Вгору-праворуч
		case Direction::bottomRight: return {i - 1, j + 1}; // Вниз-праворуч
		case Direction::topLeft: return {i + 1, j - 1}; // Вгору-ліворуч
		case Direction::bottomLeft: return {i - 1, j - 1}; // Вниз-ліворуч
		default: return {-1, -1}; // Невірний напрямок
	}
}

void Interface::setBackgroudOfTile(int i, int j, TileMoment m) {
	if (!inBounds(i, j)) return; // Перевірка меж
	string color;
	if (m == TileMoment::attack) color = colorAttack; // Колір для атаки
	else if (m == TileMoment::active) color = colorActive; // Колір активної фігури
	else if (m == TileMoment::move) color = colorMove; // Колір можливого ходу
	else if (m == TileMoment::finish) color = colorFinish; // Колір фінішу
	else color = ((i + j) % 2 != 0) ? colorBackgroundWhite : colorBackgroundBlack; // Стандартний фон
	checker.getBoard()[i][j].setSymbol(color); // Встановлення кольору фону
	checker.getBoard()[i][j].setTile(m); // Встановлення стану клітинки
}

vector<Direction> getDirections(Figure& figure, bool whiteDown) { // Отримує доступні напрямки для фігури
	if (figure.getState() == FigureState::king) // Дамка може ходити в усі 4 напрямки
		return {Direction::topRight, Direction::bottomRight, Direction::topLeft, Direction::bottomLeft};
	
	bool isWhite = figure.getSide() == FigureSide::white;
	if (whiteDown) // Якщо білі знизу
		return isWhite ? vector{Direction::topRight, Direction::topLeft} : // Білі йдуть вгору
		    vector{Direction::bottomRight, Direction::bottomLeft}; // Чорні йдуть вниз
	else // Якщо білі зверху
		return isWhite ? vector{Direction::bottomRight, Direction::bottomLeft} : // Білі йдуть вниз
		    vector{Direction::topRight, Direction::topLeft}; // Чорні йдуть вгору
}

int Interface::figureAnalisis(int i, int j, Direction dir, Figure& startFigure, // Основний алгоритм аналізу атаки
    bool drawing, bool attacking, int attackCount, 
    AttackPath& curPath, AttackPath& bestPath, vector<pair<int, int>>& visited) {
	if (!inBounds(i, j)) return attackCount; // Вийшли за межі дошки
	if (find(visited.begin(), visited.end(), make_pair(i, j)) != visited.end()) return attackCount; // Вже відвідували цю позицію

	visited.push_back({i, j}); // Додаємо до відвіданих
	curPath.addPosition(i, j); // Додаємо до поточного шляху
	Figure& curFig = checker.getFigures()[i][j]; // Поточна фігура
	vector<Direction> dirs = getDirections(startFigure, whiteInDown); // Доступні напрямки
	
	if (dir == Direction::none) { // Початковий виклик (не задано напрямок)
		int maxResult = 0;
		AttackPath localBest = curPath;
		for (Direction d : dirs) { // Перебір усіх доступних напрямків
			auto next = getNextPosByDir(i, j, d);
			if (!inBounds(next.first, next.second)) continue;
			vector<pair<int, int>> newVisited = visited;
			AttackPath newPath = curPath;
			int result = figureAnalisis(next.first, next.second, d, startFigure, 
									   drawing, false, 0, newPath, bestPath, newVisited); // Рекурсивний виклик
			if (result > maxResult) { // Оновлення максимального результату
				maxResult = result;
				localBest = newPath;
				if (result > bestPath.attackCount) bestPath = newPath; // Оновлення кращого шляху
			}
		}
		visited.pop_back(); // Видаляємо поточну позицію перед поверненням
		return maxResult;
	}
	
	if (curFig.getState() == FigureState::none) { // Порожня клітина
		if (attacking) { // Якщо атакували - потрібен пошук ворожих фігур по діагоналях
			int maxNext = attackCount;
			for (Direction d : dirs) {
				if (d == reversDir(dir)) continue; // Не повертаємось назад
				pair<int, int> enemyPos = getNextPosByDir(i, j, d);
				if (!inBounds(enemyPos.first, enemyPos.second)) continue; // Ворог має бути у межах дошки
				Figure& enemy = checker.getFigures()[enemyPos.first][enemyPos.second];
				if (enemy.getState() != FigureState::none && // Ворог не порожня клітинка
					enemy.getSide() != startFigure.getSide() && // Ворог має бути іншого кольору
					!curPath.isAlreadyCaptured(enemyPos.first, enemyPos.second)) { // Ворог ще не захоплений
					pair<int, int> behind = getNextPosByDir(enemyPos.first, enemyPos.second, d); // Клітка позаду ворога
					if (!inBounds(behind.first, behind.second)) continue;
					if (checker.getFigures()[behind.first][behind.second].getState() == FigureState::none) { // Порожня клітинка за ворожою
						vector<pair<int, int>>& newVisited = visited;
						AttackPath newPath = curPath;
						newPath.addCaptured(enemyPos.first, enemyPos.second); // Додаємо захоплену
						int result = figureAnalisis(behind.first, behind.second, d,
							  startFigure, drawing, true, 
							  attackCount + 1, newPath, bestPath, newVisited); // Продовжуємо атаку
						if (result > maxNext) {
							maxNext = result;
							if (result > bestPath.attackCount) bestPath = newPath; // Оновлення кращого шляху
						}
					}
				}
			}
			visited.pop_back();
			return maxNext;
		}
	} else { // Клітина з фігурою
		if (curFig.getSide() == startFigure.getSide()) { // Своя фігура
			visited.pop_back();
			return attackCount; // Зупиняємося
		} else if (!attacking) { // Ворожа фігура, але ми ще не атакували
			auto behind = getNextPosByDir(i, j, dir);
			if (!inBounds(behind.first, behind.second)) {
				visited.pop_back();
				return attackCount;
			}
			if (checker.getFigures()[behind.first][behind.second].getState() == FigureState::none) { // Можна атакувати
				vector<pair<int, int>> newVisited = visited;
				AttackPath newPath = curPath;
				newPath.addCaptured(i, j); // Додаємо захоплену фігуру
				int result = figureAnalisis(behind.first, behind.second, dir,
										   startFigure, drawing, true, 
										   attackCount + 1, newPath, bestPath, newVisited); // Продовжуємо атаку
				if (result > bestPath.attackCount) bestPath = newPath; // Оновлення кращого шляху
				visited.pop_back();
				return result;
			}
		}
		visited.pop_back();
		return attackCount; // Не можемо атакувати
	}
	visited.pop_back();
	return attackCount; // Запасний вихід
}

void Interface::showPossibleMovesAndAttacks() {
	drowBoard("===== MOVES AND ATTACKS =====");
	cout << "\nEnter position (e.g., C6): ";
	string selectPos; cin >> selectPos; // Введення позиції користувачем
	
	int row, col;
	if (!checker.transformPosition(selectPos, col, row)) throw "Invalid position!"; // Перетворення позиції
	Figure& figure = checker.getFigures()[row][col]; // Отримання фігури
	
	AttackPath emptyPath, bestPath;
	vector<pair<int, int>> visited;
	emptyPath.addPosition(row, col); // Додаємо стартову позицію
	int maxAttacks = figureAnalisis(row, col, Direction::none, figure, false, false, 0, emptyPath, bestPath, visited); // Запуск аналізу
	
	if (maxAttacks > 0) { // Є можливі атаки
		clearTiles();
		setBackgroudOfTile(row, col, TileMoment::active); // Виділення активної фігури
		for (const auto& cap : bestPath.captured) setBackgroudOfTile(cap.first, cap.second, TileMoment::attack); // Загрожені фігури
		for (const auto& pt : bestPath.path) { // Відображення шляху
			bool isCap = false;
			for (const auto& cap : bestPath.captured)
				if (cap.first == pt.first && cap.second == pt.second) { isCap = true; break; } // Перевірка чи це захоплена фігура
			if (!isCap && !(pt.first == row && pt.second == col)) // Якщо не захоплена і не стартова
				setBackgroudOfTile(pt.first, pt.second, TileMoment::move); // Проміжні клітини
		}
		drowBoard("===== MOVES AND ATTACKS =====");
		
		string color = (figure.getSide() == FigureSide::white) ? "White" : "Black";
		string type = (figure.getState() == FigureState::piece) ? "piece" : "king";
		cout << "\n" << color << " " << type << " at " << selectPos << " threatens:\n"; // Заголовок
		for (const auto& cap : bestPath.captured) { // Вивід загрожених фігур
			char colChar = 'A' + cap.second;
			int rowNum = cap.first + 1;
			Figure& threatened = checker.getFigures()[cap.first][cap.second];
			string tColor = (threatened.getSide() == FigureSide::white) ? "white" : "black";
			string tType = (threatened.getState() == FigureState::piece) ? "piece" : "king";
			cout << "  - " << tColor << " " << tType << " at " << colChar << rowNum << endl;
		}
		cout << "\nAttack path: "; // Вивід шляху атаки
		for (size_t i = 1; i < bestPath.path.size(); i++) {
			cout << char('A' + bestPath.path[i].second) << bestPath.path[i].first + 1;
			if (i < bestPath.path.size() - 1) cout << " -> "; // Роздільник
		}
		if (maxAttacks > 1) cout << "\n" << maxAttacks << " figures can be captured in one turn."; // Більше однієї атаки
	} else { // Немає атак
		clearTiles();
		setBackgroudOfTile(row, col, TileMoment::active); // Виділення активної фігури
		vector<Direction> dirs = getDirections(figure, whiteInDown);
		bool hasMoves = false;
		for (Direction d : dirs) { // Перевірка простих ходів
			auto next = getNextPosByDir(row, col, d);
			if (inBounds(next.first, next.second) && 
				checker.getFigures()[next.first][next.second].getState() == FigureState::none) {
				setBackgroudOfTile(next.first, next.second, TileMoment::move); // Можливий хід
				hasMoves = true;
			}
		}
    drowBoard("===== MOVES AND ATTACKS =====");
		cout << endl << (hasMoves ? "Simple moves are available." : "No moves available from this position."); // Інформація про ходи
	}
	cout << "\n\n[Press Enter to continue]";
	cin.ignore(); cin.get(); // Пауза для користувача
}

void Interface::showThreatsToAllFigures() {
	clearTiles();
	drowBoard("===== THREATS TO ALL FIGURES =====");
	cout << "\nEnter side [white/black] (w/b): ";
	string side; cin >> side; // Вибір сторони для аналізу
	if (side != "w" && side != "white" && side != "b" && side != "black") throw "Invalid side!";
	
	FigureSide targetSide = (side == "w" || side == "white") ? FigureSide::white : FigureSide::black;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			Figure& potential = checker.getFigures()[i][j];
			if (potential.getSide() != targetSide) { // Тільки фігури обраної сторони
				if (potential.getState() == FigureState::none) continue; // Пропускаємо порожні
				AttackPath emptyPath, bestPath;
				vector<pair<int, int>> visited;
				emptyPath.addPosition(i, j); // Додаємо стартову позицію
				if (figureAnalisis(i, j, Direction::none, potential, false, false, 0, emptyPath, bestPath, visited)) {
					setBackgroudOfTile(i, j, TileMoment::active); // Атакуюча фігура
					for (const pair<int, int>& capturedFigure : bestPath.captured) {
						setBackgroudOfTile(capturedFigure.first, capturedFigure.second, TileMoment::attack); // Можливе захоплення цієї фігури
					}
				}
			}
		}
	}
	drowBoard("===== THREATS TO ALL FIGURES =====");
	cout << "\n[Press Enter to continue]";
	cin.ignore(); cin.get(); // Пауза
}

void Interface::whereWhiteSide() {
	cout << "\nWhite side is " << (whiteInDown ? "down." : "up."); // Інформація про розташування білих
	cout << "\n[Press Enter to continue]";
	cin.ignore(); cin.get(); // Пауза
}

void Interface::setWhiteSide() {
  cout << endl << "White side will be in down? (y/n): ";
  whiteInDown = (cin.get() == 'y'); // Визначає розташування білих шашок
  cin.ignore();
}

void Interface::showMenu() {
	while (true) {
		clearTiles();
		drowBoard("===== CHECKERS MENU =====");
		cout << "\n1. Analyze figure moves and attacks\n2. Analyze threats to all figures\n3. Where is white side?\n"
       << "4. Change white side\n5. Exit\n\nChoice: "; // Меню
		
		int choice;
		if (!(cin >> choice)) { // Обробка некоректного вводу
			cin.clear(); cin.ignore(1000, '\n');
			continue;
		}
		cin.ignore();
		
		switch (choice) { // Обробка вибору користувача
			case 1: showPossibleMovesAndAttacks(); break;
			case 2: showThreatsToAllFigures(); break;
			case 3: whereWhiteSide(); break;
      case 4: setWhiteSide(); break;
			case 5: return; // Вихід
			default: break;
		}
	}
}