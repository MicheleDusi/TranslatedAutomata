/*
 * automata_drawer_impl.hpp
 *
 * Implementazione. TODO
 */

#ifndef INCLUDE_AUTOMATA_DRAWER_IMPL_HPP_
#define INCLUDE_AUTOMATA_DRAWER_IMPL_HPP_

#include "automata_drawer.hpp"

namespace translated_automata {

	/**
	 * Classe che si occupa della rappresentazione
	 * di automi DFA.
	 */
	class DFADrawer : public AutomataDrawer<DFA> {

	public:
		DFADrawer(const DFA& automaton) : AutomataDrawer(automaton) {};

	};

	/**
	 * Classe che si occupa della rappresentazione
	 * di automi NFA.
	 */
	class NFADrawer : public AutomataDrawer<NFA> {

	public:
		NFADrawer(const NFA& automaton) : AutomataDrawer(automaton) {};

	};


	/**
	 * Costruttore. Richiama il costruttore della classe genitore.
	 */
//	DFADrawer::DFADrawer(const DFA& automaton)
//	: AutomataDrawer(automaton) {};

	/**
	 * Costruttore. Richiama il costruttore della classe genitore.
	 */
//	NFADrawer::NFADrawer(const NFA& automaton)
//	: AutomataDrawer(automaton) {};

}

#endif /* INCLUDE_AUTOMATA_DRAWER_IMPL_HPP_ */
