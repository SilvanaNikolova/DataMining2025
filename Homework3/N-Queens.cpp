#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;

bool HAS_CONFLICTS;
int n;

int* queens;   // queens[col] = row
int* r;        // брой царици в ред
int* d1;       // диагонали (row - col)
int* d2;       // диагонали (row + col)

void putQueens() {
    // начално поставяне на цариците
    int col = 1;
    for (int row = 0; row < n; row++) {
        queens[col] = row;
        r[row]++;
        d1[col - row + n]++;
        d2[col + row]++;
        col += 2;

        if (col >= n) 
            col = 0;
    }
}

int colWithQueenWithMaxConf() {
    int maxConf = -1;
    vector<int> colsWithMaxConf;

    for (int curCol = 0; curCol < n; curCol++) {
        int curRow = queens[curCol];
        int curConf = r[curRow] + d1[curCol - curRow + n] + d2[curCol + curRow] - 3;
        if (curConf == maxConf) {
            colsWithMaxConf.push_back(curCol);
        } else if (curConf > maxConf) {
            maxConf = curConf;
            colsWithMaxConf.clear();
            colsWithMaxConf.push_back(curCol);
        }
    }
    if (maxConf == 0) 
        HAS_CONFLICTS = false;

    if (colsWithMaxConf.empty()) 
        return rand() % n; // fallback - предпазна мярка (връща произволна колона)
    
    int randIndex = rand() % colsWithMaxConf.size();
    
    return colsWithMaxConf[randIndex];
}

int rowWithMinConf(int col) {
    int minConf = n + 1;
    vector<int> rowsWithMinConf;

    for (int curRow = 0; curRow < n; curRow++) {
        int curConf;
        if (queens[col] == curRow) {
            curConf = r[curRow] + d1[col - curRow + n] + d2[col + curRow] - 3;
        } else {
            curConf = r[curRow] + d1[col - curRow + n] + d2[col + curRow];
        }

        if (curConf == minConf) {
            rowsWithMinConf.push_back(curRow);
        } else if (curConf < minConf) {
            minConf = curConf;
            rowsWithMinConf.clear();
            rowsWithMinConf.push_back(curRow);
        }
    }
    if (rowsWithMinConf.empty()) 
        return rand() % n; // fallback - предпазна мярка (връща произволен ред)
    
    int randIndex = rand() % rowsWithMinConf.size();
    
    return rowsWithMinConf[randIndex];
}

void updateState(int row, int col) {
    int prevRow = queens[col];

    r[prevRow]--;
    d1[col - prevRow + n]--;
    d2[col + prevRow]--;

    queens[col] = row;
    r[row]++;
    d1[col - row + n]++;
    d2[col + row]++;
}

void solve() {
    while (true) {
        HAS_CONFLICTS = true;
        // нулирам броячите
        fill(r, r + n, 0);
        fill(d1, d1 + 2*n, 0);
        fill(d2, d2 + 2*n, 0);
        putQueens();

        int iter = 0;
        int k = 2; // множител за броя итерации
        while (iter++ <= k * n) {
            //Ако няма конфликти задава HAS_CONFLICTS = false
            int col = colWithQueenWithMaxConf();
            if (!HAS_CONFLICTS) 
                return; // намерено решение
            
            int row = rowWithMinConf(col);
            updateState(row, col);
        }
        // ако не е намерено решение, цикълът започва отново (random restart)
    }
}

void printQueensArray() {
    for (int i = 0; i < n; i++) {
        cout << queens[i];
        if (i < n - 1) 
            cout << " ";
    }
    cout << "\n";
}

void printBoard() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << (queens[j] == i ? "* " : "_ ");
        }
        cout << "\n";
    }
}

int main() {
    cin >> n;

    if (n == 2 || n == 3) {
        cout << -1 << "\n";
        return 0;
    }

    queens = new int[n];
    r = new int[n]{0};
    d1 = new int[2 * n]{0};
    d2 = new int[2 * n]{0};

    srand(time(NULL));

    clock_t start = clock();

    solve();

    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;

    // винаги печатаме масива
    printQueensArray();

    if (n <= 50) { // за малки N печатаме и дъската
        printBoard();
    }

    cerr << "Time: " << elapsed << "s\n"; // време на stderr, за да не пречи на checker-а

    delete[] queens;
    delete[] r;
    delete[] d1;
    delete[] d2;

    return 0;
}