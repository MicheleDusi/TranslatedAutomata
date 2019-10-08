/******************************************************************************
 * state_nfa.hpp
 *
 * Project: TranslatedAutomata
 *
 * Header contenente la definizione e le firme dei metodi della classe
 * concreta "StateNFA", figlia della classe astratta "State".
 * Concettualmente, un oggetto "StateNFA" rappresenta uno stato di un NFA, ossia
 * di un automa a stati finiti non deterministico.
 * Un oggetto StateNFA è caratterizzato dalla possibilità di avere più transizioni
 * uscenti marcate con la stessa label, anche dirette a stati differenti.
 *
 ******************************************************************************/

#ifndef INCLUDE_STATE_NFA_HPP_
#define INCLUDE_STATE_NFA_HPP_

#include "state.hpp"

namespace translated_automata {

	/**
	 * Classe StateNFA.
	 * Eredita in maniera pubblica da State.
	 */
    class StateNFA : public State {

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

		bool isEmpty() const; // Al momento non serve, era per l'intensione
        bool isFinal() const;
        void setFinal(bool final);
        StateNFA* duplicate() const;
    };

}

#endif /* INCLUDE_STATE_NFA_HPP_ */
