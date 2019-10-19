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

#include "debug.hpp"

#include "automaton_nfa.hpp"
#include "automaton_dfa.hpp"
#include "subset_construction.hpp"
#include "nfa_generator.hpp"

using std::set;

using namespace translated_automata;

int main(int argc, char **argv) {
	DEBUG_MARK_PHASE( saluti ) {

		std::cout << "Hello world!" << std::endl;

	}

//	DEBUG_MARK_PHASE( subset construction ) {
//
//		NFA* nfa = new NFA();
//		StateNFA *s0 = new StateNFA("s0", false);
//		nfa->addState(s0);
//		nfa->setInitialState(s0);
//		s0->connectChild("a", s0);
//		s0->connectChild("b", s0);
//		s0->connectChild("c", s0);
//		s0->connectChild("d", s0);
//
//		SubsetConstruction *sc = new SubsetConstruction();
//		DFA *dfa = sc->run(nfa);
//
//		dfa->print();
//		std::cout << "Numero di stati del DFA: " << dfa->size() << std::endl;
//
//	}

	DEBUG_MARK_PHASE( test generazione ) {

		NFAGenerator *gen = new NFAGenerator();
		gen->setSize(10);
		gen->setFinalProbability(.2);
		gen->setTransitionPercentage(.1);

		NFA* new_nfa;

		DEBUG_MARK_PHASE( generazione ) {
			new_nfa = gen->generateRandomAutomaton();

			DEBUG_MARK_PHASE( stampa )
//				new_nfa->print();
		}

		DEBUG_MARK_PHASE( costruzione per sottoinsiemi ) {
			SubsetConstruction *sc = new SubsetConstruction();
			DFA* new_dfa = sc->run(new_nfa);

//			new_dfa->print();
			std::cout << "Numero di stati del DFA: " << new_dfa->size() << std::endl;

		}

	}

	return 0;
}
