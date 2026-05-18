module board;

using namespace std;

void Subject::attach(Observer* o) { observers.push_back(o); }

void Subject::detach(Observer *o) {
    for (auto it = observers.begin(); it != observers.end(); ++it) {
        if (*it == o) {
            observers.erase(it);
            return;
        }
    }
}

void Subject::notifyObservers() {
    for (auto obs : observers) obs->notify();
}
