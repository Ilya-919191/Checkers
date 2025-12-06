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

void AttackPath::addPosition(int i, int j) {
	path.push_back({i, j});
}
void AttackPath::addCaptured(int i, int j) {
	captured.push_back({i, j});
	attackCount++;
}

bool AttackPath::isAlreadyCaptured(int i, int j) const {
	if (!captured.empty()) {
		for (const pair<int, int>& cap : captured) {
			if (cap.first == i && cap.second == j) {
				return true;
			}
		}
	}
	return false;
}

void Interface::setWhiteSide() {
	int choice;
	cout << "White side will be in down? (y/n): ";
	cin >> choice;

  if (choice == 'y') {
    whiteInDown = true; // Білі внизу
  } else {
    whiteInDown = false; // Білі вгорі
  }
}

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

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			Board **board = checker.getBoard();
			Figure **figures = checker.getFigures();

			board[i][j].setSymbol(((i + j) % 2 != 0 ) ? colorBackgroundWhite : colorBackgroundBlack);

			if (board[i][j].getTile() == TileMoment::none) {
				board[i][j].setPositionByNums(i, j);
				figures[i][j].setPositionByNums(i, j);
			}
		}

	}
}

void Interface::clearTiles() {
  // Очищаємо всі позначки
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      setBackgroudOfTile(i, j, TileMoment::none);
    }
  }
}

void Interface::drowBoard(string text)
{
	CLS();
  cout << text << endl << endl;

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
		default:                     return Direction::none;
	}
}

pair<int,int> Interface::getNextPosByDir(int i, int j, Direction direction) {
	switch (direction) {
		case Direction::topRight:    return {i + 1, j + 1};
		case Direction::bottomRight: return {i - 1, j + 1};
		case Direction::topLeft:     return {i + 1, j - 1};
		case Direction::bottomLeft:  return {i - 1, j - 1};
		default:                     return {-1, -1};
	}
}

Figure& Interface::getNextFigureByDir(int i, int j, Direction direction) {
	pair<int,int> pos = getNextPosByDir(i, j, direction);

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
		checker.getBoard()[i][j].setSymbol( ((i + j) % 2 != 0 ) ? colorBackgroundWhite : colorBackgroundBlack);

	checker.getBoard()[i][j].setTile(moment);
}

