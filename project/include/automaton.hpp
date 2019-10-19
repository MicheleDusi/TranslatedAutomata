/******************************************************************************
 * automaton.hpp
 *
 * Project: TranslatedAutomata
 *
 * Header del file automaton.cpp. Fornisce le firme della classe "Automaton",
 * che rappresenta in astratto un Automa a stati finiti.
 * Fornisce inoltre i due alias NFA e DFA.
 *
 ******************************************************************************/

#ifndef INCLUDE_AUTOMATON_H_
#define INCLUDE_AUTOMATON_H_

#include <vector>
#include <list>

#include "state.hpp"

namespace translated_automata {

	using std::vector;
	using std::list;

	template <class State>
	class Automaton {

	private:
		map<string, State*> m_states;
		State *m_initial_state;

        void removeReachableStates(State* s, set<State*> &states);

	public:
		Automaton();
		virtual ~Automaton();

        int size();
        void addState(State* s);
        bool deleteState(string name);
        bool deleteEmptyState();
        bool removeState(State* s);
        bool removeState(string name);
        set<State*> removeUnreachableStates();
        bool hasState(State* s);
        bool hasState(string name);
        bool isInitial(State* s);
        bool isInitial(string name);
        void setInitialState(State* s);
        void setInitialState(string name);
        State* getInitialState();
        State* getState(State* s);
        State* getState(string name);
        const list<State*> getStatesList();
        const vector<State*> getStatesVector();
        const set<string>& getLabels();
        bool connectStates(State *from, State *to, string label);
        bool connectStates(string from, string to, string label);
        void print();
        Automaton<State>* clone();
        bool operator==(Automaton<State>& other);

	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATON_H_ */
