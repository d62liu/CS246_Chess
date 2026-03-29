export module observer;

import std;

export class Observer {
public:
    virtual void update() = 0;
    virtual ~Observer() = default;
};

export class Subject {
    std::vector<Observer*> observers;
public:
    void notifyObservers() const;
    void attach(Observer* o);
    void detach(Observer* o);
};
