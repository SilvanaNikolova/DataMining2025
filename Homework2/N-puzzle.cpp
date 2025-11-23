#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
using namespace std;

struct State {
    vector<int> board;
    int size;
    int zeroRow, zeroCol; //zero row, zero column
    int h; //евристика
    vector<int> path; // 0=up,1=down,2=left,3=right

    State(vector<int> b, int s, int r, int c, int hval, vector<int> p = {})
        : board(move(b)), size(s), zeroRow(r), zeroCol(c), h(hval), path(move(p)) {}
};

vector<pair<int,int>> goalPos;

int heuristic(const vector<int>& board, int size) {
    //Манхатън -> събирам общото разстояние, което всяка плочка трябва да измине, за да стигне до целевата си позиция (без нулата)
    int dist = 0;
    for (int i = 0; i < size * size; i++) {
        int val = board[i];
        if (val != 0) {
            auto [gi, gj] = goalPos[val];
            //          i/size -> row        i % size -> col
            dist += abs(i / size - gi) + abs(i % size - gj);
        }
    }

    //LINEAR CONFLICTS
    // row linear conflicts
    for (int row = 0; row < size; row++) {
        for (int i = 0; i < size; i++) {
            int currSquare = board[row * size + i]; //плочка на позиция (row, i)
            if (currSquare == 0) //ако е празната я пропускам
                continue;
            auto [currSquareRow, currSquareCol] = goalPos[currSquare]; //целеви координати на currSquare
            if (currSquareRow != row) //ако целевия ред не е същия я пропускам
                continue;
            for (int j = i + 1; j < size; j++) { //обхождам всички плочки вдясно на currSquare и ги сравнявам
                int rightSquare = board[row * size + j];
                if (rightSquare == 0) //ако е празната я пропускам
                    continue;
                auto [rightSquareRow, rightSquareCol] = goalPos[rightSquare]; //целеви координати на rightSquare
                if (rightSquareRow == row && currSquareCol > rightSquareCol) //ако целевия ред на rightSquare е същия и целевата колона на currSquare > целевата колона на rightSquare 
                                            //=> размяна => +2 dist
                    dist += 2; //admissible евристика
            }
        }
    }
    // column linear conflicts (Аналогично)
    for (int col = 0; col < size; col++) {
        for (int i = 0; i < size; i++) {
            int currSquare = board[i * size + col];
            if (currSquare == 0) 
                continue;
            auto [currSquareRow, currSquareCol] = goalPos[currSquare];
            if (currSquareCol != col) 
                continue;
            for (int j = i + 1; j < size; j++) {
                int rightSquare = board[j * size + col];
                if (rightSquare == 0) 
                    continue;
                auto [rightSquareRow, rightSquareCol] = goalPos[rightSquare];
                if (rightSquareCol == col && currSquareRow > rightSquareRow) 
                    dist += 2;
            }
        }
    }
    return dist;
}

bool isSolvable(const vector<int>& board, int size, int zeroRow) {
    int inv = 0;
    //обхождам дъската и броя инверсиите
    for (int i = 0; i < board.size(); i++) {
        if (board[i] == 0) 
            continue;
        for (int j = i + 1; j < board.size(); j++) {
            if (board[j] && board[i] > board[j]) 
                inv++;
        }
    }
    //Ако размера е нечетен, то задачата е решима <=> инверсиите са четни
    if (size % 2 == 1) 
        return inv % 2 == 0;
    else 
        //Ако рамера е нечетен, то задачата е решима <=> сумата инверсиите + реда на празната плочка отдолу нагоре трябва да е четна
        return (inv + zeroRow) % 2 == 1;
}

