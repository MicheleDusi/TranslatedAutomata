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

#include "state_dfa.hpp"
#include "state_nfa.hpp"
#include "alphabet.hpp"

namespace translated_automata {

	using std::vector;
	using std::list;
	using std::multiset;

	template <class State>
	class Automaton {

	private:
		multiset<State*> m_states;
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
        const Alphabet getAlphabet();
        bool connectStates(State *from, State *to, string label);
        bool connectStates(string from, string to, string label);

        bool operator==(Automaton<State>& other);

	};

	/* Classi specifiche per Automi */

	class DFA : public Automaton<StateDFA> {};
	class NFA : public Automaton<StateNFA> {};


} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATON_H_ */
