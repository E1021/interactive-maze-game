#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <queue>
using namespace std;

// ===== 全局常量定义 =====
const int CREATER_DIR[4][2] = {
    {-2, 0}, {2, 0}, {0, -2}, {0, 2}
};

const int SEARCH_DIR[8][2] = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1},
    {1, 1}, {-1, 1}, {-1, -1}, {1, -1}
};

const int PLAYER_DIR[4][2] = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}
};

// ===== 数据结构定义 =====
struct Cell {
    int x;
    int y;
    int parent;
    int step;
};

struct GameState {
    vector<vector<int>> maze;
    vector<vector<int>> visited;
    int playerX, playerY;
    int goalX, goalY;
    int level;
    int totalMoves;
};

// ===== MazeGame 类定义 =====
class MazeGame {
private:
    GameState gameState;
    vector<Cell> bfsQueue;
    int n, m;

    const int WALL = 1;
    const int ROAD = 0;
    const int PATH_MARK = 3;

public:
    MazeGame();
    void mainMenu();
    void startNewGame(int level);
    void generateMaze();
    void playerGame();
    void autoSolve();
    void printMaze();
    void printMazeWithArrows(const vector<Cell>& path);
    void printMenuOptions();
    bool movePlayer(int dir);
    void generateMazeDFS(int x, int y);
    void bfsFindPath();
    void clearScreen();
    void pauseScreen();
    void printGameHelp();
};

// ===== MazeGame 实现 =====

MazeGame::MazeGame() {
    n = 0;
    m = 0;
    gameState.level = 1;
    gameState.totalMoves = 0;
    gameState.playerX = 1;
    gameState.playerY = 1;
    gameState.goalX = 1;
    gameState.goalY = 1;
}

void MazeGame::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void MazeGame::pauseScreen() {
    cout << "\n按 Enter 键继续...";
    cin.ignore();
    cin.get();
}

void MazeGame::printMenuOptions() {
    clearScreen();
    cout << "\n";
    cout << "================================================\n";
    cout << "           [O] 迷宫冒险游戏 [A]              \n";
    cout << "            Maze Adventure Game               \n";
    cout << "================================================\n\n";
    cout << "  [1] 开始游戏 (从第1关开始)\n";
    cout << "  [2] 选择关卡 (1-5关)\n";
    cout << "  [3] 自动生成答案 (查看AI最优解)\n";
    cout << "  [4] 游戏说明\n";
    cout << "  [5] 退出游戏\n\n";
    cout << "请输入选择 (1-5): ";
}

void MazeGame::printGameHelp() {
    clearScreen();
    cout << "\n";
    cout << "================================================\n";
    cout << "                 游戏说明\n";
    cout << "================================================\n\n";
    cout << "[游戏目标]\n";
    cout << "  从起点 (O) 走到终点 (A)\n";
    cout << "  完成所有 5 个关卡，成为迷宫大师！\n\n";

    cout << "[操作方式]\n";
    cout << "  W 或 上 - 向上移动\n";
    cout << "  S 或 下 - 向下移动\n";
    cout << "  A 或 左 - 向左移动\n";
    cout << "  D 或 右 - 向右移动\n";
    cout << "  Q      - 返回菜单\n\n";

    cout << "[符号说明]\n";
    cout << "  O  - 你的位置 (玩家)\n";
    cout << "  A  - 出口/目标位置\n";
    cout << "  #  - 墙壁 (不能穿过)\n";
    cout << "  空格 - 通路 (可以走)\n";
    cout << "  *  - 你走过的路\n\n";

    cout << "[最优解图解说明]\n";
    cout << "  S  - 起点\n";
    cout << "  E  - 终点\n";
    cout << "  ^  - 向上走\n";
    cout << "  v  - 向下走\n";
    cout << "  <  - 向左走\n";
    cout << "  >  - 向右走\n\n";

    cout << "[关卡难度]\n";
    cout << "  第1关: 10x10 (初级)\n";
    cout << "  第2关: 12x12 (简单)\n";
    cout << "  第3关: 14x14 (中等)\n";
    cout << "  第4关: 16x16 (困难)\n";
    cout << "  第5关: 18x18 (极难)\n\n";

    cout << "[评分标准]\n";
    cout << "  根据使用步数评分:\n";
    cout << "  步数 < 最优步数*1.2 : S等级 (完美!)\n";
    cout << "  步数 < 最优步数*1.5 : A等级 (很好!)\n";
    cout << "  步数 < 最优步数*2.0 : B等级 (不错!)\n";
    cout << "  其他 : C等级 (再加油)\n\n";

    pauseScreen();
}

