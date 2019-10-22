/******************************************************************************
 * translation.cpp
 *
 * Project: TranslatedAutomata
 *
 * File sorgente per la classe Translation.
 * La classe modellizza una traduzione operata su un alfabeto, ed è implementata come
 * mappa che associa ciascun valore di un alfabeto ad un altro.
 *
 * Nota: memorizza unicamente le associazioni differenti dall'identità.
 *
 ******************************************************************************/

#include "translation.hpp"

#include <cstring>
#include <algorithm>

#include "debug.hpp"

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
	 * Metodo che applica la traduzione su un automa, secondo l'algoritmo "Automaton Translation".
	 *
	 * INPUT:
	 * @param automaton L'automa da tradurre.
	 * @param translation (parametro implicito) La traduzione da applicare all'automa
	 *
	 * OUTPUT:
	 * @return L'automa NFA tradotto.
	 * @return L'automa DFA isormorfo al precedente, che tuttavia contiene ancora punti di non-determinismo. Risulta utile ai fini dell'algoritmo "Bud Processing".
	 * @return La lista di Bud (coppie <StateDFA, string>) generata durante la traduzione.
	 *
	 */
	template <class State>
	tuple<NFA*, DFA*, vector<Bud>> Translation::translate(Automaton<State> *automaton) {
		// Preparazione degli output
		NFA* translated_nfa = new NFA();
		DFA* translated_dfa = new DFA();
		vector<Bud> buds;

		// Variabili locali ausiliarie
		map<State*, pair<StateNFA*, StateDFA*>> states_map;	// Mantiene le associazioni fra gli stati tradotti

		// Iterazione su tutti gli stati dell'automa in input per creare gli stati corrispondenti
		for (State* state : automaton->getStatesVector()) {

			// Creo uno stato copia nell'NFA
			StateNFA* translated_nfa_state = new StateNFA(state->getName(), state->isFinal());
			// Lo aggiungo all'NFA
			translated_nfa->addState(translated_nfa_state);

			// Creo uno stato copia nel DFA
			ExtensionDFA extension;
			extension.insert(translated_nfa_state);
			StateDFA* translated_dfa_state = new StateDFA(extension);
			// Lo aggiungo al DFA
			translated_dfa->addState(translated_dfa_state);

			// Associo allo stato originale i due nuovi stati, in modo da poterli ritrovare facilmente
			states_map[state] = pair<StateNFA*, StateDFA*>(translated_nfa_state, translated_dfa_state);

		}

		// Termino il primo ciclo su tutti gli stati dell'automa, in modo da procedere
		// solamente quando le associazioni fra gli stati sono complete

		// Marco gli stati iniziali
		translated_nfa->setInitialState(states_map[automaton->getInitialState()].first);
		translated_dfa->setInitialState(states_map[automaton->getInitialState()].second);

		// Iterazione su tutti gli stati dell'automa in input per copiare le transizioni
		for (State* state : automaton->getStatesVector()) {

			// Vengono recuperati gli stati creati in precedenza, associati allo stato dell'automa originale
			StateNFA* translated_nfa_state = states_map[state].first;
			StateDFA* translated_dfa_state = states_map[state].second;

			// Iterazione su tutte le transizioni uscenti dallo stato dell'automa
			for (auto &pair : state->getExitingTransitions()) {

				// Traduzione della label
				string translated_label = this->translate(pair.first);

				// Per tutti gli stati figli raggiunti da transizioni marcate con la label originaria
				for (State* child : pair.second) {

					// Inserisco le transizioni (tradotte) negli automi copiati
					translated_nfa_state->connectChild(translated_label, states_map[child].first);
					translated_dfa_state->connectChild(translated_label, states_map[child].second);

				}

			}

			// Verifica dei punti di non-determinismo
			// Per tutte le transizioni uscenti dallo stato del DFA
			for (auto &pair : translated_dfa_state->getExitingTransitions()) {

				// Verifico se gli stati raggiunti dalle transizioni marcate con quest'etichetta
				// sono più di uno.
				if (pair.second.size() > 1) {
					Bud new_bud = Bud(translated_dfa_state, pair.first);
					buds.push_back(new_bud);
				}
			}

		}

		// Restituzione dei valori in output
		return tuple<NFA*, DFA*, vector<Bud>>(translated_nfa, translated_dfa, buds);
	}

	/** Istanziazione delle funzioni template */
	template tuple<NFA*, DFA*, vector<Bud>> Translation::translate<StateNFA>(Automaton<StateNFA> *nfa);
	template tuple<NFA*, DFA*, vector<Bud>> Translation::translate<StateDFA>(Automaton<StateDFA> *dfa);

	/**
	 * Restituisce una descrizione testuale della traduzione, come lista
	 * delle associazioni.
	 */
	string Translation::toString() {
		string s = "";
		for (auto &pair : this->m_translation_map) {
			s += pair.first + " >> " + pair.second + "\n";
		}
		if (this->m_translation_map.empty()) {
			s += "No explicit associations";
		}
		return s;
	}

} /* namespace translated_automata */
