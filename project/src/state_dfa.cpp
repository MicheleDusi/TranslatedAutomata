/******************************************************************************
 * state_dfa.cpp
 *
 * Project: TranslatedAutomata
 *
 * Sorgente contenente l'implementazione della classe concreta "StateDFA", figlia
 * della classe astratta "State".
 * Concettualmente, un oggetto "StateDFA" rappresenta uno stato di un DFA, ossia
 * di un automa a stati finiti deterministico.
 * Un oggetto StateDFA ha la caratteristica di avere un solo nodo figlio per
 * ciascuna (eventuale) label. Questo implica che non avrà mai più di N transizioni
 * uscenti, dove N è la cardinalità dell'alfabeto.
 *
 ******************************************************************************/

#include "state_dfa.hpp"

#include <vector>

#include "debug.hpp"

using std::vector;

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	StateDFA::StateDFA(string name, bool final) {
		this->m_name = name;
		this->m_final = final;
    }

	/**
	 * Distruttore.
	 */
    StateDFA::~StateDFA() {}

	/**
	 * Restituisce lo stato raggiunto da una transizione con una specifica etichetta.
	 * In pratica, fa le stesse operazioni del metodo "getChildren" ma prende solamente il
	 * primo nodo.
	 * Se non viene trovato alcun figlio relativo alla label passata come argomento,
	 * viene restituito un valore nullo.
	 */
	StateDFA* StateDFA::getChild(string label) {
		set<StateDFA*> children = this->getChildren(label);
		if (children.empty()) {
			return NULL;
		} else {
			if (children.size() > 1) {
				DEBUG_LOG_ERROR("Il nodo DFA \"%s\" contiene più di un figlio", this->getName().c_str());
			}
			return *(children.begin());
		}
	}

}
