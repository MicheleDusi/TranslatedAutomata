/*
 * Bud.hpp
 *
 * Project: TranslatedAutomata
 *
 * Contiene la definizione di un Bud come coppia di ConstructedStateDFA e string.
 * La definizione è stata spostata in un file header apposito per minimizzare le
 * dipendenze fra i vari moduli del progetto, aumentando la granularità delle
 * funzionalità da importare.
 * Inoltre, vengono fornite le dichiarazioni dei metodi di una classe creata
 * appositamente per semplificare la gestione della lista di bud, contenente
 * metodi per l'inserimento (automatico, senza duplicazione) e l'estrazione del Bud
 * con distanza minore.
 *
 */

#ifndef INCLUDE_BUD_HPP_
#define INCLUDE_BUD_HPP_

#include <string>

#include "State.hpp"

using std::set;

namespace translated_automata {

	/** Dichiarazione di una struttura Bud */
	class Bud {

	private:
		ConstructedStateDFA* m_state;
		string m_label;

	public:
		Bud(ConstructedStateDFA* state, string label);
		~Bud();

		ConstructedStateDFA* getState();
		string getLabel();
		string toString();

		bool operator<(const Bud& rhs) const;
		bool operator==(const Bud& rhs) const;
		int compare(const Bud& rhs) const;
	};

	struct BudComparator {
		bool operator() (const Bud* lhs, const Bud* rhs) const {
			return *lhs < *rhs;
		}
	};

//	using Bud = pair<ConstructedStateDFA*, string>;

	/** Dichiarazione di una coda ordinata di Buds, con elementi unici */
	class BudsList {

	private:
		set<Bud*, BudComparator> m_set;

	public:
		BudsList();
		~ BudsList();

		bool empty();
		bool insert(Bud* new_bud);
		Bud* pop();
		set<string> removeBudsOfState(ConstructedStateDFA* state);
		void printBuds();

	};

}

#endif /* INCLUDE_BUD_HPP_ */
