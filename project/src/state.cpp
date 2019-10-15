/******************************************************************************
 * state.cpp
 *
 * Project: TranslatedAutomata
 *
 * File sorgente per l'implementazione della classe astratta "State", definita
 * nel file header "state.hpp".
 * Nel dominio concettuale, uno "State" è uno stato di un "automa a stati finiti",
 * sia esso deterministico o non deterministico. E' caratterizzato da un nome
 * univoco ed è legato ad altri stati mediante transizioni, ciascuna delle quali
 * è marcata da una label.
 *
 ******************************************************************************/

#include "state.hpp"

#include <string>
#include <map>
#include <set>

#include "debug.hpp"
#include "state_dfa.hpp"
#include "state_nfa.hpp"

using std::string;
using std::map;
using std::set;

namespace translated_automata {

	/**
	 * Classe astratta "State".
	 * Viene implementata da StateNFA e StateDFA.
	 */

	/**
	 * Metodo privato.
	 * Restituisce il puntatore all'oggetto corrente.
	 */
	template <class S>
	S* State<S>::getThis() const {
		return (S*) this;
	}

	/**
	 * Costruttore della classe State.
	 * Inizializza come vuoti gli insiemi di transizioni entranti e uscenti.
	 */
	template <class S>
	State<S>::State ()
	: m_exiting_transitions(), m_incoming_transitions() {
		DEBUG_LOG_SUCCESS( "Nuovo oggetto State creato correttamente" );
	}

	/**
	 * Distruttore della classe State.
	 */
	template <class S>
	State<S>::~State () {
		DEBUG_LOG( "Distruzione dell'oggetto State \"%s\"", m_name.c_str() );
		detach();
		DEBUG_LOG_SUCCESS( "Oggetto State distrutto correttamente" );
	}

	/**
	 * Restituisce il nome dello stato.
	 */
	template <class S>
	string State<S>::getName() const {
		return m_name;
	}

	/**
	 * Metodo privato.
	 * Rimuove una transizione dallo stato al figlio.
	 * Se non esiste non succede nulla.
	 */
	template <class S>
	void State<S>::removeChild(string label, S* child)	{
		m_exiting_transitions[label].erase(child);
		if (m_exiting_transitions[label].empty()) {
			m_exiting_transitions.erase(label);
		}
	}

	/**
	 * Metodo privato.
	 * Come "removeChild", ma rimuovo una transizione entrante.
	 */
	template <class S>
	void State<S>::removeParent(string label, S* parent) {
		m_incoming_transitions[label].erase(parent);
		if (m_incoming_transitions[label].empty()) {
			m_incoming_transitions.erase(label);
		}
	}

	/**
	 * Collega lo stato soggetto allo stato passato come parametro, con una transizione
	 * etichettata dalla label "label" passata come parametro.
	 */
	template <class S>
	void State<S>::connectChild(string label, S* child)	{
		// Aggiungo una transizione uscente da questo stato
		m_exiting_transitions[label].insert(child);
		// Aggiungo una transizione entrante allo stato di arrivo
		child->m_incoming_transitions[label].insert(getThis());
	}

	/**
	 * Disconnette due stati, rimuovendo la l-transizione che parte da questo
	 * stato e arriva in quello passato come parametro.
	 *
	 * Precondizione: si suppone che tale transizione sia esistente
	 */
	template <class S>
	void State<S>::disconnectChild(string label, S* child) {
		// Rimuovo la transizione uscente da questo stato
		removeChild(label, child);
		// Rimuovo la transizione entrante dal
		child->removeParent(label, getThis());
	}

	/**
	 * Rimuove tutte le transizioni entranti e uscenti
	 * da questo stato.
	 * Le transizioni vengono aggiornate anche sui nodi che
	 * risultavano precedentemente connessi.
	 */
	template <class S>
	void State<S>::detach() {
		// Rimuove le transizioni uscenti
		for (auto pair: m_exiting_transitions) {
			string label = pair.first;
			for (S* child : pair.second) {
				child->removeParent(label, getThis());
			}
		}

		// Rimuove le transizioni entranti
		for (auto pair: m_incoming_transitions) {
			string label = pair.first;
			for (S* parent: pair.second) {
				parent->removeChild(label, getThis());
			}
		}
	}

