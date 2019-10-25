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
#include <tuple>

#include "../include/automata_generator_nfa.hpp"
#include "debug.hpp"

#include "automaton_nfa.hpp"
#include "automaton_dfa.hpp"
#include "embedded_subset_construction.hpp"
#include "subset_construction.hpp"
#include "translation.hpp"
#include "translation_generator.hpp"

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

//	DEBUG_MARK_PHASE( test generazione ) {
//
//		NFAGenerator *gen = new NFAGenerator();
//		gen->setSize(40);		// Si consigliano dimensioni inferiori ai 70 per il 10% di transizioni
//		gen->setFinalProbability(.2);
//		gen->setTransitionPercentage(.005);
//
//		NFA* new_nfa;
//
//		DEBUG_MARK_PHASE( generazione ) {
//			new_nfa = gen->generateRandomAutomaton();
//		}
//
//		DEBUG_MARK_PHASE( costruzione per sottoinsiemi ) {
//			SubsetConstruction *sc = new SubsetConstruction();
//			DFA* new_dfa = sc->run(new_nfa);
//
////			new_dfa->print();
//			std::cout << "Numero di stati del DFA: " << new_dfa->size() << std::endl;
//
//		}
//
//	}

//	DEBUG_MARK_PHASE( test traduzione ) {
//
//		// Genero l'automa
//		NFAGenerator *gen = new NFAGenerator();
//		Alphabet alpha = gen->generateAlphabet("abcdefgh", 8);
//		gen->setAlphabet(alpha);
//		gen->setSize(4);
//		gen->setFinalProbability(.4);
//		gen->setTransitionPercentage(.5);
//		NFA* nfa = gen->generateRandomAutomaton();
//
//		nfa->print();
//
//		// Genero la traduzione
//		TranslationGenerator* t_gen = new TranslationGenerator();
//		t_gen->setMixingFactor(0.5);
//		t_gen->setOffset(2);
//		Translation *tau = t_gen->generateRandomTranslation(alpha);
//		std::cout << tau->toString();
//
//		// Traduco
//		DEBUG_MARK_PHASE( traduzione ) {
//			tuple<NFA*, DFA*, list<Bud>> result = tau->translate(nfa);
//
//			DEBUG_LOG_SUCCESS("Traduzione completata!");
//
//			std::cout << "\033[31;1m NFA \033[0m\n";
//			NFA* translated_nfa = std::get<0>(result);
//			DEBUG_ASSERT_NOT_NULL( translated_nfa );
//			translated_nfa->print();
//
//			std::cout << "\033[31;1m DFA \033[0m\n";
//			DFA* translated_dfa = std::get<1>(result);
//			DEBUG_ASSERT_NOT_NULL( translated_dfa );
//			translated_dfa->print();
//
//			// Applico subset construction
//			std::cout << "\033[31;1m Constructed DFA with SC \033[0m\n";
//			SubsetConstruction *sc = new SubsetConstruction();
//			DFA *constructed_dfa = sc->run(translated_nfa);
//			DEBUG_ASSERT_NOT_NULL( constructed_dfa );
//			constructed_dfa->print();
//			std::cout << "Numero di stati del DFA: " << constructed_dfa->size() << std::endl;
//
//		}
//
//
//	}

	DEBUG_MARK_PHASE( Test translated automaton ) {

		// Genero l'automa
		NFAGenerator *gen = new NFAGenerator();
		Alphabet alpha = gen->generateAlphabet("abcde", 5);
		gen->setAlphabet(alpha);
		gen->setSize(5);
		gen->setFinalProbability(.4);
		gen->setTransitionPercentage(.5);
		NFA* nfa = gen->generateRandomAutomaton();

		nfa->print();

		// Genero la traduzione
		TranslationGenerator* t_gen = new TranslationGenerator();
		t_gen->setMixingFactor(0.5);
		t_gen->setOffset(2);
		Translation *tau = t_gen->generateTranslation(alpha);

		std::cout << tau->toString();

		EmbeddedSubsetConstruction *esc = new EmbeddedSubsetConstruction();



	}

	return 0;
}
