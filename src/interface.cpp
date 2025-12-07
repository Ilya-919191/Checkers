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

// AttackPath methods
void AttackPath::addPosition(int i, int j) { path.push_back({i, j}); }
void AttackPath::addCaptured(int i, int j) { captured.push_back({i, j}); attackCount++; }
bool AttackPath::isAlreadyCaptured(int i, int j) const {
    for (const auto& cap : captured)
        if (cap.first == i && cap.second == j) return true;
    return false;
}

// Interface methods
void Interface::setWhiteSide() {
    cout << "White side will be in down? (y/n): ";
    whiteInDown = (cin.get() == 'y');
    cin.ignore();
}

Interface::Interface(CheckerBoard& checkerBoard) : checker(checkerBoard) {}

void Interface::fillBoardFromFile(string fileName) {
    ifstream file(fileName);
    if (!file.is_open()) throw "File was not open.";

    string sideStr, stateStr, position;
    while (file >> sideStr >> stateStr >> position) {
        Figure figure;
        figure.setSide(sideStr == "white" ? FigureSide::white : FigureSide::black);
        figure.setState(stateStr == "piece" ? FigureState::piece : FigureState::king);
        figure.setSymbol(format("{}{}{}", 
            figure.getSide() == FigureSide::white ? colorWhite : colorBlack,
            figure.getState() == FigureState::piece ? symbolPiece : symbolKing,
            colorEnd));
        figure.setPosition(position);
        checker.getFigures()[figure.getX()][figure.getY()] = figure;
    }

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            checker.getBoard()[i][j].setSymbol(((i + j) % 2 != 0) ? colorBackgroundWhite : colorBackgroundBlack);
            if (checker.getBoard()[i][j].getTile() == TileMoment::none) {
                checker.getBoard()[i][j].setPositionByNums(i, j);
                checker.getFigures()[i][j].setPositionByNums(i, j);
            }
        }
}

void Interface::clearTiles() {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            setBackgroudOfTile(i, j, TileMoment::none);
}

void Interface::drowBoard(string text) {
    CLS();
    cout << text << "\n\n";
    for (int i = 7; i >= 0; i--) {
        cout << "\033[092m " << i + 1 << " " << colorEnd;
        for (int j = 0; j < 8; j++)
            cout << checker.getBoard()[i][j].getSymbol() << 
                    checker.getFigures()[i][j].getSymbol() << 
                    checker.getBoard()[i][j].getSymbol() << colorEnd;
        cout << endl;
    }
    cout << "\033[092m    ";
    for (char j = 'A'; j < 'I'; j++) cout << j << "  ";
    cout << "\033[0m\n";
}

bool inBounds(int i, int j) { return i >= 0 && i < 8 && j >= 0 && j < 8; }

Direction Interface::reversDir(Direction d) {
    switch (d) {
        case Direction::topRight: return Direction::bottomLeft;
        case Direction::bottomRight: return Direction::topLeft;
        case Direction::topLeft: return Direction::bottomRight;
        case Direction::bottomLeft: return Direction::topRight;
        default: return Direction::none;
    }
}

pair<int,int> Interface::getNextPosByDir(int i, int j, Direction d) {
    switch (d) {
        case Direction::topRight: return {i + 1, j + 1};
        case Direction::bottomRight: return {i - 1, j + 1};
        case Direction::topLeft: return {i + 1, j - 1};
        case Direction::bottomLeft: return {i - 1, j - 1};
        default: return {-1, -1};
    }
}

void Interface::setBackgroudOfTile(int i, int j, TileMoment m) {
    if (!inBounds(i, j)) return;
    string color;
    if (m == TileMoment::attack) color = colorAttack;
    else if (m == TileMoment::active) color = colorActive;
    else if (m == TileMoment::move) color = colorMove;
    else if (m == TileMoment::finish) color = colorFinish;
    else color = ((i + j) % 2 != 0) ? colorBackgroundWhite : colorBackgroundBlack;
    checker.getBoard()[i][j].setSymbol(color);
    checker.getBoard()[i][j].setTile(m);
}

// Helper function to get available directions
vector<Direction> getDirections(Figure& figure, bool whiteDown) {
    if (figure.getState() == FigureState::king)
        return {Direction::topRight, Direction::bottomRight, Direction::topLeft, Direction::bottomLeft};
    
    bool isWhite = figure.getSide() == FigureSide::white;
    if (whiteDown) 
        return isWhite ? vector{Direction::topRight, Direction::topLeft} : 
                         vector{Direction::bottomRight, Direction::bottomLeft};
    else 
        return isWhite ? vector{Direction::bottomRight, Direction::bottomLeft} : 
                         vector{Direction::topRight, Direction::topLeft};
}

