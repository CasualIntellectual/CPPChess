module controller;

import <iostream>;
import <string>;

using namespace std;

CommandInterpreter::CommandInterpreter(Game *g) : game{g} {}

Position CommandInterpreter::parsePos(const string& s) {
    if (s.size() < 2) return {-1, -1};
    // chess: letter=file, digit=rank
    //our Position is row 0..7 from bottom, col 0..7 from left
    return {s[1] - '1', s[0] - 'a'};
}

void CommandInterpreter::run() {
    // banner + command cheat sheet (only runs once when program starts)
    cout << endl;
    cout << "========================================" << endl;
    cout << "       Welcome to CS246 Chess!" << endl;
    cout << "========================================" << endl;
    cout << endl;
    cout << "Commands:" << endl;
    cout << "  game <white> <black>  Start a new game" << endl;
    cout << "     Player types: human, computer1, computer2, computer3, computer4" << endl;
    cout << "     e.g. game human computer2" << endl;
    cout << "  move <from> <to>      Move a piece (e.g. move e2 e4)" << endl;
    cout << "     For pawn promotion, append the piece: move e7 e8 Q" << endl;
    cout << "     For computer players, just type: move" << endl;
    cout << "  resign                Concede the current game" << endl;
    cout << "  draw                  Offer a draw to your opponent" << endl;
    cout << "  setup                 Enter board setup mode (only between games)" << endl;
    cout << "  Ctrl-D                Quit and show final score" << endl;
    cout << endl;

    string cmd;

    auto printPrompt = [&]() {
        if (inGame) {
            string turn = (game->getCurrentTurn() == Colour::WHITE) ? "White" : "Black";
            cout << "[" << turn << "'s turn] > ";
        }
        else {
            cout << "> ";
        }
        cout.flush();
    };

    printPrompt();
    while (cin >> cmd) {
        if (cmd == "game") {
            if (inGame) {
                printPrompt();
                continue;
            }
            string wt, bt;
            if (!(cin >> wt >> bt)) break;
            if (wt != "human" && wt != "computer1" && wt != "computer2"
                    && wt != "computer3" && wt != "computer4") {
                cout << "Invalid player type." << endl;
                cout << "  Valid types: human, computer1, computer2, computer3, computer4" << endl;
                printPrompt();
                continue;
            }
            if (bt != "human" && bt != "computer1" && bt != "computer2"
                    && bt != "computer3" && bt != "computer4") {
                cout << "Invalid player type." << endl;
                cout << "  Valid types: human, computer1, computer2, computer3, computer4" << endl;
                printPrompt();
                continue;
            }
            game->initGame(wt, bt);
            inGame = true;
            cout << "Game started: White (" << wt << ") vs Black (" << bt << ")" << endl;
            cout << endl;

        }
        else if (cmd == "resign") {
            if (!inGame) {
                printPrompt();
                continue;
            }
            Colour turn = game->getCurrentTurn();
            if (turn == Colour::WHITE) {
                cout << "Black wins!" << endl;
                blackScore += 1;
            }
            else {
                cout << "White wins!" << endl;
                whiteScore += 1;
            }
            inGame = false;
            cout << "Enter 'game' to start a new game or Ctrl-D to quit." << endl;

        } else if (cmd == "draw") {
            if (!inGame) {
                printPrompt();
                continue;
            }
            Colour turn = game->getCurrentTurn();
            string offeror = (turn == Colour::WHITE) ? "White" : "Black";
            string opponent = (turn == Colour::WHITE) ? "Black" : "White";
            cout << offeror << " offers a draw." << endl;
            cout << "Does " << opponent << " accept? (yes/no): ";
            cout.flush();
            string response;
            if (!(cin >> response)) break;
            if(response == "yes"){
                cout << "Draw by agreement!" << endl;
                whiteScore += 0.5;
                blackScore += 0.5;
                inGame = false;
                cout << "Enter 'game' to start a new game or Ctrl-D to quit." << endl;
            } else {
                cout << "Draw declined." << endl;
            }

        } else if (cmd == "move") {
            if (!inGame) {
                printPrompt();
                continue;
            }
            bool ok = game->playTurn();
            if (!ok) {
                cout << "Invalid move." << endl;
                printPrompt();
                continue;
            }
            Board &b = game->getBoard();
            Colour next = game->getCurrentTurn();
            b.recordPosition(next);

            if (b.isCheckmate(next)) {
                Colour winner = (next == Colour::WHITE)
                    ? Colour::BLACK : Colour::WHITE;
                if (winner == Colour::WHITE)
                    cout << "Checkmate! White wins!" << endl;
                else
                    cout << "Checkmate! Black wins!" << endl;
                (winner == Colour::WHITE) ? whiteScore += 1 : blackScore += 1;
                inGame = false;
                cout << "Enter 'game' to start a new game or Ctrl-D to quit." << endl;
            }
            else if (b.isStalemate(next)) {
                cout << "Stalemate!" << endl;
                whiteScore += 0.5;
                blackScore += 0.5;
                inGame = false;
                cout << "Enter 'game' to start a new game or Ctrl-D to quit." << endl;
            }
            else if (b.isRepetitionDraw()) {
                cout << "Draw by threefold repetition!" << endl;
                whiteScore += 0.5;
                blackScore += 0.5;
                inGame = false;
                cout << "Enter 'game' to start a new game or Ctrl-D to quit." << endl;
            }
            else if (b.isCheck(next)) {
                if (next == Colour::WHITE)
                    cout << "White is in check." << endl;
                else
                    cout << "Black is in check." << endl;
            }

        }
        else if (cmd == "setup") {
            if (inGame) {
                printPrompt();
                continue;
            }
            setupMode();
        }
        printPrompt();
    }

    cout << endl;
    cout << "Final Score:" << endl;
    cout << "White: " << whiteScore << endl;
    cout << "Black: " << blackScore << endl;
    cout << endl;
    cout << "Thanks for playing!" << endl;
}

