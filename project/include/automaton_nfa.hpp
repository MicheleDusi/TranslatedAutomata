/******************************************************************************
 * automaton_nfa.hpp
 *
 * Project: TranslatedAutomata
 *
 * Dichiarazione della classe "NFA", derivata dalla classe Automaton
 * parametrizzata sul tipo "StateNFA".
 *
 ******************************************************************************/

#ifndef INCLUDE_AUTOMATON_NFA_HPP_
#define INCLUDE_AUTOMATON_NFA_HPP_

#include "automaton.hpp"
#include "state_nfa.hpp"

namespace translated_automata {

	class NFA : public Automaton<StateNFA> {};

}

#endif /* INCLUDE_AUTOMATON_NFA_HPP_ */
