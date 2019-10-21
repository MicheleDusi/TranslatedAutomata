/******************************************************************************
 * subset_construction.cpp
 *
 * Project: TranslatedAutomata
 *
 * Implementa l'algoritmo di "Subset Construction" per la generazione di DFA
 * equivalenti ad un NFA di partenza.
 *
 ******************************************************************************/

#include "subset_construction.hpp"

#include <queue>

namespace translated_automata {

    /**
     * Esegue l'algoritmo "Subset Construction".
     * Nota: siamo sempre nel caso in cui NON esistono epsilon-transizioni.
     */
	DFA * SubsetConstruction::run(NFA * nfa) {

		// Creo l'automa a stati finiti deterministico, inizialmente vuoto
		DFA *dfa = new DFA();

        // Creo lo stato iniziale per il DFA
		ExtensionDFA initial_dfa_extension;
		initial_dfa_extension.insert(nfa->getInitialState());
		StateDFA * initial_dfa_state = new StateDFA(initial_dfa_extension);
		// Inserisco lo stato all'interno del DFA
        dfa->addState(initial_dfa_state);
        dfa->setInitialState(initial_dfa_state);

        // Stack per i BUD
        std::queue<StateDFA*> bud_stack;

        // Inserisco come bud di partenza il nodo iniziale
        bud_stack.push(initial_dfa_state);

        // Finché nella queue sono presenti dei bud
        while (! bud_stack.empty()) {

        	// Estraggo il primo elemento della queue
            StateDFA* state = bud_stack.front();			// Ottengo un riferimento all'elemento estratto
            bud_stack.pop();								// Rimuovo l'elemento

            // Per tutte le label che marcano transizioni uscenti da questo stato
            for (string l: state->getExitingLabels()) {

            	// Computo la l-closure dello stato e creo un nuovo stato DFA
                StateDFA* new_state = new StateDFA(state->lClosure(l));

                // Verifico se lo stato DFA creato è vuoto
                if (new_state->isEmpty()) {
                	// Se sì, lo elimino e procedo
                    delete new_state;
                    continue;
                }
                // Verifico se lo stato DFA creato è già presente nel DFA
                else if (dfa->hasState(new_state)) {
                	// Se sì, lo stato estratto dalla queue può essere eliminato
                    StateDFA* tmp_state = new_state;
                    new_state = dfa->getState(tmp_state->getName());
                    delete tmp_state;
                }
                // Se si tratta di uno stato "nuovo"
                else {
                	// Lo aggiungo al DFA e alla queue
                    dfa->addState(new_state);
                    bud_stack.push(new_state);
                }

                // Effettuo la connessione:
                //	state--(l)-->new_state
                state->connectChild(l, new_state);
            }
        }

        return dfa;
	}
}
