/******************************************************************************
 * nfa_generator.cpp
 *
 * Project: TranslatedAutomata
 *
 * File sorgente della classe NFAGenerator, classe figlia di AutomataGenerator.
 * Fornisce le implementazioni delle funzioni create specificamente per la generazione di
 * automi NFA.
 *
 ******************************************************************************/

#include <vector>

#include "automata_generator_nfa.hpp"
#include "debug.hpp"

namespace translated_automata {

	NFAGenerator::NFAGenerator(Alphabet alphabet) : AutomataGenerator(alphabet) {}

	NFAGenerator::~NFAGenerator() {}

	/**
	 * Permette la creazione di un NFA secondo i parametri impostati tramite i metodi setter.
	 */
	NFA NFAGenerator::generateRandomAutomaton() {
		// Creo l'NFA
		NFA nfa = NFA();

		// Generazione degli stati
		this->generateStates(nfa);

		// Imposto lo stato iniziale
		StateNFA *initial_state = nfa.getStatesList().front();		// Nota: in questo caso sto assumendo (correttamente) che lo stato che voglio impostare sia il primo in ordine alfabetico
		nfa.setInitialState(initial_state);

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
		vector<StateNFA*> reached_states;
		reached_states.push_back(initial_state);

		/* 1.2) Parallelamente, si tiene traccia dei nodi non ancora marcati come "raggiungibili".
		 * All'inizio tutti gli stati appartengono a questa lista, tranne il nodo iniziale. */
		list<StateNFA*> unreached_queue = nfa.getStatesList();
		unreached_queue.pop_front();

		/* 1.3) Si estrae a caso uno stato "raggiungibile" e uno non "raggiungibile", e si crea una transizione
		 * dal primo al secondo. */
		while (!unreached_queue.empty()) {
			StateNFA* from = reached_states[rand() % reached_states.size()];
			StateNFA* to = unreached_queue.front();
			string label = getRandomLabelFromAlphabet();
			nfa.connectStates(from, to, label);

		/* 1.4) Il secondo stato viene marcato come "raggiungibile". Viene perciò estratto dalla seconda coda e
		 * inserito nella prima. */
			unreached_queue.pop_front();
			reached_states.push_back(to);

		/* 1.5) Se il numero corrente di transizioni è inferiore a N-1, si torna al punto (1.3).
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

			StateNFA* from = this->getRandomState(nfa);
			StateNFA* to = this->getRandomState(nfa);
			string label = getRandomLabelFromAlphabet();
			nfa.connectStates(from, to, label);
		}

		return nfa;
	}

	/**
	 * Genera una lista di oggetti StateNFA, ognuno dei quali con un nome univoco,
	 * e inserisci questi stati all'interno dell'NFA passato come parametro.
	 * Gli stati non presentano transizioni.
	 * Gli stati sono in numero pari alla dimensione prevista dai parametri dell'oggetto
	 * NFAGenerator.
	 */
	void NFAGenerator::generateStates(NFA& nfa) {
		// Flag per l'esistenza di almeno uno stato final
		bool hasFinalStates = false;

		// Generazione degli stati
		for (int s = 0; s < this->getSize(); s++) {
			// Nome dello stato
			string name = this->generateUniqueName();
			// Imposto uno stato come final in base alla percentuale
			bool final = (this->generateNormalizedDouble() < this->getFinalProbability());
			hasFinalStates |= final;

			// Creo lo stato e lo aggiungo all'NFA
			StateNFA *state = new StateNFA(name, final);
			nfa.addState(state);
		}
		DEBUG_ASSERT_TRUE(nfa.size() == this->getSize());

		// Aggiunta forzata di almeno uno stato finale
		if (!hasFinalStates) {
			this->getRandomState(nfa)->setFinal(true);
		}
	}

	/**
	 * Restituisce uno stato casuale all'interno della lista di stati dell'automa.
	 */
	StateNFA* NFAGenerator::getRandomState(NFA& nfa) {
		vector<StateNFA*> states = nfa.getStatesVector();
		return states.at(rand() % states.size());
	}

	/**
	 * Calcola il numero di transizioni da creare all'interno dell'automa.
	 * Il calcolo è effettuato secondo il seguente algoritmo:
	 * - Viene calcolato il numero massimo possibile di transizioni. Tale numero corrisponde alle transizioni orientate (!)
	 *   di un grafo completo, ossia alla quantità [ N * (N - 1) ] dove N è il numero di stati dell'automa.
	 *   (Notare che ogni transizione, essendo orientata, va conteggiata due volte).
	 * - Viene calcolata la percentuale richiesta su tale numero, arrotondata per difetto.
	 * - Se il numero risultante è inferiore al numero necessario per connettere tutti gli stati, viene alzato
	 *   fino al numero di stati - 1.
	 *
	 * In pratica, detta p la percentuale effettiva di transizioni e N il numero di stati, questo metodo accerta che:
	 * 	[ p > 1 / N ].
	 * Questa condizione, specialmente per valori molto grandi di N, è facilmente soddisfatta.
	 */
	unsigned long int NFAGenerator::computeTransitionsNumber() {
		unsigned long int max_n_trans = (this->getSize() - 1) * (this->getSize());
		unsigned long int n = (unsigned long int) (max_n_trans * this->getTransitionPercentage());
		return (n < this->getSize() - 1) ? (this->getSize() - 1) : (n);
	}


} /* namespace translated_automata */