int Interface::figureAnalisis(int i, int j, Direction dir, Figure& startFigure, 
                              bool drawing, bool attacking, int attackCount, 
                              AttackPath& curPath, AttackPath& bestPath,
                              vector<pair<int, int>>& visited) {
    if (!inBounds(i, j)) return attackCount;
    if (find(visited.begin(), visited.end(), make_pair(i, j)) != visited.end()) return attackCount;
    
    visited.push_back({i, j});
    curPath.addPosition(i, j);
    Figure& curFig = checker.getFigures()[i][j];
    vector<Direction> dirs = getDirections(startFigure, whiteInDown);
    
    if (dir == Direction::none) {
        int maxResult = 0;
        AttackPath localBest = curPath;
        for (Direction d : dirs) {
            auto next = getNextPosByDir(i, j, d);
            if (!inBounds(next.first, next.second)) continue;
            vector<pair<int, int>> newVisited = visited;
            AttackPath newPath = curPath;
            int result = figureAnalisis(next.first, next.second, d, startFigure, 
                                       drawing, false, 0, newPath, bestPath, newVisited);
            if (result > maxResult) {
                maxResult = result;
                localBest = newPath;
                if (result > bestPath.attackCount) bestPath = newPath;
            }
        }
        visited.pop_back();
        return maxResult;
    }
    
    if (curFig.getState() == FigureState::none) {
        if (!attacking) {
            auto next = getNextPosByDir(i, j, dir);
            visited.pop_back();
            return figureAnalisis(next.first, next.second, dir, startFigure, 
                                 drawing, false, attackCount, curPath, bestPath, visited);
        } else {
            int maxNext = attackCount;
            for (Direction d : dirs) {
                if (d == reversDir(dir)) continue;
                auto enemyPos = getNextPosByDir(i, j, d);
                if (!inBounds(enemyPos.first, enemyPos.second)) continue;
                Figure& enemy = checker.getFigures()[enemyPos.first][enemyPos.second];
                if (enemy.getState() != FigureState::none && 
                    enemy.getSide() != startFigure.getSide() &&
                    !curPath.isAlreadyCaptured(enemyPos.first, enemyPos.second)) {
                    auto behind = getNextPosByDir(enemyPos.first, enemyPos.second, d);
                    if (!inBounds(behind.first, behind.second)) continue;
                    if (checker.getFigures()[behind.first][behind.second].getState() == FigureState::none) {
                        vector<pair<int, int>> newVisited = visited;
                        AttackPath newPath = curPath;
                        newPath.addCaptured(enemyPos.first, enemyPos.second);
                        int result = figureAnalisis(behind.first, behind.second, d,
                                                   startFigure, drawing, true, 
                                                   attackCount + 1, newPath, bestPath, newVisited);
                        if (result > maxNext) {
                            maxNext = result;
                            if (result > bestPath.attackCount) bestPath = newPath;
                        }
                    }
                }
            }
            visited.pop_back();
            return maxNext;
        }
    } else {
        if (curFig.getSide() == startFigure.getSide()) {
            visited.pop_back();
            return attackCount;
        } else if (!attacking) {
            auto behind = getNextPosByDir(i, j, dir);
            if (!inBounds(behind.first, behind.second)) {
                visited.pop_back();
                return attackCount;
            }
            if (checker.getFigures()[behind.first][behind.second].getState() == FigureState::none) {
                vector<pair<int, int>> newVisited = visited;
                AttackPath newPath = curPath;
                newPath.addCaptured(i, j);
                int result = figureAnalisis(behind.first, behind.second, dir,
                                           startFigure, drawing, true, 
                                           attackCount + 1, newPath, bestPath, newVisited);
                if (result > bestPath.attackCount) bestPath = newPath;
                visited.pop_back();
                return result;
            }
        }
        visited.pop_back();
        return attackCount;
    }
    visited.pop_back();
    return attackCount;
}

