/******************************************************************************
 * automata_generator_dfa.hpp
 *
 * Project: TranslatedAutomata
 *
 * Header contenente la definizione e le firme dei metodi della classe
 * concreta "DFAGenerator", figlia della classe "AutomataGenerator".
 * Questa classe ha la responsabilità di creare un automa a stati finiti Deterministico (!)
 * secondo i parametri pre-impostati.
 * La generazione sfrutta funzioni che generano valori casuali; pertanto, ogni chiamata
 * restituisce un automa differente.
 *
 ******************************************************************************/

#ifndef INCLUDE_AUTOMATA_GENERATOR_DFA_HPP_
#define INCLUDE_AUTOMATA_GENERATOR_DFA_HPP_

#include "automata_generator.hpp"
#include "automaton.hpp"

namespace translated_automata {

	class DFAGenerator : public AutomataGenerator<DFA> {

	private:
		void generateStates(DFA& nfa);
		StateDFA* getRandomState(DFA& nfa);
		StateDFA* getRandomStateWithUnusedLabels(vector<StateDFA*>& states, map<StateDFA*, Alphabet>& unused_labels);
		string extractRandomUnusedLabel(map<StateDFA*, Alphabet>& unused_labels, StateDFA* state);
		unsigned long int computeTransitionsNumber();

	public:
		DFAGenerator(Alphabet alphabet);
		~DFAGenerator();

		DFA* generateRandomAutomaton();
		DFA* generateStratifiedAutomaton();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATA_GENERATOR_DFA_HPP_ */
