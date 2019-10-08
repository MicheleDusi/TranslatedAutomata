/******************************************************************************
 * state_nfa_test.cpp
 *
 * Author: Michele Dusi
 * Project: TranslatedAutomata
 *
 * TEST per il file sorgente "state_nfa.cpp".
 *
 ******************************************************************************/

#include "state_nfa_test.hpp"

using namespace translated_automata;

void test_stateNFA() {
	DEBUG_MARK_PHASE( test NFA ) {

		StateNFA* state0 = new StateNFA("S0", false);
		StateNFA* state1 = new StateNFA("S1", false);
		StateNFA* state2 = new StateNFA("S2", true);

		DEBUG_ASSERT_NOT_NULL( state0 );
		DEBUG_ASSERT_NOT_NULL( state1 );
		DEBUG_ASSERT_NOT_NULL( state2 );

		// Qualunque stringa con almeno due "a" consecutive
		//
		//  (a|b)*(aa)(a|b)*
		state0->connectChild("a", state0);
		state0->connectChild("a", state1);
		state0->connectChild("b", state0);

		state1->connectChild("a", state2);

		state2->connectChild("a", state2);
		state2->connectChild("b", state2);

		set<State*> children0a = state0->getChildren("a");
		DEBUG_ASSERT_TRUE( children0a.count(state0) > 0 );
		DEBUG_ASSERT_TRUE( children0a.count(state1) > 0 );
		DEBUG_ASSERT_FALSE( children0a.count(state2) > 0 );

		set<State*> children0b = state0->getChildren("b");
		DEBUG_ASSERT_TRUE( children0b.count(state0) > 0 );
		DEBUG_ASSERT_FALSE( children0b.count(state1) > 0 );
		DEBUG_ASSERT_FALSE( children0b.count(state2) > 0 );

		int ex_trans0 = state0->getExitingTransitionsCount();
		DEBUG_ASSERT_TRUE( ex_trans0 == 3 );
		int in_trans0 = state0->getIncomingTransitionsCount();
		DEBUG_ASSERT_TRUE( in_trans0 == 2 );

		int ex_trans1 = state1->getExitingTransitionsCount();
		DEBUG_ASSERT_TRUE( ex_trans1 == 1 );
		int in_trans1 = state1->getIncomingTransitionsCount();
		DEBUG_ASSERT_TRUE( in_trans1 == 1 );

	}

}