int Interface::figureAnalisis(int i, int j, Direction direction, Figure& startFigure, 
                              bool isDrawing, bool isAttacking, int attackCount, AttackPath& currentPath,
                              AttackPath& bestPath, vector<pair<int, int>>& visitedPositions)
{
  // Якщо клітка вийшла за межі поля.
  if (!inBounds(i, j)) return attackCount;
  
  // Перевіряємо, чи вже відвідували цю позицію на поточному шляху
  if (find(visitedPositions.begin(), visitedPositions.end(), make_pair(i, j)) != visitedPositions.end()) {
    return attackCount; // Якщо вже відвідували - повертаємось
  }
  
  // Додаємо поточну позицію до відвіданих
  visitedPositions.push_back({i, j});
  
  // ВАЖЛИВО: Завжди додаємо позицію до поточного шляху!
  currentPath.addPosition(i, j);
  
  // Поточна фігура.
  Figure& currentFigure = checker.getFigures()[i][j];

  // Визначаємо доступні напрямки залежно від типу фігури.
  vector<Direction> availableDirections;
  
  // Якщо вказана фігура - 'King'.
  if (startFigure.getState() == FigureState::king) {
    availableDirections = {
      Direction::topRight, Direction::bottomRight, 
      Direction::topLeft, Direction::bottomLeft
    };
  } else {
    if (isWhiteInDown()) {
      // Якщо вказана фігура - 'Piece'.
      if (startFigure.getSide() == FigureSide::white) {
        availableDirections = {Direction::topRight, Direction::topLeft};
      } else {
        availableDirections = {Direction::bottomRight, Direction::bottomLeft};
      }
    } else {
      if (startFigure.getSide() == FigureSide::white) {
        availableDirections = {Direction::bottomRight, Direction::bottomLeft};
      } else {
        availableDirections = {Direction::topRight, Direction::topLeft};
      }
    }
  }
  
  //=== Якщо це перший виклик.
  if (direction == Direction::none)
  {
    int maxResult = 0;
    AttackPath localBestPath = currentPath;
    
    // Збираємо результати для всіх доступних напрямків.
    for (Direction newDir : availableDirections)
    {
      // Позиція наступної фігури.
      pair<int, int> nextPos = getNextPosByDir(i, j, newDir);
      if (inBounds(nextPos.first, nextPos.second))
      {
        // Копіюємо відвідані позиції для кожного нового шляху
        vector<pair<int, int>> newVisited = visitedPositions;
        
        // Створюємо новий шлях для цього напрямку.
        AttackPath newPath = currentPath;
        int result = figureAnalisis(nextPos.first, nextPos.second, newDir, 
          startFigure, isDrawing, false, 0, newPath, bestPath, newVisited);
        
        if (result > maxResult) {
          maxResult = result;
          localBestPath = newPath;
          
          // Оновлюємо глобальний найкращий шлях
          if (result > bestPath.attackCount) {
            bestPath = newPath; // Копіюємо ВЕСЬ шлях
          }
        }
      }
    } 
    
    // Видаляємо поточну позицію з відвіданих перед поверненням
    visitedPositions.pop_back();
    
    return maxResult;
  }

  //=== Якщо клітка порожня.
  if (currentFigure.getState() == FigureState::none)
  {   
    // Якщо фігура ще не атакувала - рухаємось по тій же діагоналі.
    if (!isAttacking) {
      pair<int, int> nextPos = getNextPosByDir(i, j, direction);
      
      // Видаляємо поточну позицію з відвіданих перед переходом
      visitedPositions.pop_back();
      
      return figureAnalisis(nextPos.first, nextPos.second, direction, 
        startFigure, isDrawing, false, attackCount, currentPath, bestPath, visitedPositions);
    }
    // Якщо актака вже відбувалась - пошук наступної.
    else {
      int maxNextAttacks = attackCount;
      
      for (Direction newDir : availableDirections)
      {
        // Пропускаємо напрямок, з якого ми прийшли.
        if (newDir == reversDir(direction)) continue;
                    
        // Перевіряємо, чи є клітка із ворожою фігурою за напрямком.
        pair<int, int> enemyPos = getNextPosByDir(i, j, newDir);
        if (!inBounds(enemyPos.first, enemyPos.second)) continue;
        
        // Ворожа фігура.
        Figure& enemyFigure = checker.getFigures()[enemyPos.first][enemyPos.second];
      
        // Перевіряємо, чи це ворожа фігура і чи ще не захоплена на цьому шляху.
        if (enemyFigure.getState() != FigureState::none && enemyFigure.getSide() != startFigure.getSide() &&
          !currentPath.isAlreadyCaptured(enemyPos.first, enemyPos.second))
        {
          // Перевіряємо, чи є пуста клітка за ворожою фігурою.
          pair<int, int> behindPos = getNextPosByDir(enemyPos.first, enemyPos.second, newDir);
          if (!inBounds(behindPos.first, behindPos.second)) continue;
  
          // Фігура за ворожою.
          Figure& behindFigure = checker.getFigures()[behindPos.first][behindPos.second];
          if (behindFigure.getState() == FigureState::none)
          {
            // Копіюємо відвідані позиції для нового шляху
            vector<pair<int, int>> newVisited = visitedPositions;
            
            // Додаємо захоплену фігуру до шляху.
            AttackPath newPath = currentPath;
            newPath.addCaptured(enemyPos.first, enemyPos.second);
            
            // Продовжуємо рекурсію з нової позиції.
            int newAttackCount = figureAnalisis(behindPos.first, behindPos.second, newDir,
              startFigure, isDrawing, true, attackCount + 1, newPath, bestPath, newVisited);
            
            // Оновлюємо кращий шлях.
            if (newAttackCount > maxNextAttacks) {
              maxNextAttacks = newAttackCount;

              if (newAttackCount > bestPath.attackCount) {
                bestPath = newPath; // Копіюємо ВЕСЬ шлях
              }
            }
          }
        }
      }
      // Видаляємо поточну позицію з відвіданих перед поверненням.
      visitedPositions.pop_back();
      
      return maxNextAttacks;
    }
  }

  //=== Якщо клітка містить фігуру.
  else {
    // Якщо це наша фігура - зупиняємось.
    if (currentFigure.getSide() == startFigure.getSide()) {
      visitedPositions.pop_back();
      return attackCount;
    }
    // Якщо це ворожа фігура і ми ще не атакували.
    else if (!isAttacking)
    {
      // Перевіряємо, чи можемо атакувати цю фігуру.
      pair<int, int> behindPos = getNextPosByDir(i, j, direction);
      if (!inBounds(behindPos.first, behindPos.second)) {
        visitedPositions.pop_back();
        return attackCount;
      }
      
      Figure& behindFigure = checker.getFigures()[behindPos.first][behindPos.second];
          
      // Перевіряємо, чи пуста клітка за ворожою фігурою.
      if (behindFigure.getState() == FigureState::none)
      {
        // Копіюємо відвідані позиції для нового шляху
        vector<pair<int, int>> newVisited = visitedPositions;
        
        // Додаємо захоплену фігуру до шляху.
        AttackPath newPath = currentPath;
        newPath.addCaptured(i, j);
        
        // Продовжуємо рекурсію з нової позиції.
        int result = figureAnalisis(behindPos.first, behindPos.second, direction, startFigure,
          isDrawing, true, attackCount + 1, newPath, bestPath, newVisited);
          
        // Оновлюємо кращий шлях.
        if (result > bestPath.attackCount) {
          bestPath = newPath; // Копіюємо ВЕСЬ шлях
        }
        
        // Видаляємо поточну позицію з відвіданих перед поверненням
        visitedPositions.pop_back();
        
        return result;
      }
    }
    // Якщо це ворожа фігура і ми вже атакуємо - не можна стрибати через дві фігури поспіль.
    else {
      visitedPositions.pop_back();
      return attackCount;
    }
  }

  // Запасний вихід
  visitedPositions.pop_back();
  return attackCount;
}

