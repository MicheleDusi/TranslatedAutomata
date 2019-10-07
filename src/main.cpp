/*
 * main.cpp
 *
 */

#include "main.hpp"

#include <iostream>
#include <string>
#include <set>

#include "state.hpp"
#include "state_nfa.hpp"
#include "state_dfa.hpp"

using std::set;

//#define DEBUG_PRINTS

using namespace translated_automata;

#define NUM_STATES 10

set<StateNFA> * randomStatesSet() {
	set<StateNFA> * states;
	for (int i = 0; i < NUM_STATES; i++) {
		states->insert(StateNFA(std::to_string(i), false));
	}
	return states;
}

void createTransitions(set<StateNFA> &states, const set<string> alphabet) {
	std::cout << "HEYYYY" << std::endl;

	for (StateNFA state1 : states) {
		for (StateNFA state2 : states) {
			int random = rand();
			std::cout << "RANDOM: " << random << std::endl;
			if (random < 100) {
				state1.connect("", &state2);
			}
		}
	}
}

int main(int argc, char **argv) {
	std::cout << "Hello world!" << std::endl;

	set<string> alphabet = {"a", "b", "c", "d"};
	set<StateNFA> *states = randomStatesSet();
	createTransitions(*states, alphabet);

	for (StateNFA state : states) {
		std::cout << "Stato " << state.getName() << " : \n";
		for (auto &pair : state.getTransitions()) {// Ciclo su tutte le transizioni
			std::cout << "Label: " << pair.first << " to ";
			for (StateNFA* state : pair.second) {
				std::cout << state->getName() << ", ";
			}
			std::cout << std::endl;
		}
	}

	return 0;
}
