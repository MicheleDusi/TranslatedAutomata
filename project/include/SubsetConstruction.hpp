/*
 * SubsetConstruction.hpp
 *
 * Project: TranslatedAutomata
 *
 * Header del file subset_constrution.cpp.
 * Implementa l'algoritmo di "Subset Construction" per la generazione di DFA
 * equivalenti ad un NFA di partenza.
 * L'algoritmo è implementato come unico metodo di una classe chiamata "Subset
 * Construction"; accetta in ingresso un NFA e restituisce un DFA.
 *
 */

#ifndef INCLUDE_SUBSETCONSTRUCTION_HPP_
#define INCLUDE_SUBSETCONSTRUCTION_HPP_

#include "Automaton.hpp"

namespace translated_automata {

	class SubsetConstruction {

	public:
		DFA * run(NFA *nfa);

	};
}

#endif /* INCLUDE_SUBSETCONSTRUCTION_HPP_ */
