/******************************************************************************
 * state.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header per il sorgente "state.cpp".
 * Contiene la definizione della classe astratta "State", che viene implementata
 * dalle classe "StateNFA" e "StateDFA".
 * Nel dominio concettuale, uno "State" è uno stato di un "automa a stati finiti",
 * sia esso deterministico o non deterministico. E' caratterizzato da un nome
 * univoco ed è legato ad altri stati mediante transizioni, ciascuna delle quali
 * è marcata da una label.
 * La classe "State" prevede, quando estesa, l'implementazione dei metodi virtuali
 * "isEmpty", "isFinal" e "duplicate".
 *
 ******************************************************************************/

#ifndef INCLUDE_STATE_H_
#define INCLUDE_STATE_H_

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <cstdbool>

using std::string;
using std::map;
using std::set;

namespace translated_automata {

	/**
	 * Abstract class "State".
	 */
	class State {

    private:
        map<string, set<State*>> m_exiting_transitions;		// Transizioni uscenti dallo stato
        map<string, set<State*>> m_incoming_transitions;	// Transizioni entranti nello stato

        State* getThis() const;
        void removeChild(string label, State* child);
        void removeParent(string label, State* parent);

	protected:
		string m_name = "";									// Nome dello stato

    public:
		State();											// Costruttore
        virtual ~State();									// Distruttore (virtuale)
        string getName() const;								// Restituisce il nome

		virtual bool isEmpty() const = 0;
		virtual bool isFinal() const = 0;
		virtual State* duplicate() const = 0;

		void connectChild(string label, State* child);
		void disconnectChild(string label, State* child);
		void detach();
		set<State*> getChildren(string label);
		State* getChild(string label);
		set<State*> getParents(string label);
		bool hasExitingTransition(string label);
		bool hasExitingTransition(string label, State* child);
		map<string, set<State*>> getExitingTransitions();
		map<string, set<State*>> getIncomingTransitions();
		const map<string, set<State*>>& getExitingTransitionsRef();
		const map<string, set<State*>>& getIncomingTransitionsRef();
		int getExitingTransitionsCount();
		int getIncomingTransitionsCount();
		string toString() const;

		bool operator<(const State &other) const;
		bool operator==(const State &other) const;
		bool operator!=(const State &other) const;

    };

}

#endif /* INCLUDE_STATE_H_ */
