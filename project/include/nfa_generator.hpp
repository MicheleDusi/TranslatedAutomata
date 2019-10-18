/******************************************************************************
 * nfa_generator.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header della classe NFAGenerator, classe figlia di AutomataGenerator.
 * Fornisce le firme delle funzioni create specificamente per la generazione di
 * automi NFA.
 *
 ******************************************************************************/

#ifndef INCLUDE_NFA_GENERATOR_HPP_
#define INCLUDE_NFA_GENERATOR_HPP_

#include "automata_generator.hpp"
#include "automaton_nfa.hpp"

namespace translated_automata {

	class NFAGenerator : public AutomataGenerator<NFA> {

	public:
		NFAGenerator();
		virtual ~NFAGenerator();

		virtual NFA* generateRandomAutomaton();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_NFA_GENERATOR_HPP_ */
