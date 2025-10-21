// solution.cpp

// Вариант B — Code Runner (едно натискане)

// Натисни Ctrl + Alt + N

// Програмата се компилира и стартира автоматично в терминала на VS Code.

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>

std::string makeInitialBoard(int N)
{
    int size = N * 2 + 1;
    std::string board(size, '_');
    for (int i = 0; i < N; i++)
        board[i] = '>';
    for(int i = N + 1; i < size; i++) 
        board[i] = '<';
    
    return board;
}

std::string makeGoalBoard(int N)
{
    int size = N * 2 + 1;
    std::string board(size, '_');

    for(int i = 0; i < N; i++)
        board[i] = '<';
    for(int i = N + 1; i < size; i++)
        board[i] = '>';

    return board;
}

std::vector<std::string> generateMoves(const std::string& board)
{
    std::vector<std::string> nextMoves;
    int n = board.size();

    for(int i = 0; i < n; i++)
    {
        if(board[i] == '>')
        {
            if(i + 1 < n && board[i + 1] == '_')
            {
                std::string tmp = board;
                std::swap(tmp[i], tmp[i + 1]);
                nextMoves.push_back(tmp);
            }
            if(i + 2 < n && board[i + 1] != '_' && board[i + 2] == '_')
            {
                std::string tmp = board;
                std::swap(tmp[i], tmp[i + 2]);
                nextMoves.push_back(tmp);
            }
        }
        else if(board[i] == '<')
        {
            if(i - 1 >= 0 && board[i - 1] == '_')
            {
                std::string tmp = board;
                std::swap(tmp[i], tmp[i - 1]);
                nextMoves.push_back(tmp);
            }
            if(i - 2 >= 0 && board[i - 1] != '_' && board[i - 2] == '_')
            {
                std::string tmp = board;
                std::swap(tmp[i], tmp[i - 2]);
                nextMoves.push_back(tmp);
            }
        }
    }

    return nextMoves;
}

bool dfs(const std::string& current, const std::string& goal, std::unordered_set<std::string>& visited, std::vector<std::string>& path)
{
    if(current == goal)
    {
        path.push_back(current);
        return true;
    }

    visited.insert(current);
    std::vector<std::string> moves = generateMoves(current);

    for(auto& nxt : moves)
    {
        if(!visited.count(nxt))
        {
            if(dfs(nxt, goal, visited, path))
            {
                path.push_back(current);
                return true;
            }
        }
    }

    return false;
}

void frogLeap(int N)
{
    std::string start = makeInitialBoard(N);
    std::string goal = makeGoalBoard(N);

    std::unordered_set<std::string> visited;
    std::vector<std::string> path;

    if(dfs(start, goal, visited, path))
    {;
        std::reverse(path.begin(), path.end());
        for(auto& state : path)
            std::cout << state << "\n";
    }
    else 
    {
        std::cout << "No path found!\n";
    }
}

int main() {
    int N;
    std::cin >> N;

    frogLeap(N);

    return 0;
}
