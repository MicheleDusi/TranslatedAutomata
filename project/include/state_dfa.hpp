/******************************************************************************
 * state_nfa.hpp
 *
 * Project: TranslatedAutomata
 *
 * Header contenente la definizione e le firme dei metodi della classe
 * concreta "StateDFA", figlia della classe astratta "State".
 * Concettualmente, un oggetto "StateDFA" rappresenta uno stato di un DFA, ossia
 * di un automa a stati finiti deterministico.
 * Un oggetto StateDFA ha la caratteristica di avere un solo nodo figlio per
 * ciascuna (eventuale) label. Questo implica che non avrà mai più di N transizioni
 * uscenti, dove N è la cardinalità dell'alfabeto.
 * Nel nostro problema, uno StateDFA è fortemente legato al concetto di "Estensione":
 * l'estensione di uno stato DFA è un'insieme di stati NFA a cui lo stato corrisponde,
 * secondo un processo di Subset Construction che traduce l'NFA in DFA.
 *
 ******************************************************************************/

#ifndef INCLUDE_STATE_DFA_H_
#define INCLUDE_STATE_DFA_H_

#include "state.hpp"

namespace translated_automata {

	/**
	 * Concrete class "StateDFA".
	 * Eredita pubblicamente da State parametrizzata sulla
	 * stessa classe StateDFA.
	 */
    class StateDFA : public State<StateDFA> {

    public:
		StateDFA(string name, bool final = false);
		~StateDFA();

		StateDFA* getChild(string label);

    };

}

#endif /* INCLUDE_STATE_DFA_H_ */