void Interface::showPossibleMovesAndAttacks() {
    drowBoard("===== MOVES AND ATTACKS =====");
    cout << "\nEnter position (e.g., C6): ";
    string selectPos; cin >> selectPos;
    
    int row, col;
    if (!checker.transformPosition(selectPos, col, row)) throw "Invalid position!";
    Figure& figure = checker.getFigures()[row][col];
    
    AttackPath emptyPath, bestPath;
    vector<pair<int, int>> visited;
    emptyPath.addPosition(row, col);
    int maxAttacks = figureAnalisis(row, col, Direction::none, figure, false, false, 0, emptyPath, bestPath, visited);
    
    if (maxAttacks > 0) {
        clearTiles();
        setBackgroudOfTile(row, col, TileMoment::active);
        for (const auto& cap : bestPath.captured) setBackgroudOfTile(cap.first, cap.second, TileMoment::attack);
        for (const auto& pt : bestPath.path) {
            bool isCap = false;
            for (const auto& cap : bestPath.captured)
                if (cap.first == pt.first && cap.second == pt.second) { isCap = true; break; }
            if (!isCap && !(pt.first == row && pt.second == col))
                setBackgroudOfTile(pt.first, pt.second, TileMoment::move);
        }
        drowBoard("===== MOVES AND ATTACKS =====");
        
        string color = (figure.getSide() == FigureSide::white) ? "White" : "Black";
        string type = (figure.getState() == FigureState::piece) ? "piece" : "king";
        cout << "\n" << color << " " << type << " at " << selectPos << " threatens:\n";
        for (const auto& cap : bestPath.captured) {
            char colChar = 'A' + cap.second;
            int rowNum = cap.first + 1;
            Figure& threatened = checker.getFigures()[cap.first][cap.second];
            string tColor = (threatened.getSide() == FigureSide::white) ? "white" : "black";
            string tType = (threatened.getState() == FigureState::piece) ? "piece" : "king";
            cout << "  - " << tColor << " " << tType << " at " << colChar << rowNum << endl;
        }
        cout << "\nAttack path: ";
        for (size_t i = 1; i < bestPath.path.size(); i++) {
            cout << char('A' + bestPath.path[i].second) << bestPath.path[i].first + 1;
            if (i < bestPath.path.size() - 1) cout << " -> ";
        }
        if (maxAttacks > 1) cout << "\n" << maxAttacks << " figures can be captured in one turn.";
    } else {
        cout << "No attacks possible from this position.";
        clearTiles();
        setBackgroudOfTile(row, col, TileMoment::active);
        vector<Direction> dirs = getDirections(figure, whiteInDown);
        bool hasMoves = false;
        for (Direction d : dirs) {
            auto next = getNextPosByDir(row, col, d);
            if (inBounds(next.first, next.second) && 
                checker.getFigures()[next.first][next.second].getState() == FigureState::none) {
                setBackgroudOfTile(next.first, next.second, TileMoment::move);
                hasMoves = true;
            }
        }
        drowBoard();
        cout << (hasMoves ? "Simple moves are available." : "No moves available from this position.");
    }
    cout << "\n\n[Press Enter to continue]";
    cin.ignore(); cin.get();
}

void Interface::showThreatsToFigure() {
    clearTiles();
    drowBoard("===== THREATS TO FIGURE =====");
    cout << "\nEnter position (e.g., C6): ";
    string selectPos; cin >> selectPos;
    
    int row, col;
    if (!checker.transformPosition(selectPos, col, row)) throw "Invalid position!";
    Figure& target = checker.getFigures()[row][col];
    
    vector<pair<string, pair<int, int>>> threats;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Figure& potential = checker.getFigures()[i][j];
            if (potential.getState() == FigureState::none) continue;
            if (potential.getSide() == target.getSide()) continue;
            if (canAttackTarget(i, j, row, col, potential)) {
                char colChar = 'A' + j;
                string threatColor = (potential.getSide() == FigureSide::white) ? "White" : "Black";
                string threatType = (potential.getState() == FigureState::piece) ? "piece" : "king";
        
                threats.push_back({threatColor + " " + threatType + " at " + colChar, {i, j}});
            }
        }
    }
    
    visualizeThreats(row, col, threats);
    drowBoard("===== THREATS TO FIGURE =====");
    
    if (target.getState() == FigureState::none) {
        cerr << "No figure at position " << selectPos << "!";
        cin.ignore(); cin.get();
        return;
    }
    
    cout << "\n" << (target.getSide() == FigureSide::white ? "White" : "Black") << " " 
         << (target.getState() == FigureState::piece ? "piece" : "king") << " at " << selectPos << "\n";
    
    if (!threats.empty()) {
        cout << "\nThreatened by " << threats.size() << " opponent figures:\n";
        for (const auto& t : threats) cout << "  - " << t.first << endl;
    } else {
        cout << "\nNot under direct threat from any opponent figures.";
    }
    cout << "\n\n[Press Enter to continue]";
    cin.ignore(); cin.get();
}