	/**
	 * Metodo protetto.
	 * Restituisce l'insieme di stati raggiunti da tutte le transizioni
	 * uscenti da questo nodo marcate con la label "l" passata come parametro.
	 * In pratica, restituisce la l-closure di questo nodo.
	 *
	 * Nota: questo metodo verrà "wrappato" dalla classe StateNFA, poiché per un NFA
	 * è possibile avere più figli marcati con la stessa label. Nel caso di uno stato
	 * StateDFA, invece, sarà opportuno operare alcuni controlli per verificare che
	 * esista un unico figlio per ciascuna label.
	 */
	template <class S>
	set<S*> State<S>::getChildren(string label) {
		auto search = m_exiting_transitions.find(label);
		// Con "auto" sto esplicitando il processo di type-inference
		if (search != m_exiting_transitions.end()) {
			// Restituisco i nodi alla transizione uscente
			return m_exiting_transitions[label];
		} else {
			// Restituisco un insieme vuoto
			return set<S*>();
		}
	}

	/**
	 * Restituisce lo stato raggiunto da una transizione con una specifica etichetta.
	 * In caso ne sia presente più di uno (caso NFA), restituisce solo il primo della lista.
	 * Per ottenere tutti gli stati raggiunti, è opportuno chiamare il metodo "getChildren".
	 */
	template <class S>
	S* State<S>::getChild(string label) {
		auto search = m_exiting_transitions.find(label);
		// Con "auto" sto esplicitando il processo di type-inference
		if (search != m_exiting_transitions.end()) {
			// Restituisco il primo nodo della lista
			return *(m_exiting_transitions[label].begin());
		}	else {
			// Altrimenti non restituisco nulla
			return NULL;
		}
	}

	/**
	 * Restituisce gli stati che hanno un transizione marcata da una specifica label
	 * che punta a questo stato. In pratica, tutti gli stati "padri" secondo una certa
	 * label.
	 */
	template <class S>
	set<S*> State<S>::getParents(string label) {
		auto search = m_incoming_transitions.find(label);
		if (search != m_incoming_transitions.end()) {
			return m_incoming_transitions[label];
		} else {
			return set<S*>();
		}
	}

	/**
	 * Verifica se lo stato soggetto ha una transizione USCENTE
	 * marcata con la label passata come parametro.
	 */
	template <class S>
	bool State<S>::hasExitingTransition(string label)	{
		auto search = m_exiting_transitions.find(label);
		return search != m_exiting_transitions.end();
	}

	/**
	 * Verifica se lo stato soggetto ha una transizione USCENTE che vada
	 * allo stato "child" e che sia marcata con l'etichetta "label".
	 */
	template <class S>
	bool State<S>::hasExitingTransition(string label, S* child) {
		auto search = m_exiting_transitions.find(label);
		if (search != m_exiting_transitions.end()) {
			return search->second.count(child);
		} else {
			return false;
		}
	}

	/**
	 * Verifica se lo stato soggetto ha una transizione ENTRANTE
	 * marcata con la label passata come parametro.
	 */
	template <class S>
	bool State<S>::hasIncomingTransition(string label)	{
		auto search = m_incoming_transitions.find(label);
		return search != m_incoming_transitions.end();
	}

	/**
	 * Verifica se lo stato soggetto ha una transizione ENTRANTE
	 * che parta dallo stato "child" e che sia marcata con l'etichetta "label".
	 */
	template <class S>
	bool State<S>::hasIncomingTransition(string label, S* child) {
		auto search = m_incoming_transitions.find(label);
		if (search != m_incoming_transitions.end()) {
			return search->second.count(child);
		} else {
			return false;
		}
	}

	/**
	 * Restituisce la mappa di transizioni uscenti da questo stato.
	 */
	template <class S>
	map<string, set<S*>> State<S>::getExitingTransitions() {
		return m_exiting_transitions;
	}

	/**
	 * Restituisce la mappa di transizioni entranti in questo stato.
	 */
	template <class S>
	map<string, set<S*>> State<S>::getIncomingTransitions() {
		return m_incoming_transitions;
	}

	/**
	 * Restituisce una reference alla mappa di transizioni, ossia l'indirizzo della memoria
	 * in cui la mappa è salvata.
	 * Restituire un indirizzo permette di usare questo metodo come lvalue in un assegnamento, ad esempio.
	 */
	template <class S>
	const map<string, set<S*>>& State<S>::getExitingTransitionsRef() {
		return m_exiting_transitions;
	}

	/**
	 * Restituisce una reference alla mappa di transizioni entranti, ossia l'indirizzo della memoria
	 * in cui la mappa è salvata.
	 * Restituire un indirizzo permette di usare questo metodo come lvalue in un assegnamento, ad esempio.
	 */
	template <class S>
	const map<string, set<S*>>& State<S>::getIncomingTransitionsRef() {
		return m_incoming_transitions;
	}

