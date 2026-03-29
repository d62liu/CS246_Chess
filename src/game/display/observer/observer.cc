module observer;

import std;

void Subject::notifyObservers() const {
    for (auto o : observers) o->update();
}

void Subject::attach(Observer* o) {
    observers.push_back(o);
}

void Subject::detach(Observer* o) {
    std::erase(observers, o);
}
