/*
 * state.cpp
 *
 */

#include "state.hpp"

#include <string>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

using std::string;
using std::vector;
using std::map;
using std::set;

namespace translated_automata {

	/**
	 * Classe astratta "State".
	 * Viene implementata da StateNFA e StateDFA.
	 */

	/**
	 * Metodo privato che restituisce il puntatore all'oggetto corrente.
	 */
	State* State::getThis() {
		return this;
	}

//	/**
//	 * Costruttore della classe State, generica sul tipo "S".
//	 * Inizializza come vuoti gli insiemi di transizioni entranti e uscenti.
//	 */
//	template <class S>
//	State<S>::State ()
//	: m_transitions(), m_incoming_transitions() {}
//
//	/**
//	 * Distruttore della classe State.
//	 */
//	template <class S>
//	State<S>::~State () {
//		#if defined DEBUG_PRINTS
//			std::cout << "DELETING " << getThis()->getName() << " (" << this << ")" << std::endl;
//		#endif
//		detach();
//	}
//
//	/**
//	 * Restituisce il nome dello stato.
//	 */
//	template <class S>
//	string State<S>::getName() const {
//		return m_name;
//	}
//
//	/**
//	 * Collega lo stato soggetto allo stato passato come parametro, con una transizione
//	 * etichettata dalla label "label" passata come parametro.
//	 */
//	template <class S>
//	void State<S>::connect(string label, S* child)
//	{
//		// Aggiungo una transizione uscente da questo stato
//		m_transitions[label].insert(child);
//		// Aggiungo una transizione entrante allo stato di arrivo
//		child->m_incoming_transitions[label].insert(getThis());
//	}
//
//	/**
//	 * Disconnette due stati, rimuovendo la l-transizione che parte da questo
//	 * stato e arriva in quello passato come parametro.
//	 *
//	 * Precondizione: si suppone che tale transizione sia esistente
//	 */
//	template <class S>
//	void State<S>::disconnect(string label, S* child)
//	{
//		removeChild(label, child);
//		child->removeParent(label, getThis());
//	}
//
//	/**
//	 * Stacca tutte le transizioni entranti e uscenti.
//	 */
//	template <class S>
//	void State<S>::detach()
//	{
//		// remove pointers to this state from children
//		for (auto pair: m_transitions) {
//			string label = pair.first;
//			for (S* child: pair.second) {
//				child->removeParent(label, getThis());
//			}
//		}
//
//		// remove pointers to this state from parents
//		for (auto pair: m_incoming_transitions) {
//			string label = pair.first;
//			for (S* parent: pair.second) {
//				parent->removeChild(label, getThis());
//			}
//		}
//	}
//
//	/**
//	 * Rimuove una transizione dallo stato al figlio.
//	 * Se non esiste non succede nulla.
//	 *
//	 * Privato
//	 */
//	template <class S>
//	void State<S>::removeChild(string label, S* child)
//	{
//		m_transitions[label].erase(child);
//		if (m_transitions[label].empty()) {
//			m_transitions.erase(label);
//		}
//	}
//
//	/**
//	 * Come "removeChild", ma rimuovo una transizione entrante.
//	 *
//	 * Privato
//	 */
//	template <class S>
//	void State<S>::removeParent(string label, S* parent)
//	{
//		m_incoming_transitions[label].erase(parent);
//		if (m_incoming_transitions[label].empty()) {
//			m_incoming_transitions.erase(label);
//		}
//	}
//
//	/**
//	 * Restituisce l'insieme di stati raggiunti da tutte le transizioni
//	 * uscenti da questo nodo marcate con la label passata come parametro.
//	 * In pratica, restituisce la label-closure.
//	 */
//	template <class S>
//	set<S*> State<S>::getChildren(string label)
//	{
//		auto search = m_transitions.find(label);
//		// Con "auto" sto esplicitando il processo di type-inference
//		if (search != m_transitions.end()) {
//			return m_transitions[label];
//		} else {
//			return set<S*>();
//		}
//	}
//
//	/**
//	 * Restituisce lo stato raggiunto da una transizione con una specifica etichetta.
//	 * In caso ne sia presente più di uno (caso NFA), restituisce solo il primo della lista.
//	 * Per ottenere tutti gli stati raggiunti, è opportuno chiamare il metodo "getChildren".
//	 */
//	template <class S>
//	S* State<S>::getChild(string label)
//	{
//		auto search = m_transitions.find(label);
//		// Con "auto" sto esplicitando il processo di type-inference
//		if (search != m_transitions.end()) {
//			return *(m_transitions[label].begin());
//		}	else {
//			return NULL;
//		}
//	}
//
//	/**
//	 * Restituisce gli stati che hanno un transizione marcata da una specifica label
//	 * che punta a questo stato.
//	 */
//	template <class S>
//	set<S*> State<S>::getParents(string label)
//	{
//		auto search = m_incoming_transitions.find(label);
//		if (search != m_incoming_transitions.end()) {
//			return m_incoming_transitions[label];
//		} else {
//			return set<S*>();
//		}
//	}
//
//	/**
//	 * Verifica se il nodo soggetto ha una transizione uscente
//	 * marcata con la label passata come parametro.
//	 */
//	template <class S>
//	bool State<S>::hasTransition(string label)
//	{
//		auto search = m_transitions.find(label);
//
//		#if defined DEBUG_PRINTS
//		if (search != m_transitions.end()) {
//			if (!m_transitions[label].size()) {
//				std::cout << "ERR> found nonexisting transition\n";
//			}
//		}
//		#endif
//
//		return search != m_transitions.end();
//	}
//
//	/**
//	 * Verifica se il nodo soggetto ha una transizione al nodo "child" marcata con l'etichetta "label".
//	 */
//	template <class S>
//	bool State<S>::hasTransition(string label, S* child)
//	{
//		auto search = m_transitions.find(label);
//		if (search != m_transitions.end()) {
//			return search->second.count(child);
//		} else {
//			return false;
//		}
//	}
//
//	/**
//	 * Restituisce la mappa di transizioni uscenti da questo stato.
//	 */
//	template <class S>
//	map<string, set<S*>> State<S>::getTransitions()
//	{
//		return m_transitions;
//	}
//
//	/**
//	 * Restituisce una reference alla mappa di transizioni, ossia l'indirizzo della memoria
//	 * in cui la mappa è salvata.
//	 * Restituire un indirizzo permette di usare questo metodo come lvalue in un assegnamento, ad esempio.
//	 */
//	template <class S>
//	const map<string, set<S*>>& State<S>::getTransitionsRef()
//	{
//		return m_transitions;
//	}
//
//	/**
//	 * Restituisce la mappa di transizioni entranti in questo stato.
//	 */
//	template <class S>
//	map<string, set<S*>> State<S>::getIncomingTransitions()
//	{
//		return m_incoming_transitions;
//	}
//
//	/**
//	 * Restituisce una reference alla mappa di transizioni entranti, ossia l'indirizzo della memoria
//	 * in cui la mappa è salvata.
//	 * Restituire un indirizzo permette di usare questo metodo come lvalue in un assegnamento, ad esempio.
//	 */
//	template <class S>
//	const map<string, set<S*>>& State<S>::getIncomingTransitionsRef()
//	{
//		return m_incoming_transitions;
//	}
//
//	/**
//	 * Conta le transizioni uscenti dallo stato.
//	 * Per ciascuna label, conteggia la quantità di transizioni riferite a tale label uscenti
//	 * dal nodo corrente.
//	 */
//	template <class S>
//	int State<S>::transitionsCount()
//	{
//		int count = 0;
//		for (auto &pair: m_transitions) {// Ciclo su tutti gli elementi della mappa
//			count += pair.second.size();
//		}
//		// "second" prende il secondo elemento della coppia,
//		// in questo caso l'insieme di nodi raggiunti dalle transizioni con label predefinita
//		return count;
//	}
//
//	/**
//	 * Conta le transizioni entranti nello stato.
//	 */
//	template <class S>
//	int State<S>::incomingTransitionsCount()
//	{
//		int count = 0;
//		for (auto &pair: m_incoming_transitions) {
//			count += pair.second.size();
//		}
//		return count;
//	}
//
//	/**
//	 * Restituisce una stringa contenente tutte le informazioni relative allo stato.
//	 */
//	template <class S>
//	string State<S>::toString() const
//	{
//		// Stringa che verrà restituita
//		string result = "";
//
//		if (getThis()->isFinal()) {
//			result += "[" + getThis()->getName() + "]:\n";
//			std::cout << '[' << getThis()->getName() << "]:\n";
//		} else {
//			std::cout << getThis()->getName() << ":\n";
//		}
//
//		for (auto &pair: m_transitions) {
//			string label = pair.first;
//			for (S* state: pair.second) {
//				std::cout << label << "-> " << state->getName() << '\n';
//			}
//		}
//
//		for (auto &pair: m_incoming_transitions) {
//			string label = pair.first;
//			for (S* state: pair.second) {
//				std::cout << state->getName() << " " << label << "->" << '\n';
//			}
//		}
//
//		std::cout << '\n';
//		return result;
//	};
//
//	/**
//	 * Definisce un operatore "<" di confronto, basato sul confronto dei nomi.
//	 */
//	template <class S>
//	bool State<S>::operator<(const S &other) const
//	{
//		return getThis()->getName() < other.getName();
//	}
//
//	/**
//	 * Definisce un operatore di uguaglianza "==", basato sull'uguaglianza dei nomi.
//	 */
//	template <class S>
//	bool State<S>::operator==(const S &other) const
//	{
//		return getThis()->getName() == other.getName();
//	}
//
//	/**
//	 * Definisce un operatore di disuguaglianza "!=", basato sul confronto dei nomi.
//	 */
//	template <class S>
//	bool State<S>::operator!=(const S &other) const
//	{
//			return getThis()->getName() != other.getName();
//	}




}