void Interface::showPossibleMovesAndAttacks()
{
  const string nameSection = "===== MOVES AND ATTACKS =====";
  drowBoard(nameSection);

  string selectPos;
  cout << endl << "Enter position (e.g., C6): ";
  cin >> selectPos;
  
  int row = 0, col = 0;
  if (!checker.transformPosition(selectPos, col, row)) {
      throw "Invalid position!";
  }
  Figure& figure = checker.getFigures()[row][col];

  AttackPath emptyPath;
  AttackPath bestPath;
  vector<pair<int, int>> visitedPositions;

  // Додаємо стартову позицію до порожнього шляху
  emptyPath.addPosition(row, col);

  int maxAttacks = figureAnalisis(row, col, Direction::none, figure, 
                                  false, false, 0, emptyPath,
                                  bestPath, visitedPositions);

  vector<Direction> availableDirections;

  // Якщо вказана фігура - 'King'.
  if (figure.getState() == FigureState::king) {
    availableDirections = {
      Direction::topRight, Direction::bottomRight, 
      Direction::topLeft, Direction::bottomLeft
    };
  } else {
    if (isWhiteInDown()) {
      // Якщо вказана фігура - 'Piece'.
      if (figure.getSide() == FigureSide::white) {
        availableDirections = {Direction::topRight, Direction::topLeft};
      } else {
        availableDirections = {Direction::bottomRight, Direction::bottomLeft};
      }
    } else {
      if (figure.getSide() == FigureSide::white) {
        availableDirections = {Direction::bottomRight, Direction::bottomLeft};
      } else {
        availableDirections = {Direction::topRight, Direction::topLeft};
      }
    }
  }
  
  if (maxAttacks > 0) {
    clearTiles();
    
    // Відрисовка початкової позиції.
    setBackgroudOfTile(row, col, TileMoment::active);
    
    // Позначаємо захоплені фігури.
    for (const auto& cap : bestPath.captured) {
      setBackgroudOfTile(cap.first, cap.second, TileMoment::attack);
    }
    
    // Позначаємо шлях переміщення.
    for (const auto& pt : bestPath.path) {
      int pathRow = pt.first;
      int pathCol = pt.second;
      
      // Перевіряємо, чи це не початкова позиція і не захоплена фігура.
      bool isCaptured = false;
      for (const auto& cap : bestPath.captured) {
        if (cap.first == pathRow && cap.second == pathCol) {
          isCaptured = true;
          break;
        }
      }
      
      if (!isCaptured && !(pathRow == row && pathCol == col)) {
        setBackgroudOfTile(pathRow, pathCol, TileMoment::move);
      }
    }
    
    drowBoard(nameSection);
    
    // Виводить інформацію про захоплені фігури.
    string color = (figure.getSide() == FigureSide::white) ? "White" : "Black";
    string type = (figure.getState() == FigureState::piece) ? "piece" : "king";
  
    cout << endl << color << " " << type << " at " << selectPos << " threatens:" << endl;
    for (const auto& cap : bestPath.captured)
    {
      // Вказанні позицій захоплених фігур.
      char colChar = 'A' + cap.second;
      int rowNum = cap.first + 1;

      Figure& threatened = checker.getFigures()[cap.first][cap.second];
      string threatColor = (threatened.getSide() == FigureSide::white) ? "white" : "black";
      string threatType = (threatened.getState() == FigureState::piece) ? "piece" : "king";
      
      cout << "  - " << threatColor << " " << threatType << " at " << colChar << rowNum << endl;
    }
    
    cout << "\nAttack path: ";
    for (size_t i = 1; i < bestPath.path.size(); i++) {
      char colChar = 'A' + bestPath.path[i].second;
      int rowNum = bestPath.path[i].first + 1;
      
      cout << colChar << rowNum;
      if (i < bestPath.path.size() - 1) cout << " -> ";
    }
    cout << endl;
    
    if (maxAttacks > 1) {
      cout <<endl << maxAttacks << " figures can be captured in one turn." << endl;
    }
    
  } else {
    cout << "No attacks possible from this position." << endl;
    
    // Очищаємо всі позначки
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        setBackgroudOfTile(i, j, TileMoment::none);
      }
    }
    
    // Тоді позначаємо можливі її варіації ходів.
    setBackgroudOfTile(row, col, TileMoment::active);

    bool hasMoves = false;
    for (Direction newDir : availableDirections) {
      pair<int, int> nextPos = getNextPosByDir(row, col, newDir);
      if (inBounds(nextPos.first, nextPos.second)) {
        Figure& nextFigure = checker.getFigures()[nextPos.first][nextPos.second];
        if (nextFigure.getState() == FigureState::none) {
          setBackgroudOfTile(nextPos.first, nextPos.second, TileMoment::move);
          hasMoves = true;
        }
      }
    }
    
    if (hasMoves) {
      drowBoard();
      cout << "Simple moves are available." << endl;
    } else {
      drowBoard();
      cout << "No moves available from this position." << endl;
    }
  }
  
  cout << endl << "[Press Enter to continue]" << endl;
  cin.ignore();
  cin.get();
  return;
}

