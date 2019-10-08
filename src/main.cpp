/******************************************************************************
 * main.cpp
 *
 * Project: TranslatedAutomata
 *
 * Main del progetto. Gestisce il flusso di esecuzione in maniera
 * centralizzata, richiamando le funzioni necessarie.
 *
 ******************************************************************************/

#include "main.hpp"

#include <iostream>
#include <string>
#include <set>

#include "state_nfa_test.hpp"
#include "debug.hpp"

using std::set;

using namespace translated_automata;

int main(int argc, char **argv) {
	DEBUG_MARK_PHASE( saluti ) {

		std::cout << "Hello world!" << std::endl;

	}
	DEBUG_MARK_PHASE( testing ) {
		test_stateNFA();
	}
	return 0;
}
