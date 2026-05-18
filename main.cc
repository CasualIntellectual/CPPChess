import <chrono>;
import <iostream>;
import <stdexcept>;
import <string>;
import board;
import controller;
import display;
import game;
import player;

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    unsigned seed = system_clock::now().time_since_epoch().count();
    if (argc > 1) {
        try {
            seed = stoi(string{argv[1]});
        } catch (invalid_argument& e) {
            cerr << e.what() << endl;
            return 1;
        } catch (out_of_range& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    init_random_seed(seed);  // same seed -> computer moves repeatable

    Game g;
    Board& b = g.getBoard();
    TextDisplay td{&b};
    GraphicalDisplay gd{&b};
    b.attach(&td);
    b.attach(&gd);
    CommandInterpreter ci{&g};
    ci.run();
}
