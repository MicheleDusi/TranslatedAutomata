/******************************************************************************
 * automaton.hpp
 *
 * Project: TranslatedAutomata
 *
 * Header del file automaton.cpp. Fornisce le firme della classe "Automaton",
 * che rappresenta in astratto un Automa a stati finiti.
 *
 ******************************************************************************/

#ifndef INCLUDE_AUTOMATON_H_
#define INCLUDE_AUTOMATON_H_

#include "state.hpp"

namespace translated_automata {

	template <class State>
	class Automaton {

	private:
		map<string, State*> m_states;
		State *m_initial_state;

        void removeReachableStates(State* s, set<State*> &states);

	public:
		Automaton();
		virtual ~Automaton();

        void addState(State* s);
        bool removeState(State* s);
        bool removeState(string name);
        bool deleteState(string name);
        void setInitialState(State* s);
        void setInitialState(string name);
        bool isInitial(State* s);
        bool isInitial(string name);
        State* getInitialState();
        bool hasState(State* s);
        bool hasState(string name);
        const map<string, State*>& getStates();
        State* getState(State* s);
        State* getState(string name);
        void deleteEmptyState();
        set<State*> removeUnreachableStates();
        void print();
        Automaton<State>* clone();
        bool operator==(Automaton<State>& other);
        set<string> getLabels();
        int size();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATON_H_ */
