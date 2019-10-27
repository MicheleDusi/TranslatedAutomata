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
#include "constructed_state_dfa.hpp"
#include "translation.hpp"

namespace translated_automata {

	class EmbeddedSubsetConstruction {

	private:
		tuple<NFA*, DFA*, list<Bud>> runAutomatonTranslation(DFA *automaton, Translation *translation);
		void runBudProcessing(NFA* translated_nfa, DFA* translated_dfa, list<Bud> buds);
		void runDistanceRelocation(list<pair<StateDFA*, int>> relocation_sequence);
		void runDistanceRelocation(StateDFA* state, int new_distance);
		void runExtensionUpdate(ConstructedStateDFA* state, ExtensionDFA new_extension, list<Bud>& buds, DFA* dfa);

	public:
		EmbeddedSubsetConstruction();
		virtual ~EmbeddedSubsetConstruction();

		DFA* run(DFA* dfa, Translation* tau);

	};

} /* namespace translated_automata */

#endif /* INCLUDE_EMBEDDED_SUBSET_CONSTRUCTION_HPP_ */
