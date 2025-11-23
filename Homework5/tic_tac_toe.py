def print_board(board):
    print("+---+---+---+")
    for row in board:
        print("| " + " | ".join(row) + " |")
        print("+---+---+---+")

def is_terminal(board):
    lines = [
        board[0], board[1], board[2],#редове
        [board[i][0] for i in range(3)],#колони
        [board[i][1] for i in range(3)],
        [board[i][2] for i in range(3)],
        [board[i][i] for i in range(3)],#диагонали
        [board[i][2 - i] for i in range(3)]
    ]
    if ["X"] * 3 in lines:
        return True, "X"
    if ["O"] * 3 in lines: 
        return True, "O"
    if all(cell != "_" for row in board for cell in row): 
        return True, "DRAW"
    return False, None

def evaluate(board, depth):
    terminal, winner = is_terminal(board)
    if not terminal: 
        return 0
    if winner == "X": 
        return 10 - depth
    if winner == "O": 
        return depth - 10
    return 0

#Ред на ходовете
def get_moves(board):
    preferred_order = [(1,1), (0,0), (0,2), (2,0), (2,2),
                       (0,1), (1,0), (1,2), (2,1)]
    return [(r,c) for (r,c) in preferred_order if board[r][c] == "_"]

#Проверка за победа
def find_winning_move(board, player):
    for r, c in get_moves(board):
        board[r][c] = player
        terminal, winner = is_terminal(board)
        board[r][c] = "_"
        if terminal and winner == player:
            return (r, c)
    return None

#Проверка за блокиращ ход
def find_blocking_move(board, player):
    opponent = "O" if player == "X" else "X"
    for r, c in get_moves(board):
        board[r][c] = opponent
        terminal, winner = is_terminal(board)
        board[r][c] = "_"
        if terminal and winner == opponent:
            return (r, c)
    return None

#Minimax + alpha-beta
def minimax(board, depth, alpha, beta, maximizing, player):
    #Победа
    winning = find_winning_move(board, player)
    if winning:
        return 100 - depth, winning

    #Блокиращ ход
    blocking = find_blocking_move(board, player)
    if blocking:
        return 0, blocking

    #Терминално състояние
    terminal, winner = is_terminal(board)
    if terminal:
        return evaluate(board, depth), None

    if maximizing:
        maxEval, bestMove = -999, None
        for r, c in get_moves(board):
            board[r][c] = player
            eval, _ = minimax(board, depth + 1, alpha, beta, False, "O" if player=="X" else "X")
            board[r][c] = "_"
            if eval > maxEval:
                maxEval, bestMove = eval, (r, c)
            alpha = max(alpha, eval)
            if beta <= alpha: 
                break
        return maxEval, bestMove
    else:
        minEval, bestMove = 999, None
        for r, c in get_moves(board):
            board[r][c] = player
            eval, _ = minimax(board, depth + 1, alpha, beta, True, "O" if player=="X" else "X")
            board[r][c] = "_"
            if eval < minEval:
                minEval, bestMove = eval, (r, c)
            beta = min(beta, eval)
            if beta <= alpha: 
                break
        return minEval, bestMove

def main():
    mode = input().strip()
    if mode == "JUDGE":
        turn_line = input().strip()         
        turn = turn_line.split()[1]
        board_lines = [input().strip() for _ in range(7)]  
        board = [row.split("|")[1:-1] for row in board_lines if "|" in row]
        board = [[c.strip() for c in row] for row in board]

        terminal, _ = is_terminal(board)
        if terminal:
            print(-1)
        else:
            _, move = minimax(board, 0, -999, 999, True, turn)
            print(move[0] + 1, move[1] + 1)

    elif mode == "GAME":
        first_line = input().strip()          
        human_line = input().strip()          
        first = first_line.split()[1]
        human = human_line.split()[1]

        board_lines = [input().strip() for _ in range(7)]
        board = [row.split("|")[1:-1] for row in board_lines if "|" in row]
        board = [[c.strip() for c in row] for row in board]

        print("Начало на играта:")
        print_board(board)

        current = first
        while True:
            terminal, winner = is_terminal(board)
            if terminal:
                if winner == "DRAW": print("DRAW")
                else: 
                    print(f"WINNER: {winner}")
                break

            if current == human:
                try:
                    r, c = map(int, input().split())
                except EOFError:
                    print("Прекратяване на играта.")
                    break
                if 1 <= r <= 3 and 1 <= c <= 3 and board[r - 1][c - 1] == "_":
                    board[r - 1][c - 1] = human
            else:
                _, move = minimax(board, 0, -999, 999, True, current)
                board[move[0]][move[1]] = current

            print_board(board)
            current = "O" if current == "X" else "X"

if __name__ == "__main__":
    main()