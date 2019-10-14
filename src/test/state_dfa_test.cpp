/******************************************************************************
 * state_dfa_test.cpp
 *
 * Project: TranslatedAutomata
 *
 * TEST per il file sorgente "state_dfa.cpp".
 *
 ******************************************************************************/

#include "state_dfa_test.hpp"

using namespace translated_automata;

void test_constructor();

void test_stateDFA() {
	DEBUG_MARK_PHASE( test DFA ) {

		test_constructor();

	}
}

void test_constructor() {
	DEBUG_MARK_PHASE( constructor test ) {

		StateNFA* nstate0 = new StateNFA("n0", false);
		StateNFA* nstate1 = new StateNFA("n1", false);
		nstate0->connectChild("label", nstate1);

		ExtensionDFA* ext0 = new ExtensionDFA();
		ext0->insert(nstate0);
		StateDFA* dstate0 = new StateDFA(ext0);

		ExtensionDFA* ext1 = new ExtensionDFA();
		ext1->insert(nstate1);
		StateDFA* dstate1 = new StateDFA(ext1);

		DEBUG_ASSERT_NOT_NULL( dstate0 );
		DEBUG_ASSERT_NOT_NULL( dstate1 );

		set<string> labels0 = dstate0->getExitingLabels();
		DEBUG_ASSERT_TRUE( labels0.count("label") > 0 );

	}
}

