/******************************************************************************
 * state_nfa.cpp
 *
 * Project: TranslatedAutomata
 *
 * Sorgente contenente l'implementazione della classe concreta "StateNFA", figlia
 * della classe astratta "State".
 * Concettualmente, un oggetto "StateNFA" rappresenta uno stato di un NFA, ossia
 * di un automa a stati finiti non deterministico.
 * Un oggetto StateNFA è caratterizzato dalla possibilità di avere più transizioni
 * uscenti marcate con la stessa label, anche dirette a stati differenti.
 *
 ******************************************************************************/

#include "state_nfa.hpp"

#include <string>

#include "debug.hpp"

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	StateNFA::StateNFA (string name, bool final) : State() {
		m_final = final;
		m_name = name;
	}

	/**
	 * Distruttore.
	 */
	StateNFA::~StateNFA () {
		DEBUG_LOG( "Cancellazione dello stato \"%s\"", m_name.c_str() );
	}

}



