/*
 * automata_drawer.hpp
 *
 * Modulo che si occupa della rappresentazione di un automa nelle seguenti forme:
 * - Forma testuale (come output su terminale).
 * - Forma grafica, tramite la libreria GraphViz, su file.
 */

#ifndef INCLUDE_AUTOMATA_DRAWER_HPP_
#define INCLUDE_AUTOMATA_DRAWER_HPP_

#include <string>

#include "automaton.hpp"

namespace translated_automata {

	/**
	 * Classe genitore generica per la rappresentazione
	 * di un automa. Viene istanziata e specificata nelle
	 * classi DFADrawer e NFADrawer.
	 */
	template <class Automaton>
	class AutomataDrawer {

	private:
		Automaton m_automaton;

	public:
		AutomataDrawer(const Automaton& automaton);
		virtual ~AutomataDrawer();

		string asString();
		void asDotFile(string filename);

	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATA_DRAWER_HPP_ */