void Interface::showThreatsToFigure()
{
  clearTiles();

  const string nameSection = "===== THREATS TO FIGURE =====";
  drowBoard(nameSection);

  string selectPos;
  cout << endl << "Enter position (e.g., C6): ";
  cin >> selectPos;
  
  int row = 0, col = 0;
  if (!checker.transformPosition(selectPos, col, row)) {
    cout << "Invalid position!" << endl;
    cin.ignore();
    cin.get();
    return;
  }

  vector<pair<string, pair<int, int>>> threateningFigures;
  Figure& targetFigure = checker.getFigures()[row][col];

  // Перевіряємо всі фігури на дошці
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Figure& potentialThreat = checker.getFigures()[i][j];
      
      // Пропускаємо порожні клітини та фігури того ж кольору
      if (potentialThreat.getState() == FigureState::none) continue;
      if (potentialThreat.getSide() == targetFigure.getSide()) continue;
      
      // Перевіряємо чи може ця фігура атакувати цільову фігуру
      if (canAttackTarget(i, j, row, col, potentialThreat)) {
        char colChar = 'A' + j;
        int rowNum = i + 1;
        string pos = string(1, colChar) + to_string(rowNum);
        
        string threatColor = (potentialThreat.getSide() == FigureSide::white) ? "White" : "Black";
        string threatType = (potentialThreat.getState() == FigureState::piece) ? "piece" : "king";
        
        threateningFigures.push_back({threatColor + " " + threatType + " at " + pos, {i, j}});
      }
    }
  }

  // Візуалізуємо загрози на дошці
  visualizeThreats(row, col, threateningFigures);
  drowBoard(nameSection);
  
  // Перевіряємо, чи є фігура на цій позиції
  if (targetFigure.getState() == FigureState::none) {
    cerr << "No figure at position " << selectPos << "!" << endl;
    cin.ignore();
    cin.get();
    return;
  }
  
  string color = (targetFigure.getSide() == FigureSide::white) ? "White" : "Black";
  string type = (targetFigure.getState() == FigureState::piece) ? "piece" : "king";

  cout << endl << color << " " << type << " at " << selectPos  << endl;

  if (!threateningFigures.empty()) {
    cout << "\nThis figure is threatened by " << threateningFigures.size() << " opponent figures:" << endl;
    for (const auto& threat : threateningFigures) {
      cout << "  - " << threat.first << endl;
    }
  } else {
    drowBoard(nameSection);
    cout << endl << "This figure is not under direct threat from any opponent figures." << endl;
  }
  
  cout << endl << "[Press Enter to continue]" << endl;
  cin.ignore();
  cin.get();
}

