/******************************************************************************
 * embedded_subset_construction.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header per la classe "EmbeddedSubsetConstruction" che implementa l'algoritmo
 * "Embedded Subset Construction" per la traduzione di un automa DFA.
 *
 ******************************************************************************/

#ifndef INCLUDE_EMBEDDED_SUBSET_CONSTRUCTION_HPP_
#define INCLUDE_EMBEDDED_SUBSET_CONSTRUCTION_HPP_

#include "automaton_dfa.hpp"
#include "automaton_nfa.hpp"
#include "translation.hpp"

namespace translated_automata {

	class EmbeddedSubsetConstruction {

	private:
		void runBudProcessing(NFA* translated_nfa, DFA* translated_dfa, std::vector<Bud> buds);

	public:
		EmbeddedSubsetConstruction();
		virtual ~EmbeddedSubsetConstruction();

		DFA* run(DFA* dfa, Translation* tau);

	};

} /* namespace translated_automata */

#endif /* INCLUDE_EMBEDDED_SUBSET_CONSTRUCTION_HPP_ */
