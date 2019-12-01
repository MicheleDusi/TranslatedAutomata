/******************************************************************************
 * automata_generator_nfa.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header della classe NFAGenerator, classe figlia di AutomataGenerator.
 * Fornisce le firme delle funzioni create specificamente per la generazione di
 * automi NFA.
 *
 ******************************************************************************/

#ifndef INCLUDE_AUTOMATA_GENERATOR_NFA_HPP_
#define INCLUDE_AUTOMATA_GENERATOR_NFA_HPP_

#include "automata_generator.hpp"
#include "automaton_nfa.hpp"

namespace translated_automata {

	class NFAGenerator : public AutomataGenerator<NFA> {

	private:
		void generateStates(NFA& nfa);
		StateNFA* getRandomState(NFA& nfa);
		unsigned long int computeTransitionsNumber();

	public:
		NFAGenerator(Alphabet alphabet);
		~NFAGenerator();

		NFA generateRandomAutomaton();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATA_GENERATOR_NFA_HPP_ */