bool Interface::canAttackTarget(int attackerRow, int attackerCol, int targetRow, int targetCol, Figure& attacker)
{
  // Якщо атакуюча фігура - король
  if (attacker.getState() == FigureState::king) {
    // Король може атакувати з будь-якого напрямку
    Direction directions[4] = {
      Direction::topRight, Direction::bottomRight, 
      Direction::topLeft, Direction::bottomLeft
    };
    
    for (Direction dir : directions) {
      // Перевіряємо чи є цільова фігура в цьому напрямку
      int checkRow = attackerRow;
      int checkCol = attackerCol;

      while (inBounds(checkRow, checkCol)) {
        pair<int, int> nextPos = getNextPosByDir(checkRow, checkCol, dir);
        if (!inBounds(nextPos.first, nextPos.second)) break;
        
        Figure& nextFigure = checker.getFigures()[nextPos.first][nextPos.second];
        
        // Якщо знайшли цільову фігуру
        if (nextPos.first == targetRow && nextPos.second == targetCol) {
          // Перевіряємо чи є вільна клітка за нею
          pair<int, int> behindPos = getNextPosByDir(targetRow, targetCol, dir);
          if (!inBounds(behindPos.first, behindPos.second)) break;
          
          Figure& behindFigure = checker.getFigures()[behindPos.first][behindPos.second];
          if (behindFigure.getState() == FigureState::none) {
            return true;
          }
          break;
        }
        
        // Якщо зустріли іншу фігуру - не можемо атакувати через неї
        if (nextFigure.getState() != FigureState::none) {
          break;
        }
        
        checkRow = nextPos.first;
        checkCol = nextPos.second;
      }
    }
  } 
  // Якщо атакуюча фігура - звичайна шашка
  else {
    // Визначаємо доступні напрямки для атаки
    vector<Direction> availableDirections;
    
    if (isWhiteInDown()) {
      if (attacker.getSide() == FigureSide::white) {
        availableDirections = {Direction::topRight, Direction::topLeft};
      } else {
        availableDirections = {Direction::bottomRight, Direction::bottomLeft};
      }
    } else {
      if (attacker.getSide() == FigureSide::white) {
        availableDirections = {Direction::bottomRight, Direction::bottomLeft};
      } else {
        availableDirections = {Direction::topRight, Direction::topLeft};
      }
    }
    
    for (Direction dir : availableDirections) {
      // Перевіряємо чи є ворожа фігура поруч
      pair<int, int> enemyPos = getNextPosByDir(attackerRow, attackerCol, dir);
      if (!inBounds(enemyPos.first, enemyPos.second)) continue;
      
      // Якщо це цільова фігура
      if (enemyPos.first == targetRow && enemyPos.second == targetCol) {
        // Перевіряємо чи є вільна клітка за нею
        pair<int, int> behindPos = getNextPosByDir(targetRow, targetCol, dir);
        if (!inBounds(behindPos.first, behindPos.second)) continue;
        
        Figure& behindFigure = checker.getFigures()[behindPos.first][behindPos.second];
        if (behindFigure.getState() == FigureState::none) {
          return true;
        }
      }
    }
  }
  return false;
}