void CommandInterpreter::setupMode() {
    Board& b = game->getBoard();
    string cmd;

    cout << endl;
    cout << "-- Board Setup Mode --" << endl;
    cout << "  + <piece> <pos>   Place a piece (e.g. + K e1, + p d7)" << endl;
    cout << "     Uppercase = White, lowercase = Black" << endl;
    cout << "     Pieces: K/k Q/q R/r B/b N/n P/p" << endl;
    cout << "  - <pos>           Remove a piece (e.g. - e1)" << endl;
    cout << "  = <colour>        Set whose turn is next (white or black)" << endl;
    cout << "  done              Leave setup mode" << endl;
    cout << endl;

    cout << "setup> ";
    cout.flush();
    while (cin >> cmd) {
        if (cmd == "+") {
            char type;
            string pos;
            if (!(cin >> type >> pos)) break;
            Position p = parsePos(pos);
            if (p.row < 0 || p.row > 7 || p.col < 0 || p.col > 7) {
                cout << "Invalid position." << endl;
                cout << "setup> ";
                cout.flush();
                continue;
            }
            b.setupAdd(type, p);
            b.notifyObservers();

        } else if (cmd == "-") {
            string pos;
            if (!(cin >> pos)) break;
            Position p = parsePos(pos);
            if(p.row<0||p.row>7||p.col<0||p.col>7){
                cout << "Invalid position." << endl;
                cout << "setup> ";
                cout.flush();
                continue;
            }
            b.setupRemove(p);
            b.notifyObservers();

        } else if (cmd == "=") {
            string col;
            if (!(cin >> col)) break;
            if (col == "white")
                game->setTurn(Colour::WHITE);
            else if (col == "black")
                game->setTurn(Colour::BLACK);

        } else if (cmd == "done") {
            if (b.verifySetup()) {
                game->setCustomSetup(true);
                cout << "Setup complete." << endl;
                return;
            }
            cout << "Invalid board setup." << endl;
            cout << "  Ensure: exactly one king per side, no pawns on rows 1/8, no king in check." << endl;
        }
        cout << "setup> ";
        cout.flush();
    }
}