void MazeGame::mainMenu() {
    int choice = 0;
    while (true) {
        printMenuOptions();
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            startNewGame(1);
            playerGame();
            break;

        case 2: {
            clearScreen();
            cout << "请选择关卡 (1-5): ";
            int level;
            cin >> level;
            cin.ignore();
            if (level >= 1 && level <= 5) {
                startNewGame(level);
                playerGame();
            } else {
                clearScreen();
                cout << "错误: 关卡必须在 1-5 之间！\n";
                pauseScreen();
            }
            break;
        }

        case 3: {
            clearScreen();
            cout << "\n请选择迷宫难度:\n";
            cout << "  [1] 简单 (10x10)\n";
            cout << "  [2] 中等 (12x12)\n";
            cout << "  [3] 困难 (14x14)\n";
            cout << "请输入 (1-3): ";
            int diff;
            cin >> diff;
            cin.ignore();

            int size = 10;
            if (diff == 1) size = 10;
            else if (diff == 2) size = 12;
            else if (diff == 3) size = 14;
            else size = 12;

            n = size;
            m = size;
            gameState.maze.assign(n, vector<int>(m, 1));
            gameState.visited.assign(n, vector<int>(m, 0));
            gameState.playerX = 1;
            gameState.playerY = 1;
            gameState.goalX = n - 2;
            gameState.goalY = m - 2;
            gameState.level = diff;

            generateMaze();
            clearScreen();
            autoSolve();
            pauseScreen();
            break;
        }

        case 4:
            printGameHelp();
            break;

        case 5:
            clearScreen();
            cout << "\n";
            cout << "================================================\n";
            cout << "  感谢游玩迷宫冒险游戏！再见！(^_^)\n";
            cout << "================================================\n\n";
            return;

        default:
            clearScreen();
            cout << "错误: 请输入 1-5 之间的数字！\n";
            pauseScreen();
        }
    }
}

void MazeGame::startNewGame(int level) {
    if (level < 1) level = 1;
    if (level > 5) level = 5;

    gameState.level = level;
    gameState.totalMoves = 0;

    int size = 8 + level * 2;
    n = size;
    m = size;

    gameState.maze.assign(n, vector<int>(m, 1));
    gameState.visited.assign(n, vector<int>(m, 0));
    gameState.playerX = 1;
    gameState.playerY = 1;
    gameState.goalX = n - 2;
    gameState.goalY = m - 2;

    generateMaze();
}

void MazeGame::generateMaze() {
    // 初始化为全是墙
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            gameState.maze[i][j] = WALL;
        }
    }

    // 起点设为通路
    gameState.maze[1][1] = ROAD;
    
    // DFS 生成迷宫
    generateMazeDFS(1, 1);
    
    // 确保终点是通路
    gameState.maze[n - 2][m - 2] = ROAD;
}

void MazeGame::generateMazeDFS(int x, int y) {
    int directions[4] = {0, 1, 2, 3};
    mt19937 rng(random_device{}());
    shuffle(directions, directions + 4, rng);

    for (int i = 0; i < 4; ++i) {
        int d = directions[i];
        int nx = x + CREATER_DIR[d][0];
        int ny = y + CREATER_DIR[d][1];

        if (nx >= 1 && nx <= n - 2 && ny >= 1 && ny <= m - 2 &&
            gameState.maze[nx][ny] == WALL) {
            
            // 打通中间的墙
            int wallX = x + CREATER_DIR[d][0] / 2;
            int wallY = y + CREATER_DIR[d][1] / 2;
            gameState.maze[wallX][wallY] = ROAD;
            gameState.maze[nx][ny] = ROAD;
            
            // 递归生成下一个单元格
            generateMazeDFS(nx, ny);
        }
    }
}

