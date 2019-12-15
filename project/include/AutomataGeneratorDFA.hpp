/*
 * AutomataGeneratorDFA.hpp
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
 */

#ifndef INCLUDE_AUTOMATAGENERATORDFA_HPP_
#define INCLUDE_AUTOMATAGENERATORDFA_HPP_

#include "AutomataGenerator.hpp"
#include "Automaton.hpp"

namespace translated_automata {

	enum DFAType {
		DFA_RANDOM,
		DFA_STRATIFIED
	};

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

		DFA* generateAutomaton(DFAType type);
		DFA* generateRandomAutomaton();
		DFA* generateStratifiedAutomaton();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATAGENERATORDFA_HPP_ */
