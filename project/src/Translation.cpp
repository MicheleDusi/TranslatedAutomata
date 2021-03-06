/*
 * Translation.cpp
 *
 * Project: TranslatedAutomata
 *
 * File sorgente per la classe Translation.
 * La classe modellizza una traduzione operata su un alfabeto, ed è implementata come
 * mappa che associa ciascun valore di un alfabeto ad un altro.
 *
 * Nota: memorizza unicamente le associazioni differenti dall'identità.
 *
 */

#include "Translation.hpp"

#include <cstring>
#include <algorithm>

#include "Debug.hpp"

namespace translated_automata {

	/**
	 * Costruttore.
	 * Memorizza unicamente le associazioni (label->label_tradotta) differenti dall'identità.
	 * A differenza dell'altro costruttore, questo opera un controllo agiuntivo:
	 * per ciascuna associazione, verifica che la label di dominio appartenga all'alfabeto passato
	 * come parametro; se non vi appartiene, viene lanciata un'eccezione.
	 */
	Translation::Translation(Alphabet alpha, map<string, string> translation_map) {
		for (auto &pair : translation_map) {

			// Verifico se la label di dominio appartiene o meno all'alfabeto
			auto search = std::find(alpha.begin(), alpha.end(), pair.first);
			if (search == alpha.end()) {
				// Rimuovo tutti gli elementi inseriti finora
				this->m_translation_map.clear();
				// Lancio un'eccezione
				throw "Impossibile istanziare una traduzione se tutti gli elementi non appartengono all'alfabeto";
			}

			// Se l'associazione è differente dall'identità
			if (strcmp(pair.first.c_str(), pair.second.c_str()) != 0) {
				this->m_translation_map.insert(pair);
			}
		}
	}

	/**
	 * Costruttore.
	 * Memorizza unicamente le associazioni (label->label_tradotta) differenti dall'identità.
	 */
	Translation::Translation(map<string, string> translation_map) {
		for (auto &pair : translation_map) {
			// Se l'associazione è differente dall'identità
			if (strcmp(pair.first.c_str(), pair.second.c_str()) != 0) {
				this->m_translation_map.insert(pair);
			}
		}
	}

	/**
	 * Costruttore.
	 * Verifica che tutte le associazioni memorizzate all'interno della mappa appartengano all'alfabeto.
	 */
	Translation::~Translation() {
		this->m_translation_map.clear();
	}

	/**
	 * Opera la tradizione su una singola stringa.
	 * Se la stringa è associata ad una stringa specifica all'interno della traduzione,
	 * questa funzione restituisce la stringa tradotta.
	 * Altrimenti viene restituita la stringa originaria.
	 */
	string Translation::translate(string label) {
		// Verifico se la label originale è presente nella traduzione
		if (this->m_translation_map.count(label)) {
			// Nel caso sia presente, la "traduco"
			return this->m_translation_map[label];
		} else {
			// Altrimenti, se la label NON è presente, viene lasciata inalterata
			return label;
		}
	}

	/**
	 * Opera la traduzione su un intero alfabeto.
	 * Questo metodo richiama il metodo "translate" su ogni stringa dell'alfabeto.
	 * Se una stringa non è specificata all'interno della traduzione, viene lasciata inalterata.
	 */
	Alphabet Translation::translate(Alphabet alpha) {
		Alphabet new_alpha;
		for (string label : alpha) {
			new_alpha.push_back(this->translate(label));
		}
		return new_alpha;
	}

	/**
	 * Traduce un automa creandone una copia con tutte le label tradotte.
	 * Poiché possono generarsi punti di non determinismo (transizioni uscenti
	 * dallo stesso stato marcate con la medesima label) l'automa restituito
	 * in output è un NFA.
	 */
	NFA* Translation::translate(DFA* dfa) {
		// Istanzio un automa NFA, che verrà restituito in output al termine
		NFA* translated_nfa = new NFA();
		// Istanzio un dizionario per associare agli stati originali quelli nuovi.
		std::map<StateDFA*, StateNFA*> states_map = map<StateDFA*, StateNFA*>();

		// Creo le copie degli stati, per il momento senza transizioni
		for (StateDFA* state : dfa->getStatesVector()) {
			StateNFA* new_state = new StateNFA(state->getName(), state->isFinal());
			states_map[state] = new_state;
			translated_nfa->addState(new_state);
		}

		// Creo e collego le transizioni (tradotte!)
		for (auto &states_pair : states_map) {

			// Per ciascuna transizione uscente dallo stato DFA originale
			for (auto &trans_pair : states_pair.first->getExitingTransitionsRef()) {
				for (StateDFA* child : trans_pair.second) {

					// Creo la transizione corrispondente nello stato NFA associato
					states_pair.second->connectChild(this->translate(trans_pair.first), states_map[child]);
					// Nota: questo crea anche le transizioni entranti nei figli, in automatico.
				}
			}
		}
		// Impostazione dello stato iniziale
		translated_nfa->setInitialState(states_map[dfa->getInitialState()]);

		return translated_nfa;
	}

	/**
	 * Restituisce una descrizione testuale della traduzione, come lista
	 * delle associazioni.
	 * Le traduzioni identiche non vengono stampate.
	 */
	string Translation::toString() {
		string s = "Translation:\n";
		for (auto &pair : this->m_translation_map) {
			s += " " + SHOW(pair.first) + " ⟼   " + SHOW(pair.second) + "\n";
		}
		if (this->m_translation_map.empty()) {
			s += "No explicit associations.";
		}
		return s;
	}

	/**
	 * Restituisce una descrizione testuale della traduzione, come lista
	 * delle associazioni in riferimento a tutte e solo le stringhe contenute
	 * all'interno di un alfabeto.
	 */
	string Translation::toString(Alphabet &reference) {
		string s = "Translation:\n";
		for (auto label : reference) {
			s += " " + SHOW(label) + " ⟼   " + SHOW(this->translate(label)) + "\n";
		}
		if (reference.empty()) {
			s += "No labels in the alphabet.";
		}
		return s;
	}

} /* namespace translated_automata */
