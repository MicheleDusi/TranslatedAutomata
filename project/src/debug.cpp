/******************************************************************************
 * debug.hpp
 *
 * Project: TranslatedAutomata
 *
 * Semplice libreria che fornisce una comoda implementazione
 * delle più comuni funzioni di debug sotto forma di MACRO.
 *
 * Per attivare la modalità di debug -> agire sulla macro
 * DEBUG_MODE facendo in modo che sia definita (si veda sotto).
 * Per disattivare la modalità di debug -> agire sulla macro
 * DEBUG_MODE commentando la riga della sua definizione.
 *
 ******************************************************************************/

#include "debug.hpp"

#include <list>

using std::list;

namespace translated_automata {

	list<int> tickets_stack = list<int>();
	int number_of_elements = 0;

	/**
	 * Funzione utilizzata per la marcatura delle fasi nella libreria di debug.
	 * Permette la computazione della lista di numeri relativi alle varie fasi di
	 * cui si sta tenendo traccia.
	 */
	string debugComputeTicketsList() {
		string result = std::to_string(*tickets_stack.begin());

		auto cursor = ++tickets_stack.begin();
		for (int i = 1; i < number_of_elements; i++) {
			result += "." + std::to_string(*cursor);
			cursor++;
		}

		return result;
	}

	/**
	 * Funzione utilizzata per la marcatura delle fasi nella libreria di debug.
	 * Viene chiamata nel momento in cui si entra in una fase; ha due funzionalità principali:
	 * - Modifica una variabile globale (stack) scrivendo al suo interno che si è entrati in una
	 * 	 nuova fase; in altre parole, segna che si è "scesi di un livello".
	 * - Restituisce una stringa che contiene la traccia di tutte le fasi attraversate finora.
	 */
	string debugAcquireTicket() {

		// Aggiungo un elemento alla catena
		number_of_elements++;

		// Se supero la dimensione dello stack
		if (number_of_elements > tickets_stack.size()) {
			// Aggiungo un nuovo elemento, inizializzato a 1
			tickets_stack.push_back(1);
		} else {
			// Altrimenti incremento quello già presente
			auto cursor = tickets_stack.begin();
			for (int i = 0; i < number_of_elements - 1; i++) {
				cursor++;
			}
			(*cursor)++;
		}

		// Restituisco la rappresentazione testuale della lista aggiornata
		return debugComputeTicketsList();
	}

	/**
	 * Funzione utilizzata per la marcatura delle fasi nella libreria di debug.
	 * Viene chiamata nel momento in cui si esce da una fase; ha due funzionalità principali:
	 * - Modifica una variabile globale (stack) scrivendo al suo interno che si è usciti dalla fase
	 *   corrente; in altre parole, segna che si è "risaliti di un livello".
	 * - Restituisce una stringa che contiene la traccia di tutte le fasi attraversate finora.
	 */
	string debugReleaseTicket() {
		// Restituisco la rappresentazione testuale della lista aggiornata
		string s = debugComputeTicketsList();

		// Considero un elemento in meno nella catena
		number_of_elements--;

		// Se risalgo troppo in fretta, dimentico gli indici più interni
		if (tickets_stack.size() > number_of_elements + 1) {
			tickets_stack.pop_back();
		}

		return s;

	}

}




