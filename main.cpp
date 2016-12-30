#include <cstdio>
#include <string>

const int FIRST_SYMBOL = 97;
const int ALPHABET_SIZE = 26;

struct Transition;

// TODO: check if final states are correct.
struct State {
    Transition* next[ALPHABET_SIZE];
    State* suffixLink;
    bool isFinal;

    State();
    int getTransition(State* state);
};

struct Transition {
    State* state;
    bool primary;

    Transition(State* s, bool p) : state(s), primary(p) {}
    Transition(State* s) : Transition(s, false) {}
};

State::State() {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        this->next[i] = nullptr;
    }
    this->suffixLink = nullptr;
    this->isFinal = false;
}

// TODO: retrieve state in linear time.
int State::getTransition(State* state) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (this->next[i]->state == state) {
            return i;
        }
    }
    return -1;
}

struct SuffixAutomata {
    State* source;
    int statesCount;
    int transitionsCount;
    int finalStatesCount;

    SuffixAutomata() :
        statesCount(0), transitionsCount(0), finalStatesCount(0) {}

    void build(std::string content);
    State* update(State* currentSink, char a);
    State* split(State* parent, State* child, int a);
};

void SuffixAutomata::build(std::string word) {
    this->source = new State;
    State* currentSink = this->source;
    this->statesCount++;

    currentSink->isFinal = true;
    this->finalStatesCount++;

    for (int i = 0; i < word.length(); i++) {
        currentSink = this->update(currentSink, word[i]);
    }
}

State* SuffixAutomata::update(State* currentSink, char a) {
    int index = a - FIRST_SYMBOL;

    currentSink->isFinal = false;

    State* newSink = new State;
    newSink->isFinal = true;
    this->statesCount++;

    Transition* primaryTransition = new Transition(newSink, true);
    currentSink->next[index] = primaryTransition;
    this->transitionsCount++;

    State* current = currentSink;
    State* suffixState = nullptr;

    while (current != this->source && suffixState == nullptr) {
        current = current->suffixLink;
        if (current->next[index] == nullptr) {
            Transition* secondaryTransition = new Transition(newSink);
            this->transitionsCount++;
            current->next[index] = secondaryTransition;
        } else if (current->next[index]->primary) {
            suffixState = current->next[index]->state;
        } else {
            State* childState = current->next[index]->state;
            suffixState = this->split(current, childState, index);
        }
    }

    if (suffixState == nullptr) suffixState = this->source;

    newSink->suffixLink = suffixState;
    return newSink;
}

State* SuffixAutomata::split(State* parent, State* child, int a) {
    State* newChildState = new State;
    this->statesCount++;

    newChildState->isFinal = true;
    this->finalStatesCount++;

    if (parent->isFinal) {
        parent->isFinal = false;
        this->finalStatesCount--;
    }

    parent->next[a]->state = newChildState;
    parent->next[a]->primary = true;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (child->next[i] != nullptr) {
            Transition* t = new Transition(child->next[i]->state);
            newChildState->next[i] = t;
            this->transitionsCount++;
        }
    }

    newChildState->suffixLink = child->suffixLink;
    child->suffixLink = newChildState;

    State* current = parent;
    while (current != this->source) {
        current = current->suffixLink;
        int a = current->getTransition(child);
        if (a > 0 && !current->next[a]->primary) {
            current->next[a]->state = newChildState;
        } else {
            break;
        }
    }
    return newChildState;
}

int main() {
    SuffixAutomata* automata = new SuffixAutomata;

    automata->build("abcbc");

    printf("states count: %d, transitions count: %d, final count: %d\n",
            automata->statesCount, automata->transitionsCount,
            automata->finalStatesCount);
    return 0;
}
