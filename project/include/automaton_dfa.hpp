/******************************************************************************
 * automaton_dfa.hpp
 *
 * Project: TranslatedAutomata
 *
 * Dichiarazione della classe "DFA", derivata dalla classe Automaton
 * parametrizzata sul tipo "StateDFA".
 *
 ******************************************************************************/

#ifndef INCLUDE_AUTOMATON_DFA_HPP_
#define INCLUDE_AUTOMATON_DFA_HPP_

#include "automaton.hpp"
#include "state_dfa.hpp"

namespace translated_automata {

	class DFA : public Automaton<StateDFA> {};

}

#endif /* INCLUDE_AUTOMATON_DFA_HPP_ */
