/*
 * AutomataDrawer_impl.hpp
 *
 * Implementazione della classe template AutomataDrawer:
 * DFADrawer --> AutomataDrawer<DFA>.
 * NFADrawer --> AutomataDrawer<NFA>.
 */

#ifndef INCLUDE_AUTOMATADRAWER_IMPL_HPP_
#define INCLUDE_AUTOMATADRAWER_IMPL_HPP_

#include "AutomataDrawer.hpp"

namespace translated_automata {

	/**
	 * Classe che si occupa della rappresentazione
	 * di automi DFA.
	 */
	class DFADrawer : public AutomataDrawer<DFA> {

	public:
		DFADrawer(DFA* automaton) : AutomataDrawer(automaton) {};

	};

	/**
	 * Classe che si occupa della rappresentazione
	 * di automi NFA.
	 */
	class NFADrawer : public AutomataDrawer<NFA> {

	public:
		NFADrawer(NFA* automaton) : AutomataDrawer(automaton) {};

	};

}

#endif /* INCLUDE_AUTOMATADRAWER_IMPL_HPP_ */
