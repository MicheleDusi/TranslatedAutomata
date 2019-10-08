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
 *
 ******************************************************************************/