void MazeGame::printMaze() {
    clearScreen();
    cout << "\n";
    cout << "================================================\n";
    cout << "  第 " << gameState.level << " 关  |  迷宫大小: " 
         << n << "x" << m << "  |  步数: " << gameState.totalMoves << "\n";
    cout << "================================================\n\n";

    // 打印迷宫
    for (int i = 0; i < n; ++i) {
        cout << "  ";
        for (int j = 0; j < m; ++j) {
            if (i == gameState.playerX && j == gameState.playerY) {
                cout << "O ";  // 玩家
            } else if (i == gameState.goalX && j == gameState.goalY) {
                cout << "A ";  // 目标
            } else if (gameState.maze[i][j] == WALL) {
                cout << "# ";  // 墙
            } else if (gameState.maze[i][j] == PATH_MARK) {
                cout << "* ";  // 走过的路
            } else {
                cout << "  ";  // 通路
            }
        }
        cout << endl;
    }
    cout << endl;
}

bool MazeGame::movePlayer(int dir) {
    int newX = gameState.playerX + PLAYER_DIR[dir][0];
    int newY = gameState.playerY + PLAYER_DIR[dir][1];

    // 边界检查和墙壁检查
    if (newX >= 0 && newX < n && newY >= 0 && newY < m &&
        gameState.maze[newX][newY] != WALL) {
        
        // 标记走过的路
        gameState.maze[gameState.playerX][gameState.playerY] = PATH_MARK;
        gameState.playerX = newX;
        gameState.playerY = newY;
        gameState.totalMoves++;
        return true;
    }
    return false;
}

void MazeGame::playerGame() {
    while (true) {
        printMaze();
        cout << "  输入 (W/A/S/D 移动, Q 返回菜单, H 帮助): ";
        char c;
        cin >> c;
        cin.ignore();
        c = toupper(c);

        if (c == 'Q') {
            return;
        }

        if (c == 'H') {
            clearScreen();
            cout << "\n  快速帮助:\n";
            cout << "  W - 向上    S - 向下\n";
            cout << "  A - 向左    D - 向右\n";
            cout << "  Q - 返回菜单\n";
            cout << "  目标: 从 O 走到 A\n";
            pauseScreen();
            continue;
        }

        int dir = -1;
        if (c == 'W') dir = 0;      // 上
        else if (c == 'S') dir = 1; // 下
        else if (c == 'A') dir = 2; // 左
        else if (c == 'D') dir = 3; // 右

        if (dir == -1) {
            cout << "  错误: 请输入 W/A/S/D！\n";
            pauseScreen();
            continue;
        }

        if (!movePlayer(dir)) {
            cout << "  撞墙了！那里无法通行！\n";
            pauseScreen();
            continue;
        }

        // 检查是否到达目标
        if (gameState.playerX == gameState.goalX &&
            gameState.playerY == gameState.goalY) {
            
            // 计算评分
            bfsFindPath();
            int bestSteps = bfsQueue.empty() ? gameState.totalMoves : (int)bfsQueue[bfsQueue.size() - 1].step;
            string grade = "C";
            if (gameState.totalMoves < bestSteps * 1.2) grade = "S";
            else if (gameState.totalMoves < bestSteps * 1.5) grade = "A";
            else if (gameState.totalMoves < bestSteps * 2.0) grade = "B";

            clearScreen();
            cout << "\n";
            cout << "================================================\n";
            cout << "              *** 恭喜通关! ***\n";
            cout << "================================================\n";
            cout << "  关卡: 第 " << gameState.level << " 关\n";
            cout << "  你的步数: " << gameState.totalMoves << "\n";
            cout << "  最优步数: " << bestSteps << "\n";
            cout << "  评分等级: " << grade << "\n";
            cout << "================================================\n\n";

            cout << "请选择下一步:\n";
            cout << "  [1] 查看最优路线 (带箭头图解)\n";
            cout << "  [2] 进入下一关\n";
            cout << "  [3] 重新挑战本关\n";
            cout << "  [4] 返回主菜单\n";
            cout << "请输入 (1-4): ";

            int choice;
            cin >> choice;
            cin.ignore();

            switch (choice) {
            case 1:
                clearScreen();
                printMazeWithArrows(bfsQueue);
                pauseScreen();
                return;

            case 2:
                if (gameState.level < 5) {
                    startNewGame(gameState.level + 1);
                    playerGame();
                } else {
                    clearScreen();
                    cout << "\n";
                    cout << "================================================\n";
                    cout << "       [CONGRATULATIONS] 你已通关所有关卡!\n";
                    cout << "               成为迷宫大师了！\n";
                    cout << "================================================\n";
                    cout << "\n";
                    pauseScreen();
                }
                return;

            case 3:
                startNewGame(gameState.level);
                playerGame();
                return;

            case 4:
            default:
                return;
            }
        }
    }
}

