/*
 * nfa_generator.hpp
 *
 *  Created on: 17 ott 2019
 *      Author: thoozee
 */

#ifndef INCLUDE_NFA_GENERATOR_HPP_
#define INCLUDE_NFA_GENERATOR_HPP_

#include "automata_generator.hpp"
#include "automaton_nfa.hpp"

namespace translated_automata {

	class NFAGenerator : public AutomataGenerator<NFA> {

	public:
		NFAGenerator();
		virtual ~NFAGenerator();

		NFA* generateRandomAutomaton();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_NFA_GENERATOR_HPP_ */
