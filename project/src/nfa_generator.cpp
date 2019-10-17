/******************************************************************************
 * nfa_generator.cpp
 *
 * Project: TranslatedAutomata
 *
 * File header della classe NFAGenerator, classe figlia di AutomataGenerator.
 * Fornisce le firme delle funzioni create specificamente per la generazione di
 * automi NFA.
 *
 ******************************************************************************/

#include "nfa_generator.hpp"

namespace translated_automata {

	NFAGenerator::NFAGenerator() {}

	NFAGenerator::~NFAGenerator() {
		this->resetNames();
	}

	NFA* NFAGenerator::generateRandomAutomaton() {
		return NULL;
	}

} /* namespace translated_automata */