void MazeGame::bfsFindPath() {
    bfsQueue.clear();
    vector<vector<int>> visited(n, vector<int>(m, 0));

    Cell start;
    start.x = 1;
    start.y = 1;
    start.parent = 0;
    start.step = 0;
    bfsQueue.push_back(start);
    visited[1][1] = 1;

    int head = 0;
    bool found = false;

    while (head < (int)bfsQueue.size()) {
        int x = bfsQueue[head].x;
        int y = bfsQueue[head].y;

        // 尝试8个方向
        for (int i = 0; i < 8; ++i) {
            int tx = x + SEARCH_DIR[i][0];
            int ty = y + SEARCH_DIR[i][1];

            if (tx >= 1 && tx <= n - 2 && ty >= 1 && ty <= m - 2 &&
                visited[tx][ty] == 0 && gameState.maze[tx][ty] != WALL) {
                
                visited[tx][ty] = 1;

                Cell cell;
                cell.x = tx;
                cell.y = ty;
                cell.parent = head;
                cell.step = bfsQueue[head].step + 1;
                bfsQueue.push_back(cell);

                if (tx == gameState.goalX && ty == gameState.goalY) {
                    found = true;
                    break;
                }
            }
        }

        if (found) break;
        head++;
    }
}

void MazeGame::printMazeWithArrows(const vector<Cell>& path) {
    // 重建路径用于显示方向
    vector<pair<int, int>> route;
    int idx = (int)path.size() - 1;

    while (idx > 0) {
        route.push_back({path[idx].x, path[idx].y});
        idx = path[idx].parent;
    }
    route.push_back({1, 1});
    reverse(route.begin(), route.end());

    cout << "\n";
    cout << "================================================\n";
    cout << "          [最优路线图解] 第 " << gameState.level << " 关\n";
    cout << "          最少步数: " << path[path.size() - 1].step << "\n";
    cout << "================================================\n\n";

    // 打印迷宫
    for (int i = 0; i < n; ++i) {
        cout << "  ";
        for (int j = 0; j < m; ++j) {
            if (i == 1 && j == 1) {
                cout << "S ";  // 起点
            } else if (i == gameState.goalX && j == gameState.goalY) {
                cout << "E ";  // 终点
            } else if (gameState.maze[i][j] == WALL) {
                cout << "# ";  // 墙
            } else {
                // 查找该点在路径中的位置，显示对应的箭头
                bool found = false;
                for (size_t k = 0; k + 1 < route.size(); ++k) {
                    if (route[k].first == i && route[k].second == j) {
                        int nextX = route[k + 1].first;
                        int nextY = route[k + 1].second;

                        if (nextX < i) cout << "^ ";      // 向上
                        else if (nextX > i) cout << "v ";  // 向下
                        else if (nextY < j) cout << "< ";  // 向左
                        else if (nextY > j) cout << "> ";  // 向右
                        else cout << "* ";

                        found = true;
                        break;
                    }
                }
                if (!found) cout << "  ";
            }
        }
        cout << endl;
    }
    cout << endl;

    // 打印完整路线说明
    cout << "================================================\n";
    cout << "完整路线: S";
    for (size_t k = 0; k < route.size(); ++k) {
        cout << " -> (" << route[k].first << "," << route[k].second << ")";
    }
    cout << " -> E\n";
    cout << "================================================\n\n";
}

void MazeGame::autoSolve() {
    bfsFindPath();
    if (!bfsQueue.empty()) {
        cout << "已找到最优路线！\n";
        cout << "最少步数: " << bfsQueue[bfsQueue.size() - 1].step << "\n";
        printMazeWithArrows(bfsQueue);
    } else {
        cout << "无法找到路线！\n";
    }
}

// ===== 主函数 =====
int main() {
    MazeGame game;
    game.mainMenu();
    return 0;
}
