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

#include "automaton.hpp"
#include "bud.hpp"
#include "constructed_state_dfa.hpp"
#include "translation.hpp"

namespace translated_automata {

	class EmbeddedSubsetConstruction {

		/* TODO
		 * Fare in modo che la classe abbia degli attributi membri, almeno
		 * relativi al DFA e NFA "originali", in modo da non tirarseli dietro nei vari metodi.
		 */

	private:
		void runAutomatonTranslation(DFA* automaton, Translation* translation, NFA* translated_nfa, DFA* translated_dfa, BudsList& buds_list);
		void runBudProcessing(NFA* translated_nfa, DFA* translated_dfa, BudsList& buds);
		void runDistanceRelocation(list<pair<StateDFA*, int>> relocation_sequence);
		void runDistanceRelocation(StateDFA* state, int new_distance);
		void runExtensionUpdate(ConstructedStateDFA* state, ExtensionDFA& new_extension, BudsList& buds, DFA* dfa);

	public:
		EmbeddedSubsetConstruction();
		virtual ~EmbeddedSubsetConstruction();

		DFA* run(DFA* original_dfa, Translation* tau);

	};

} /* namespace translated_automata */

#endif /* INCLUDE_EMBEDDED_SUBSET_CONSTRUCTION_HPP_ */
