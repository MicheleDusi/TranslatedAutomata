/*
 * automaton_nfa.hpp
 *
 *  Created on: 15 ott 2019
 *      Author: thoozee
 */

#ifndef INCLUDE_AUTOMATON_NFA_HPP_
#define INCLUDE_AUTOMATON_NFA_HPP_

#include "automaton.hpp"
#include "state_nfa.hpp"
#include "state_dfa.hpp"

namespace translated_automata {

	/**
	 * Semi-istanziazione finale delle due classi concrete.
	 */
	class NFA : public Automaton<StateNFA> {};
	class DFA : public Automaton<StateDFA> {};

}

#endif /* INCLUDE_AUTOMATON_NFA_HPP_ */
