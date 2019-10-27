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

#include "constructed_state_dfa.hpp"

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
		ConstructedStateDFA * initial_dfa_state = new ConstructedStateDFA(initial_dfa_extension);
		// Inserisco lo stato all'interno del DFA
        dfa->addState(initial_dfa_state);
        dfa->setInitialState(initial_dfa_state);

        // Stack per i BUD
        std::queue<ConstructedStateDFA*> buds_stack;

        // Inserisco come bud di partenza il nodo iniziale
        buds_stack.push(initial_dfa_state);

        // Finché nella queue sono presenti dei bud
        while (! buds_stack.empty()) {

        	// Estraggo il primo elemento della queue
        	ConstructedStateDFA* state = buds_stack.front();			// Ottengo un riferimento all'elemento estratto
            buds_stack.pop();								// Rimuovo l'elemento

            // Per tutte le label che marcano transizioni uscenti da questo stato
            for (string l: state->getLabelsExitingFromExtension()) {

            	// Computo la l-closure dello stato e creo un nuovo stato DFA
            	ExtensionDFA l_closure = state->computeLClosureOfExtension(l);
            	ConstructedStateDFA* new_state = new ConstructedStateDFA(l_closure);

                // Verifico se lo stato DFA creato è vuoto
                if (new_state->isExtensionEmpty()) {
                	// Se sì, lo elimino e procedo
                    delete new_state;
                    continue;
                }
                // Verifico se lo stato DFA creato è già presente nel DFA
                else if (dfa->hasState(new_state)) {
                	// Se sì, lo stato estratto dalla queue può essere eliminato
                	ConstructedStateDFA* tmp_state = new_state;
                    new_state = (ConstructedStateDFA*) dfa->getState(tmp_state->getName());
                    delete tmp_state;
                }
                // Se si tratta di uno stato "nuovo"
                else {
                	// Lo aggiungo al DFA e alla queue
                    dfa->addState(new_state);
                    buds_stack.push(new_state);
                }

                // Effettuo la connessione:
                //	state--(l)-->new_state
                state->connectChild(l, new_state);
            }
        }

        return dfa;
	}
}
