/*
 * automata_drawer.cpp
 *
 * Modulo che si occupa, in generale, della rappresentazione di automi.
 */

#include "automata_drawer.hpp"

#include <fstream>

#include "state_nfa.hpp"
#include "state_dfa.hpp"
#include "debug.hpp"

namespace translated_automata {

	/**
	 * Costruttore.
	 * Richiede in input l'automa da visualizzare.
	 * Precondizione: l'automa non deve essere nullo.
	 */
	template <class Automaton>
	AutomataDrawer<Automaton>::AutomataDrawer(const Automaton& automaton) {
		this->m_automaton = automaton;
	}

	/**
	 * Distruttore.
	 * XXX Assicurarsi che non distrugga l'automa membro.
	 */
	template <class Automaton>
	AutomataDrawer<Automaton>::~AutomataDrawer() {
		DEBUG_LOG("Sto eliminando l'oggetto \"AUTOMATA DRAWER\", assicurarsi che non si stia eliminando l'automa membro.");
	}

	/**
	 * Restituisce una descrizione testuale di tutti gli stati appartenenti all'automa,
     * evidenziando lo stato iniziale. E' importante che ogni stato sia stato istanziato correttamente.
     * Lo stato iniziale deve essere stato impostato correttamente.
     *
	 * La descrizione testuale può essere visualizzata su terminale. Nota: vengono utilizzati i caratteri ANSI escape.
	 */
	template <class Automaton>
	string AutomataDrawer<Automaton>::asString() {
    	string result = "";
    	result += "AUTOMATON (size = " + std::to_string(this->m_automaton.size()) + ")\n";
        result += "Initial state: " + this->m_automaton.getInitialState()->getName() + '\n';

        // Per ogni stato dell'automa
        for (auto s : this->m_automaton.getStatesVector()) {
            result += s->toString();
        }

        return result;
	}

    /**
     * Restituisce una descrizione testuale di tutti gli stati appartenenti all'automa,
     * evidenziando lo stato iniziale.
     * Ogni stato deve essere stato istanziato correttamente.
     * Lo stato iniziale deve essere stato impostato correttamente.
     */
	template <class Automaton>
	void AutomataDrawer<Automaton>::asDotFile(string filename) {

	}

	/* Istanziazione della classe DFADrawer */
	template class AutomataDrawer<DFA>;
	/* Istanziazione della classe NFADrawer */
	template class AutomataDrawer<NFA>;

} /* namespace translated_automata */
