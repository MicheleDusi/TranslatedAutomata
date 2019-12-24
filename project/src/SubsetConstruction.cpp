/*
 * SubsetConstruction.cpp
 *
 * Project: TranslatedAutomata
 *
 * Implementa l'algoritmo di "Subset Construction" per la generazione di DFA
 * equivalenti ad un NFA di partenza.
 *
 */

#include "SubsetConstruction.hpp"

#include <queue>

#include "Debug.hpp"
#include "State.hpp"

namespace translated_automata {

    /**
     * Esegue l'algoritmo "Subset Construction".
     * Nota: siamo sempre nel caso in cui NON esistono epsilon-transizioni.
     */
	DFA* SubsetConstruction::run(NFA* nfa) {

		// Creo l'automa a stati finiti deterministico, inizialmente vuoto
		DFA* dfa = new DFA();

        // Creo lo stato iniziale per il DFA
		ExtensionDFA initial_dfa_extension;
		StateNFA* nfa_initial_state = nfa->getInitialState();
		DEBUG_ASSERT_NOT_NULL(nfa_initial_state);
		initial_dfa_extension.insert(nfa_initial_state);
		ExtensionDFA epsilon_closure = ConstructedStateDFA::computeEpsilonClosure(initial_dfa_extension);
		ConstructedStateDFA * initial_dfa_state = new ConstructedStateDFA(epsilon_closure);

		// Inserisco lo stato all'interno del DFA
        dfa->addState(initial_dfa_state);

        // Stack per gli stati ancora da processare (bud)
        std::queue<ConstructedStateDFA*> buds_stack;

        // Inserisco come bud di partenza il nodo iniziale
        buds_stack.push(initial_dfa_state);

        // Finché nella queue sono presenti dei bud
        while (! buds_stack.empty()) {

        	// Estraggo il primo elemento della queue
        	ConstructedStateDFA* current_state = buds_stack.front();			// Ottengo un riferimento all'elemento estratto
            buds_stack.pop();								// Rimuovo l'elemento

            // Per tutte le label che marcano transizioni uscenti da questo stato
            for (string l : current_state->getLabelsExitingFromExtension()) {
            	// Salto le epsilon-transizioni
            	if (l == EPSILON) {
            		continue;
            	}

            	// Computo la l-closure dello stato e creo un nuovo stato DFA
            	ExtensionDFA l_closure = current_state->computeLClosureOfExtension(l);
            	ConstructedStateDFA* new_state = new ConstructedStateDFA(l_closure);
            	DEBUG_LOG("Dallo stato %s, con la label %s, ho creato lo stato %s",
            			current_state->getName().c_str(),
						l.c_str(),
						new_state->getName().c_str());

                // Verifico se lo stato DFA creato è vuoto
                if (new_state->isExtensionEmpty()) {
                	// Se sì, lo elimino e procedo
                	DEBUG_LOG("Estensione vuota, salto l'iterazione");
                    delete new_state;
                    continue;
                }
                // Verifico se lo stato DFA creato è già presente nel DFA (come nome)
                else if (dfa->hasState(new_state->getName())) {
                	// Se sì, lo stato estratto dalla queue può essere eliminato
                	DEBUG_LOG("Lo stato è già presente, lo elimino e recupero quello vecchio");
                	ConstructedStateDFA* tmp_state = new_state;
                    new_state = (ConstructedStateDFA*) dfa->getState(tmp_state->getName());
                    delete tmp_state;
                }
                // Se si tratta di uno stato "nuovo"
                else {
                	// Lo aggiungo al DFA e alla queue
                	DEBUG_LOG("Lo stato è nuovo, lo aggiungo all'automa");
                    dfa->addState(new_state);
                    buds_stack.push(new_state);
                }

                // Effettuo la connessione:
                //	state--(l)-->new_state
                current_state->connectChild(l, new_state);
            }
        }

        // Imposto lo stato iniziale
        // Questa operazione sistema le distanze in automatico
        dfa->setInitialState(initial_dfa_state);

        return dfa;
	}
}
