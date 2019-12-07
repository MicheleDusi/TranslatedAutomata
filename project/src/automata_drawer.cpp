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
	AutomataDrawer<Automaton>::AutomataDrawer(Automaton* automaton) {
		this->m_automaton = automaton;
	}

	/**
	 * Distruttore.
	 * XXX Assicurarsi che non distrugga l'automa membro.
	 */
	template <class Automaton>
	AutomataDrawer<Automaton>::~AutomataDrawer() {
		DEBUG_LOG("Sto eliminando l'oggetto \"Automata Drawer\"");
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
    	result += "AUTOMATON (size = " + std::to_string(this->m_automaton->size()) + ")\n";
        result += "Initial state: " + this->m_automaton->getInitialState()->getName() + '\n';

        // Per ogni stato dell'automa
        for (auto s : this->m_automaton->getStatesVector()) {
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
		// Apro lo stream su file
		std::ofstream out(filename, std::ios_base::out | std::ios_base::trunc);

		// Se il file non è aperto, restituisco un errore
		if (!out.is_open()) {
			DEBUG_LOG_ERROR("Impossibile scrivere il file \"%s\"", filename.c_str());
			return;
		}

		// Prefisso per la rappresentazione dell'automa
		out << "digraph finite_state_machine {\n"
				"rankdir=LR;\n"
				"size=\"8,5\"\n";

		// Rappresentazione di tutti gli stati
		for (auto state : this->m_automaton->getStatesVector()) {
			// Verifico se lo stato è final
			if (state->isFinal()) {
				out << "node [shape = doublecircle, label = \"" << state->getName() << "\", fontsize = 10] \"" << state->getName() << "\";\n";
			} else {
				out << "node [shape = circle, label = \"" << state->getName() << "\", fontsize = 10] \"" << state->getName() << "\";\n";
			}
		}

		// Rappresentazione dello stato iniziale
		out << "node [shape = point]; init\n";
		out << "init -> \"" << this->m_automaton->getInitialState()->getName() << "\"\n";

		// Rappresentazione delle transizioni
		for (auto state : this->m_automaton->getStatesVector()) {
			for (auto &pair : state->getExitingTransitions()) {
				for (auto child : pair.second) {
					out << "\"" << state->getName() << "\" -> \"" << child->getName() << "\" [ label = \"" << pair.first << "\" ];\n";
				}
			}
		}

		out << "}";

		// Chiusura del file
		out.close();
	}

	/* Istanziazione della classe DFADrawer */
	template class AutomataDrawer<DFA>;
	/* Istanziazione della classe NFADrawer */
	template class AutomataDrawer<NFA>;

} /* namespace translated_automata */