	/**
	 * Conta le transizioni uscenti dallo stato.
	 * Per ciascuna label, conteggia la quantità di transizioni riferite a tale label uscenti
	 * dallo stato corrente.
	 */
	template <class S>
	int State<S>::getExitingTransitionsCount() {
		int count = 0;
		for (auto &pair: m_exiting_transitions) {// Ciclo su tutti gli elementi della mappa
			count += pair.second.size();
		}
		// "second" prende il secondo elemento della coppia,
		// in questo caso l'insieme di stati raggiunti dalle transizioni con label predefinita
		return count;
	}

	/**
	 * Conta le transizioni entranti nello stato.
	 */
	template <class S>
	int State<S>::getIncomingTransitionsCount() {
		int count = 0;
		for (auto &pair: m_incoming_transitions) {
			count += pair.second.size();
		}
		return count;
	}

	/**
	 * Verifica se lo stato ha le stesse transizioni (entranti E uscenti) dello stato
	 * passato come parametro.
	 */
	template <class S>
	bool State<S>::hasSameTransitions(S* s) {
		// Verifico che il numero di transizioni uscenti sia uguale
		if (this->m_exiting_transitions.size() != s->m_exiting_transitions.size()) {
			return false;
		}

		// Per tutte le transizioni uscenti
		for (auto &pair: m_exiting_transitions) {
			string label = pair.first;
			set<S*> other_children = s->m_exiting_transitions[label];

			// Verifico che il numero di figli sia uguale
			if (pair.second.size() != other_children.size()) {
				return false;
			}

			// Verifico che tutti i figli siano coincidenti
			for (auto &child: pair.second) {
				// Se il figlio non è contenuto nell'altra lista
				if (other_children.count(child) <= 0) {
					// TODO Verificare che il controllo sia corretto
					return false;
				}
			}
		}

		// Verifico che il numero di transizioni entranti sia uguale
		if (this->m_incoming_transitions.size() != s->m_incoming_transitions.size()) {
			return false;
		}

		// Per tutte le transizioni entranti
		for (auto &pair: m_incoming_transitions) {
			string label = pair.first;
			set<S*> other_parents = s->m_incoming_transitions[label];

			// Verifico che il numero di padri sia uguale
			if (pair.second.size() != other_parents.size()) {
				return false;
			}

			// Verifico che tutti i padri siano coincidenti
			for (auto &parent: pair.second) {
				// Se il padre non è contenuto nell'altra lista
				if (other_parents.count(parent) <= 0) {
					// TODO Verificare che il controllo sia corretto
					return false;
				}
			}
		}

		return true;
	}

	/**
	 * Restituisce una stringa contenente tutte le informazioni relative allo stato.
	 */
	template <class S>
	string State<S>::toString() const {
		// Stringa che verrà restituita
		string result = "";

		// Inserisco il nome dello stato
		result += getThis()->getName();

		// Se lo stato è final, aggiungo un'etichetta alla stringa visualizzata
		if (getThis()->isFinal()) {
			result += "[FINAL]";
		}
		result += "\n";

		// Per tutte le label delle transizioni uscenti
		for (auto &pair: m_exiting_transitions) {
			string label = pair.first;
			// Per tutti gli stati associati ad una label
			for (S* state: pair.second) {
				// Inserisco le informazioni riguardanti la transizione uscente
				result += "--(" + label + ")--> " + state->getName() + '\n';
			}
		}

		// Per tutte le label delle transizioni entranti
		for (auto &pair: m_incoming_transitions) {
			string label = pair.first;
			// Per tutti gli stati associati ad una label
			for (S* state: pair.second) {
				// Inserisco le informazioni riguardanti la transizione entrante
				result += state->getName() + " --(" + label + ")-->" + '\n';
			}
		}

		return result;
	};

	/**
	 * Definisce un operatore "<" di confronto, basato sul confronto dei nomi.
	 */
	template <class S>
	bool State<S>::operator<(const S &other) const	{
		return getThis()->getName() < other.getName();
	}

	/**
	 * Definisce un operatore di uguaglianza "==", basato sull'uguaglianza dei nomi.
	 */
	template <class S>
	bool State<S>::operator==(const S &other) const {
		return getThis()->getName() == other.getName();
	}

	/**
	 * Definisce un operatore di disuguaglianza "!=", basato sul confronto dei nomi.
	 */
	template <class S>
	bool State<S>::operator!=(const S &other) const {
			return getThis()->getName() != other.getName();
	}

	/// Istanziazione delle classi parametrizzate su StateNFA e StateDFA
    template class State<StateNFA>;
    template class State<StateDFA>;

}
