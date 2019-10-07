/*
 * state_nfa.cpp
 *
 */

#include "state_nfa.hpp"

#include <string>
#include "state.hpp"


namespace translated_automata {

	/**
	 * Costruttore.
	 * @param name Il nome dello statoNFA
	 * @param final se lo stato è finale o no.
	 */
	StateNFA::StateNFA (string name, bool final) : State<StateNFA>() {
		m_final = final;
		m_name = name;
	}

	StateNFA::~StateNFA () {
		#if defined DEBUG_PRINTS
			std::cout << "DELETING " << getThis()->getName() << " (" << this << ")" << std::endl;
		#endif
	}

	bool StateNFA::isEmpty() const {
		// DEFAULT
		return true;
	}

	bool StateNFA::isFinal() const {
		// DEFAULT
		return false;
	}

	/**
	 * Restituisce un duplicato dello stato.
	 */
	StateNFA* StateNFA::duplicate() const {
        return new StateNFA(m_name, m_final);
	}


}
