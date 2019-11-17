/******************************************************************************
 * state.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header per il sorgente "state.cpp".
 * Contiene la definizione della classe astratta "State", che viene implementata
 * dalle classe "StateNFA" e "StateDFA" ed è parametrizzata sul tipo "S".
 * Nel dominio concettuale, uno "State" è uno stato di un "automa a stati finiti",
 * sia esso deterministico o non deterministico. E' caratterizzato da un nome
 * univoco ed è legato ad altri stati mediante transizioni, ciascuna delle quali
 * è marcata da una label.
 * La classe "State" prevede, quando estesa, l'implementazione dei metodi virtuali
 * "isFinal" e "duplicate".
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

#define DEFAULT_VOID_DISTANCE 1U<<30

namespace translated_automata {

	/**
	 * Abstract class "State".
	 */
	template <class S>
	class State {

    private:
        map<string, set<S*>> m_exiting_transitions;		// Transizioni uscenti dallo stato
        map<string, set<S*>> m_incoming_transitions;	// Transizioni entranti nello stato

        S* getThis() const;
        void removeChild(string label, S* child);
        void removeParent(string label, S* parent);

	protected:
		string m_name = "";									// Nome dello stato
		bool m_final = false;								// Flag che indica se lo stato è finale o meno
		unsigned int m_distance = DEFAULT_VOID_DISTANCE;	// Distanza del nodo dal nodo iniziale

    public:
		State();											// Costruttore
        virtual ~State();									// Distruttore (virtuale)

        string getName() const;
        bool isFinal();
        void setFinal(bool final);
		void connectChild(string label, S* child);
		void disconnectChild(string label, S* child);
		void detachAllTransitions();
		set<S*> getChildren(string label);
		set<S*> getParents(string label);
		bool hasExitingTransition(string label);
		bool hasExitingTransition(string label, S* child);
		bool hasIncomingTransition(string label);
		bool hasIncomingTransition(string label, S* child);
		map<string, set<S*>> getExitingTransitions();
		map<string, set<S*>> getIncomingTransitions();
		const map<string, set<S*>>& getExitingTransitionsRef();
		const map<string, set<S*>>& getIncomingTransitionsRef();
		int getExitingTransitionsCount();
		int getIncomingTransitionsCount();
		void copyExitingTransitionsOf(S* state);
		void copyIncomingTransitionsOf(S* state);
		void copyAllTransitionsOf(S* state);
		bool hasSameTransitions(S* otherState);
		unsigned int getDistance();
		void setDistance(unsigned int distance);
		void initDistancesRecursively(int root_distance);
	    int getMinimumParentsDistance();
		string toString() const;

//		bool operator<(const S &other) const;
//		bool operator==(const S &other) const;
//		bool operator!=(const S &other) const;
		int compareNames(const S &other) const;

        struct Comparator {
            bool operator() (const S* lhs, const S* rhs) const {
                return lhs->getName() < rhs->getName();
            }
        };

    };

}

#endif /* INCLUDE_STATE_H_ */
