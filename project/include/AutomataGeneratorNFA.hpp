/*
 * AutomataGeneratorNFA.hpp
 *
 * Header contenente la definizione e le firme dei metodi della classe
 * concreta "AutomataGeneratorNFA", figlia della classe "AutomataGenerator".
 * Questa classe ha la responsabilità di creare un automa a stati finiti NON deterministico (!)
 * secondo i parametri pre-impostati.
 * La generazione sfrutta funzioni che generano valori casuali; pertanto, ogni chiamata
 * restituisce un automa differente.
 */

#ifndef INCLUDE_AUTOMATAGENERATORNFA_HPP_
#define INCLUDE_AUTOMATAGENERATORNFA_HPP_

#include "AutomataGenerator.hpp"

namespace translated_automata {

	class NFAGenerator: public AutomataGenerator<NFA> {

	private:
		void generateStates(NFA* nfa);
		StateNFA* getRandomState(NFA* nfa);
		StateNFA* getRandomState(vector<StateNFA*>& states);
		StateNFA* getRandomStateWithUnusedLabels(vector<StateNFA*>& states, map<StateNFA*, Alphabet>& unused_labels);
		string extractRandomUnusedLabel(map<StateNFA*, Alphabet>& unused_labels, StateNFA* state);

	public:
		NFAGenerator(Alphabet alphabet);
		~NFAGenerator();

		NFA* generateStratifiedWithSafeZoneAutomaton();
	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATAGENERATORNFA_HPP_ */
