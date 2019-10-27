/******************************************************************************
 * automata_generator_dfa.hpp
 *
 * Project: TranslatedAutomata
 *
 * File sorgente contenente la classe concreta "DFAGenerator", figlia della classe "AutomataGenerator<DFA>".
 *
 ******************************************************************************/

#include "automata_generator_dfa.hpp"

#include "debug.hpp"

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	DFAGenerator::DFAGenerator() {}

	/**
	 * Distruttore
	 */
	DFAGenerator::~DFAGenerator() {}


	DFA * DFAGenerator::generateRandomAutomaton() {
		// Creo il DFA
		DFA* dfa = new DFA();
		DEBUG_ASSERT_NOT_NULL(dfa);

		// Generazione degli stati
		this->generateStates(dfa);
		DEBUG_ASSERT_TRUE( this->getSize() == dfa->size() );

		// Ottengo un riferimento allo stato iniziale
		StateDFA *initial_state = dfa->getStatesList().front();		// Nota: in questo caso sto assumendo (correttamente) che lo stato che voglio impostare sia il primo in ordine alfabetico
		dfa->setInitialState(initial_state);

		// Creazione delle transizioni

		/* Nota:
		 * La creazione delle transizioni viene effettuata in due passaggi, al fine di garantire
		 * la connessione dell'automa finale.
		 *
		 * FASE (0) : Calcolo del numero di transizioni
		 *
		 * 0.1) Viene calcolato il numero di transizioni da generare, considerandone il numero massimo
		 * (ossia il numero di transizioni di un grafo completo, per ciascuna possibile etichetta) e
		 * la percentuale di transizioni desiderata.
		 * 0.2) Se tale numero non è sufficiente per coprire tutti gli stati, il numero viene portato a N-1
		 * (dove N è il numero di stati dell'automa). Questo numero, per come è stato costruito l'algoritmo,
		 * garantisce la connessione. */
		unsigned long int transitions_number = this->computeTransitionsNumber();

		/* FASE (1) : Creazione di un albero di copertura
		 *
		 * 1.1) Viene preparata una coda di nodi "raggiungibili", alla quale viene aggiunto lo stato iniziale
		 * dell'automa. */
		vector<StateDFA*> reached_states;
		reached_states.push_back(initial_state);

		/* 1.2) Una mappa tiene traccia delle label usate per ciascuno stato, in modo che non si abbiano stati con
		 * transizioni uscenti marcate dalla stessa label.  */
		map<StateDFA*, Alphabet> unused_labels;
		for (StateDFA* state : dfa->getStatesVector()) {
			unused_labels[state] = Alphabet(this->getAlphabet());
		}

		/* 1.3) Parallelamente, si tiene traccia dei nodi non ancora marcati come "raggiungibili".
		 * All'inizio tutti gli stati appartengono a questa lista, tranne il nodo iniziale. */
		list<StateDFA*> unreached_queue = dfa->getStatesList();
		unreached_queue.pop_front();

		/* 1.4) Si estrae a caso uno stato "raggiungibile" e uno non "raggiungibile", e si crea una transizione
		 * dal primo al secondo.
		 * Una condizione importante è che lo stato "from" abbia ancora labels uscenti a disposizione per poter
		 * generare la transizione. Se così non fosse, viene direttamente escluso dalla lista degli stati raggiungibili
		 * poiché non più di alcuna utilità. */
		while (!unreached_queue.empty()) {
			StateDFA* from = this->getRandomStateWithUnusedLabels(reached_states, unused_labels);
			StateDFA* to = unreached_queue.front();

		/* 1.5) Oltre a ciò, viene anche estratta (ed eliminata) una label casuale fra quelle non utilizzate nello stato "from". */
			string label = this->extractRandomUnusedLabel(unused_labels, from);

			dfa->connectStates(from, to, label);

		/* 1.6) Il secondo stato viene marcato come "raggiungibile". Viene perciò estratto dalla seconda coda e
		 * inserito nella prima. */
			unreached_queue.pop_front();
			reached_states.push_back(to);

		/* 1.7) Se il numero corrente di transizioni è inferiore a N-1, si torna al punto (1.3).
		 */
		}

		/* FASE (2) : Raggiungimento della percentuale richiesta
		 *
		 * 2.1) Vengono estratti due stati casuali dall'automa. Fra di essi è creata una transizione.
		 * 2.2) Viene ripetuto il punto (2.1) finché non si raggiunge o si supera il numero di transizioni
		 * calcolato durante la fase (0).
		 */
		for (	unsigned long int transitions_created = this->getSize() - 1;
				transitions_created < transitions_number;
				transitions_created++) {

			StateDFA* from = this->getRandomStateWithUnusedLabels(reached_states, unused_labels);
			StateDFA* to = this->getRandomState(dfa);
			string label = this->extractRandomUnusedLabel(unused_labels, from);
			dfa->connectStates(from, to, label);
		}

		// Impostazione delle distanza a partire dallo stato iniziale
		dfa->getInitialState()->initDistancesRecursively(0);

		return dfa;
	}

	/**
	 * Genera una lista di oggetti StateDFA, ognuno dei quali con un nome univoco,
	 * e inserisci questi stati all'interno del DFA passato come parametro.
	 * Gli stati non presentano transizioni.
	 * Gli stati sono in numero pari alla dimensione prevista dai parametri dell'oggetto
	 * DFAGenerator.
	 */
	void DFAGenerator::generateStates(DFA* dfa) {
		// Flag per l'esistenza di almeno uno stato final
		bool hasFinalStates = false;

		// Generazione degli stati
		for (int s = 0; s < this->getSize(); s++) {
			// Nome dello stato
			string name = this->generateUniqueName();
			// Imposto uno stato come final in base alla percentuale
			bool final = (this->generateNormalizedDouble() < this->getFinalProbability());
			hasFinalStates |= final;

			// Creo lo stato e lo aggiungo all'DFA
			StateDFA *state = new StateDFA(name, final);
			dfa->addState(state);
		}
		DEBUG_ASSERT_TRUE(dfa->size() == this->getSize());

		// Aggiunta forzata di almeno uno stato finale
		if (!hasFinalStates) {
			this->getRandomState(dfa)->setFinal(true);
		}
	}

	/**
	 * Estrae casualmente uno stato dall'automa.
	 */
	StateDFA* DFAGenerator::getRandomState(DFA* dfa) {
		vector<StateDFA*> states = dfa->getStatesVector();
		return states.at(rand() % states.size());
	}

	/**
	 * Restituisce uno stato casuale scelto da una lista assicurandosi che abbia ancora delle labels inutilizzate e disponibili
	 * per la creazione di transizioni.
	 * Nota: NON rimuove la label dalla mappa degli utilizzi.
	 */
	StateDFA* DFAGenerator::getRandomStateWithUnusedLabels(vector<StateDFA*> &states, map<StateDFA*, Alphabet> &unused_labels) {
		if (states.empty()) {
			DEBUG_LOG_ERROR("Impossibile estrarre uno stato da una lista vuota");
			return NULL;
		}

		StateDFA* from;
		bool from_state_has_unused_labels = false; // Flag pessimista
		do {
			// Estrazione di un indice casuale
			int random_index = rand() % states.size();
			from = states[random_index];

			// Verifica dell'esistenza di label inutilizzate ancora disponibili
			if (unused_labels[from].size() > 0) {
				from_state_has_unused_labels = true;
			} else {
				// Eliminazione dello stato dalla lista degli stati da cui attingere
				states.erase(states.begin() + random_index);
			}

		} while (!from_state_has_unused_labels);

		return from;
	}

	/**
	 * Estrae (ed elimina) una label casuale dalla lista di label non ancora utilizzate di uno specifico stato.
	 */
	string DFAGenerator::extractRandomUnusedLabel(map<StateDFA*, Alphabet> &unused_labels, StateDFA* state) {
		if (unused_labels[state].empty()) {
			DEBUG_LOG_ERROR( "Non è stata trovata alcuna label inutilizzata per lo stato %s", state->getName().c_str() );
			return NULL;
		}
		int label_random_index = rand() % unused_labels[state].size();
		unused_labels[state].erase(unused_labels[state].begin() + label_random_index);
		return (unused_labels[state][label_random_index]);
	}

	/**
	 * Calcola il numero di transizioni da creare all'interno dell'automa.
	 * Il calcolo è effettuato secondo il seguente algoritmo:
	 * - Viene calcolato il numero massimo possibile di transizioni. Tale numero corrisponde al numero massimo di transizioni
	 * 	 per ogni stato moltiplicate per il numero N degli stati. Il numero massimo di transizioni di un singolo stato è dato
	 * 	 dal numero L di label disponibili (uno stato DFA non può presentare due transizioni uscenti con la medesima label).
	 * - Viene calcolata la percentuale richiesta su tale numero, arrotondata per difetto.
	 * - Se il numero risultante è inferiore al numero necessario per connettere tutti gli stati, viene alzato
	 *   fino al numero di stati - 1.
	 */
	unsigned long int DFAGenerator::computeTransitionsNumber() {
		unsigned long int max_n_trans = (this->getSize()) * (this->getAlphabet().size());
		unsigned long int n = (unsigned long int) (max_n_trans * this->getTransitionPercentage());
		return (n < this->getSize() - 1) ? (this->getSize() - 1) : (n);
	}

} /* namespace translated_automata */
