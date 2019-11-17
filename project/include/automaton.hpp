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
		set<State*> m_states;
		State *m_initial_state;

        void removeReachableStates(State* s, set<State*> &states); // INUTILE

	public:
		Automaton();
		virtual ~Automaton();

        int size();
        void addState(State* s);
        bool removeState(State* s);
        set<State*> removeUnreachableStates(); // INUTILE
        bool hasState(State* s);
        bool hasState(string name);
        bool isInitial(State* s);
        bool isInitial(string name);
        void setInitialState(State* s);
        void setInitialState(string name);
        State* getInitialState();
        State* getState(string name);
        const vector<State*> getStatesByName(string name);
        const list<State*> getStatesList();
        const vector<State*> getStatesVector();
        const set<string>& getLabels();
        bool connectStates(State *from, State *to, string label);
        bool connectStates(string from, string to, string label);
        string toString();

//        bool operator==(Automaton<State>& other);

	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATON_H_ */
