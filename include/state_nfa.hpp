/*
 * stat_nfa.h
 *
 */

#ifndef INCLUDE_STATE_NFA_H_
#define INCLUDE_STATE_NFA_H_

#include "state.hpp"

namespace translated_automata
{

	/**
	 * Classe StateNFA.
	 * Eredita in maniera pubblica da State<StateNFA>.
	 */
    class StateNFA : public State<StateNFA>
    {

	private:
		bool m_final = false;

    public:
        struct Comparator {
            bool operator() (const StateNFA* lhs, const StateNFA* rhs) const {
                return lhs->getName() < rhs->getName();
            }
        };


        StateNFA(string name, bool final = false);
		~StateNFA();

		bool isEmpty() const;
        bool isFinal() const;
        StateNFA* duplicate() const;

    };
}

#endif /* INCLUDE_STATE_NFA_H_ */