bool search(State& node, int g, int threshold, int& nextThreshold, const vector<int>& goal, vector<int>& resultPath, int prevMove) {
    int f = g + node.h;
    if (f > threshold) {
        //не навлизам по-дълбоко и ъпдействам следващия по големина праг
        nextThreshold = min(nextThreshold, f);
        return false;
    }

    //Ако текущата дъска е равна на целевата, задавам целевия път да е равен на текущия (намерена цел)
    if (node.board == goal) {
        resultPath = node.path;
        return true;
    }

    static int rowMove[4] = {-1,1,0,0};    //движения по редове
    static int colMove[4] = {0,0,-1,1};    //движения по колони
    static int opposite[4] = {1,0,3,2};
    // 0 = up -> opposite = down (1)
    // 1 = down -> opposite = up (0)
    // 2 = left -> opposite = right (3)
    // 3 = right -> opposite = left (2)


    for (int d = 0; d < 4; d++) {
        //Проверка за обратно движение
        //opposite[prevMove] е обратната посока на предишния ход
        if (prevMove != -1 && d == opposite[prevMove]) 
            continue;
        
        //Новите координати на празното поле (newRow, newCol)
        int newRow = node.zeroRow + rowMove[d];
        int newCol = node.zeroCol + colMove[d];

        if (newRow < 0 || newRow >= node.size || newCol < 0 || newCol >= node.size) 
            continue;

        //index = row * size + col;
        int idx1 = node.zeroRow * node.size + node.zeroCol; //индекс на празната плочка
        int idx2 = newRow * node.size + newCol;//нов индекс на празната плочка

        vector<int> newBoard = node.board;

        //Разменям мястото на празната плочка в новата дъска
        swap(newBoard[idx1], newBoard[idx2]);

        //Пресмятам новата евристика за детето
        int newH = heuristic(newBoard, node.size);
        vector<int> newPath = node.path;
        //Добавям кода на хода към пътя на детето
        newPath.push_back(d);

        State child(newBoard, node.size, newRow, newCol, newH, newPath);

        //Рекурсивно извиквам функцията за новото състяние (детето)
        if (search(child, g+1, threshold, nextThreshold, goal, resultPath, d))
            return true;
    }
    return false;
}

int IDA(State& start, const vector<int>& goal, vector<int>& resultPath) {
    int threshold = start.h; //евристиката на текущото състояние (инициализация на прага)
    while (true) {
        int nextThreshold = INT_MAX;
        //ако search намери целта връща дължината на пътя и го записва в resultPath
        if (search(start, 0, threshold, nextThreshold, goal, resultPath, -1))
            return resultPath.size();
        //Ако search не намери целта и няма друг праг, връща -1
        if (nextThreshold == INT_MAX) 
            return -1;
        //Ако search не намери целта, но намери следващия праг, задава текущия да е равен на следващия по големина
        threshold = nextThreshold;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, zeroIdx;
    cin >> N >> zeroIdx;
    int size = sqrt(N+1);

    vector<int> board(size * size);

    //ред и колона на нулата
    int zi = -1, zj = -1;
    for (int i = 0; i < size * size; i++) {
        cin >> board[i];
        if (board[i] == 0) 
        { 
            zi = i / size; 
            zj = i % size; 
        }
    }

    ///целеви вектор
    vector<int> goal(N+1);
    //задавам му стойности от 1 до N
    iota(goal.begin(), goal.end()-1, 1);

    goal[N] = 0;
    if (zeroIdx != -1 && zeroIdx != N) {
        //задавам целевото положение на празната плочка
        //когато трябва да е на конкретна позиция, различна от последната
        swap(goal[zeroIdx], goal[N]);
    }

    //целевите позиции на всички плочки
    goalPos.assign(N + 1, {0, 0});
    for (int i = 0; i < size * size; i++) {
        int val = goal[i];
        goalPos[val] = { i / size, i % size };
    }

    //изчислявам реда на нулата отдолу (В примерния инпут: size = 3, zi = 2 => zeroRow = 1)
    int zeroRow = size - zi;
    if (!isSolvable(board, size, zeroRow)) {
        cout << -1 << "\n";
        return 0;
    }

    int startHeuristic = heuristic(board, size);
    State start(board, size, zi, zj, startHeuristic);

    vector<int> resultPath;
    int steps = IDA(start, goal, resultPath);

    if (steps == -1) {
        cout << -1 << "\n";
    } else {
        cout << steps << "\n";
        static string names[4] = { "up", "down", "left", "right" };
        static int opposite[4] = { 1, 0, 3, 2 };
        for (int d : resultPath) 
            cout << names[opposite[d]] << "\n";
    }
    return 0;
}