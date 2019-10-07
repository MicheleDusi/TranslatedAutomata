/*
 * state.h
 *
 */

#ifndef INCLUDE_STATE_H_
#define INCLUDE_STATE_H_

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>

using std::string;
using std::vector;
using std::map;
using std::set;

namespace translated_automata {

	/**
	 * Abstract class "State"
	 */
	class State {

    private:
		/** Transizioni uscenti dallo stato */
        map<string, set<State*>> m_exiting_transitions;
        /** Transizioni entranti nello stato */
        map<string, set<State*>> m_incoming_transitions;

        State* getThis();
        bool removeChild(string label, State* child);
        bool removeParent(string label, State* parent);

	protected:
		string m_name = "";				// Nome dello stato

    public:
		State();						// Costruttore
        ~State();						// Distruttore
        string getName() const;			// Restituisce il nome
    };


	/* Metodi che sono stati commentati: */
	//	/** Metodi astratti */
	//    	virtual bool isEmpty() const = 0;
	//    	virtual bool isFinal() const = 0;
	//    	virtual State* duplicate() const = 0;
	//		void connect(string transition, S* child);
	//        void disconnect(string transition, S* child);
	//        void detach();
	//        set<S*> getChildren(string label);
	//        S* getChild(string label);
	//        set<S*> getParents(string label);
	//        bool hasTransition(string label);
	//        bool hasTransition(string label, S* child);
	//        map<string, set<S*>> getTransitions();
	//        map<string, set<S*>> getIncomingTransitions();
	//        const map<string, set<S*>>& getTransitionsRef();
	//        const map<string, set<S*>>& getIncomingTransitionsRef();
	//        int transitionsCount();
	//        int incomingTransitionsCount();
	//        string toString() const;
	//        bool operator<(const S &other) const;
	//        bool operator==(const S &other) const;
	//        bool operator!=(const S &other) const;


//	// TODO Questi non dovrebbero stare qui: un'astrazione non dovrebbe avere riferimenti alle sue classi concrete.
//	// TUTTAVIA togliendoli il progetto non viene compilato.
//	// Più nello specifico, i file obj compilati singolarmente non riescono ad essere linkati insieme
//	// poiché in questo manca la compilazione esplicita della classe State parametrizzata sul tipo StateNFA.
//	// Il file infatti è compilato su "State<S>", ma la compilazione di "State<StateNFA>" deve essere forzata in questo modo,
//	// altrimenti le funzioni parametriche sul tipo S della classe State<S> NON andranno bene quando userò la classe State<StateNFA>.
//
//	template class State<StateNFA>;
//	template class State<StateDFA>;

}

#endif /* INCLUDE_STATE_H_ */