bool Interface::canAttackTarget(int ar, int ac, int tr, int tc, Figure& attacker) {
    if (attacker.getState() == FigureState::king) {
        for (Direction d : {Direction::topRight, Direction::bottomRight, Direction::topLeft, Direction::bottomLeft}) {
            int cr = ar, cc = ac;
            while (inBounds(cr, cc)) {
                auto next = getNextPosByDir(cr, cc, d);
                if (!inBounds(next.first, next.second)) break;
                if (next.first == tr && next.second == tc) {
                    auto behind = getNextPosByDir(tr, tc, d);
                    if (inBounds(behind.first, behind.second) && 
                        checker.getFigures()[behind.first][behind.second].getState() == FigureState::none)
                        return true;
                    break;
                }
                if (checker.getFigures()[next.first][next.second].getState() != FigureState::none) break;
                cr = next.first; cc = next.second;
            }
        }
    } else {
        for (Direction d : getDirections(attacker, whiteInDown)) {
            auto enemyPos = getNextPosByDir(ar, ac, d);
            if (!inBounds(enemyPos.first, enemyPos.second)) continue;
            if (enemyPos.first == tr && enemyPos.second == tc) {
                auto behind = getNextPosByDir(tr, tc, d);
                if (inBounds(behind.first, behind.second) && 
                    checker.getFigures()[behind.first][behind.second].getState() == FigureState::none)
                    return true;
            }
        }
    }
    return false;
}

void Interface::visualizeThreats(int tr, int tc, const vector<pair<string, pair<int, int>>>& threats, bool drawLines) {
    setBackgroudOfTile(tr, tc, TileMoment::active);
    for (const auto& t : threats) setBackgroudOfTile(t.second.first, t.second.second, TileMoment::attack);
    if (drawLines) {
        for (const auto& t : threats) {
            Direction dir = findAttackDirection(t.second.first, t.second.second, tr, tc);
            if (dir != Direction::none) drawAttackLine(t.second.first, t.second.second, tr, tc, dir);
        }
    }
}

Direction Interface::findAttackDirection(int fr, int fc, int tr, int tc) {
    int rd = tr - fr, cd = tc - fc;
    if (rd > 0 && cd > 0) return Direction::topRight;
    if (rd < 0 && cd > 0) return Direction::bottomRight;
    if (rd > 0 && cd < 0) return Direction::topLeft;
    if (rd < 0 && cd < 0) return Direction::bottomLeft;
    return Direction::none;
}

void Interface::drawAttackLine(int fr, int fc, int tr, int tc, Direction d) {
    int cr = fr, cc = fc;
    while (cr != tr || cc != tc) {
        if (!(cr == fr && cc == fc) && !(cr == tr && cc == tc))
            setBackgroudOfTile(cr, cc, TileMoment::move);
        auto next = getNextPosByDir(cr, cc, d);
        if (!inBounds(next.first, next.second)) break;
        cr = next.first; cc = next.second;
    }
}

void Interface::showThreatsToAllFigures() {
    clearTiles();
    drowBoard("===== THREATS TO ALL FIGURES =====");
    cout << "\nEnter side [white/black] (w/b): ";
    string side; cin >> side;
    if (side != "w" && side != "white" && side != "b" && side != "black") throw "Invalid side!";
    
    FigureSide targetSide = (side == "w" || side == "white") ? FigureSide::white : FigureSide::black;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (checker.getFigures()[i][j].getSide() == targetSide) {
                vector<pair<string, pair<int, int>>> threats;
                for (int ti = 0; ti < 8; ti++) {
                    for (int tj = 0; tj < 8; tj++) {
                        Figure& potential = checker.getFigures()[ti][tj];
                        if (potential.getState() == FigureState::none) continue;
                        if (potential.getSide() == targetSide) continue;
                        if (canAttackTarget(ti, tj, i, j, potential))
                            threats.push_back({"", {ti, tj}});
                    }
                }
                visualizeThreats(i, j, threats, false);
            }
        }
    }
    drowBoard("===== THREATS TO ALL FIGURES =====");
    cout << "\n[Press any key to continue]";
    cin.ignore(); cin.get();
}

void Interface::whereWhiteSide() {
    CLS();
    cout << "\nWhite side is " << (whiteInDown ? "down." : "up.");
    cout << "\n[Press any key to continue]";
    cin.ignore(); cin.get();
}

void Interface::showMenu() {
    while (true) {
        clearTiles();
        drowBoard("===== CHECKERS MENU =====");
        cout << "\n1. Analyze figure moves and attacks\n2. Analyze threats to figure\n"
             << "3. Analyze threats to all figures\n4. Where is white side?\n5. Exit\n\nChoice: ";
        
        int choice;
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(1000, '\n');
            continue;
        }
        cin.ignore();
        
        switch (choice) {
            case 1: showPossibleMovesAndAttacks(); break;
            case 2: showThreatsToFigure(); break;
            case 3: showThreatsToAllFigures(); break;
            case 4: whereWhiteSide(); break;
            case 5: return;
            default: break;
        }
    }
}
