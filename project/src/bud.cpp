/*
 * bud.cpp
 *
 * Implementazione della struttura Bud e BudsList.
 * Una BudsList garantisce:
 * - ordinamento dei bud.
 * - accesso al primo elemento della lista.
 * - unicità degli elementi.
 */

#include "bud.hpp"

#include "debug.hpp"

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	Bud::Bud(ConstructedStateDFA* state, string label) {
		if (state == NULL) {
			DEBUG_LOG_ERROR("Impossibile creare un Bud con stato vuoto");
		}
		this->m_state = state;
		this->m_label = label;
	}

	/**
	 * Distruttore.
	 * Nota: lo stato a cui fa riferimento il bud NON è eliminato
	 * quando viene distrutto il bud.
	 */
	Bud::~Bud() {}

	/**
	 * Restituisce lo stato del Bud.
	 */
	ConstructedStateDFA* Bud::getState() {
		return this->m_state;
	}

	/**
	 * Restituisce la label del Bud.
	 */
	string Bud::getLabel() {
		return this->m_label;
	}

	/**
	 * Restituisce una descrizione testuale del Bud.
	 */
	string Bud::toString() {
		return ("(" + this->m_state->getName() + ", " + this->m_label + ")\033[33m[" + std::to_string(this->m_state->getDistance()) + "]\033[0m");
	}

	/**
	 * Override dell'operatore less.
	 */
	bool Bud::operator<(const Bud& rhs) const {
		return (this->compare(rhs) < 0);
	}

	/**
	 * Override dell'operatore equal.
	 */
	bool Bud::operator==(const Bud& rhs) const {
		return (this->compare(rhs) == 0);
	}

	/**
	 * Funzione di comparazione di due Bud sulla base di:
	 * 1) La distanza dello stato dallo stato iniziale dell'automa.
	 * 2) Il nome dello stato.
	 * 3) La label.
	 */
	int Bud::compare(const Bud& rhs) const {
		// Verifico le distanze degli stati
		if (this->m_state->getDistance() == rhs.m_state->getDistance()) {
			// Caso: Distanze degli stati uguali

			// Verifico i nomi degli stati
			if (this->m_state->getName() == rhs.m_state->getName()) {
				// Caso: Nomi degli stati uguali

				// Il confronto si opera sulle labels
				return this->m_label.compare(rhs.m_label);

			} else {
				// Caso: Nomi degli stati diversi
				// Il confronto si opera sui nomi
				return this->m_state->getName().compare(rhs.m_state->getName());
			}
		} else {
			// Caso: Distanze degli stati diverse
			// Il confronto si opera sulle distanze
			return (this->m_state->getDistance() - rhs.m_state->getDistance());
		}
	}

	/**
	 * Costruttore.
	 * Si occupa di inizializzare le strutture dati.
	 */
	BudsList::BudsList() : m_set() {}

	/**
	 * Distruttore vuoto
	 */
	BudsList::~BudsList() {}

	/**
	 * Restituisce l'informazione riguardo alla presenza di almeno
	 * un Bud nella lista.
	 */
	bool BudsList::empty() {
		return this->m_set.empty();
	}

	/**
	 * Inserisce un nuovo bud all'interno della lista,
	 * solamente se questo bud non è già presente.
	 * In caso l'inserimento vada a buon fine, restituisce TRUE.
	 */
	bool BudsList::insert(Bud* new_bud) {
		// Tento l'inserimento all'interno del Set;
		return ((this->m_set.insert(new_bud)).second);
	}

	/**
	 * Estrae il primo elemento della lista.
	 */
	Bud* BudsList::pop() {
		return (this->m_set.extract(this->m_set.begin()).value());
	}

	/**
	 * Stampa tutti i bud rimanenti nella lista, in ordine.
	 */
	void BudsList::printBuds() {
		for (Bud* b : this->m_set) {
			std::cout << b->toString() << std::endl;
		}
	}

	/**
	 * Rimuove tutti i buds della lista relativi ad un particolare stato.
	 * I buds vengono eliminati.
	 * Inoltre, restituisce tutte le label che appartenenvano a quei bud.
	 */
	set<string> BudsList::removeBudsOfState(ConstructedStateDFA* target_state) {
		DEBUG_LOG("Stampa di tutti i bud attualmente presenti:");
		IF_DEBUG_ACTIVE(printBuds());

		set<string> removed_labels = set<string>();
		for (auto bud_iterator = this->m_set.begin(); bud_iterator != this->m_set.end(); /* No increment */) {

			DEBUG_LOG_SUCCESS("Iterazione sul Bud %s", (*bud_iterator)->toString().c_str());
			if ((*bud_iterator)->getState() == target_state) {

				DEBUG_LOG("Ho trovato un Bud associato allo stato %s da rimuovere dalla lista dei bud", target_state->getName().c_str());

				DEBUG_LOG("Memorizzo la label %s", (*bud_iterator)->getLabel().c_str());
				removed_labels.insert((*bud_iterator)->getLabel());

				DEBUG_LOG("Rimuovo il bud %s", (*bud_iterator)->toString().c_str());
				this->m_set.erase(bud_iterator++);

			} else {

				DEBUG_LOG("Il Bud %s non è associato allo stato %s", (*bud_iterator)->toString().c_str(), target_state->getName().c_str());
				++bud_iterator;
			}
		}


		DEBUG_LOG("Stampa di tutti i bud rimasti nella lista:");
		IF_DEBUG_ACTIVE(printBuds());

		return removed_labels;
	}

}