// Метод для візуалізації загроз
void Interface::visualizeThreats(int targetRow, int targetCol, 
                                const vector<pair<string, pair<int, int>>>& threats)
{ 
  // Позначаємо цільову фігуру
  setBackgroudOfTile(targetRow, targetCol, TileMoment::active);
  
  // Позначаємо загрозні фігури
  for (const auto& threat : threats) {
    int threatRow = threat.second.first;
    int threatCol = threat.second.second;
    setBackgroudOfTile(threatRow, threatCol, TileMoment::attack);
  }
  
  for (const auto& threat : threats) {
    int threatRow = threat.second.first;
    int threatCol = threat.second.second;
    
    // Знаходимо напрямок атаки
    Direction attackDir = findAttackDirection(threatRow, threatCol, targetRow, targetCol);
    if (attackDir != Direction::none) {
      // Малюємо лінію атаки
      drawAttackLine(threatRow, threatCol, targetRow, targetCol, attackDir);
    }
  }
}

// Допоміжний метод для знаходження напрямку атаки
Direction Interface::findAttackDirection(int fromRow, int fromCol, int toRow, int toCol)
{
  // Визначаємо різницю координат
  int rowDiff = toRow - fromRow;
  int colDiff = toCol - fromCol;
  
  // Перевіряємо всі можливі напрямки
  if (rowDiff > 0 && colDiff > 0 && abs(rowDiff) == abs(colDiff)) {
    return Direction::topRight;
  } else if (rowDiff < 0 && colDiff > 0 && abs(rowDiff) == abs(colDiff)) {
    return Direction::bottomRight;
  } else if (rowDiff > 0 && colDiff < 0 && abs(rowDiff) == abs(colDiff)) {
    return Direction::topLeft;
  } else if (rowDiff < 0 && colDiff < 0 && abs(rowDiff) == abs(colDiff)) {
    return Direction::bottomLeft;
  }
  
  return Direction::none;
}

// Допоміжний метод для малювання лінії атаки
void Interface::drawAttackLine(int fromRow, int fromCol, int toRow, int toCol, Direction dir)
{
  int currentRow = fromRow;
  int currentCol = fromCol;
  
  while (currentRow != toRow || currentCol != toCol) {
    // Позначаємо проміжні клітини
    if (!(currentRow == fromRow && currentCol == fromCol) &&
        !(currentRow == toRow && currentCol == toCol)) {
      setBackgroudOfTile(currentRow, currentCol, TileMoment::move);
    }
    
    // Переходимо до наступної клітини
    pair<int, int> nextPos = getNextPosByDir(currentRow, currentCol, dir);
    if (!inBounds(nextPos.first, nextPos.second)) break;
    
    currentRow = nextPos.first;
    currentCol = nextPos.second;
  }
}

void Interface::whereWhiteSide() {
  CLS();
  cout << endl << ((whiteInDown) ? "White side is down." : "White side is up.");
  cout << endl << "[Press Enter to continue]" << endl;
  cin.ignore();
  cin.get();
}

void Interface::showMenu() {
  while (true) {
    clearTiles();
    drowBoard("===== CHECKERS MENU =====");
    cout << endl << "1. Analyze figure moves and attacks" << endl;
    cout << "2. Analyze threats to figure" << endl;
    cout << "3. Where is white side?" << endl;
    cout << "4. Exit" << endl;
    cout << endl << "Choice: ";
    
    int choice;
    if (!(cin >> choice)) {
      cin.clear();
      cin.ignore();
      cerr << "Invalid input! Please enter a number (1-3)." << endl;
      continue;
    }
    
    switch (choice) {
      case 1: showPossibleMovesAndAttacks(); break;
      case 2: showThreatsToFigure(); break;
      case 3: whereWhiteSide(); break;
      case 4: return;
    }
  }
}