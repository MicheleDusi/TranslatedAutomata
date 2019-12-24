/*
 * EmbeddedSubsetConstruction.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header per la classe "EmbeddedSubsetConstruction" che implementa l'algoritmo
 * "Embedded Subset Construction" per la traduzione di un automa DFA.
 *
 */

#ifndef INCLUDE_EMBEDDEDSUBSETCONSTRUCTION_HPP_
#define INCLUDE_EMBEDDEDSUBSETCONSTRUCTION_HPP_

#include "Automaton.hpp"
#include "Bud.hpp"
#include "Translation.hpp"

namespace translated_automata {

	class EmbeddedSubsetConstruction {

	private:
		DFA* m_original_dfa;
		Translation* m_translation;

		BudsList* m_buds;
		NFA* m_translated_nfa;
		DFA* m_translated_dfa;

		void runDistanceRelocation(list<pair<StateDFA*, int>> relocation_sequence);
		void runDistanceRelocation(StateDFA* state, int new_distance);
		void runExtensionUpdate(ConstructedStateDFA* state, ExtensionDFA& new_extension);
		void runAutomatonPruning(Bud* bud);

		void addBudToList(ConstructedStateDFA* bud_state, string bud_label);

	public:
		EmbeddedSubsetConstruction();
		virtual ~EmbeddedSubsetConstruction();

		void loadInputs(DFA* automaton, Translation* translation);
		void runAutomatonTranslation();
		void runBudProcessing();
		DFA* getResult();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_EMBEDDEDSUBSETCONSTRUCTION_HPP_ */
