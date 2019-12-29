/*
 * AutomataGeneratorDFA.hpp
 *
 * Project: TranslatedAutomata
 *
 * File sorgente contenente la classe concreta "AutomataGeneratorDFA", figlia della classe "AutomataGenerator<DFA>".
 * Permette la generazione di un automa a stati finiti deterministico, secondo i parametri previsti dalla classe
 * padre "AutomataGenerator".
 *
 * - "generateRandomAutomaton" genera un automa in maniera casuale, senza specifiche caratteristiche topologiche,
 * 	rispettandone la dimensione e la percentuale di transizioni.
 * - "generateStratifiedAutomaton" genera un automa a strati, dove "max_distance" è il numero di strati previsto.
 * 	Se il parametro "max_distance" non viene impostato, risulta pari alla dimensione stessa.
 *
 */

#include "AutomataGeneratorDFA.hpp"

#include "Debug.hpp"

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	DFAGenerator::DFAGenerator(Alphabet alphabet) : AutomataGenerator(alphabet) {}

	/**
	 * Distruttore
	 */
	DFAGenerator::~DFAGenerator() {}


	DFA* DFAGenerator::generateRandomAutomaton() {
		// Creo il DFA
		DFA* dfa = new DFA();

		// Generazione degli stati
		this->generateStates(*dfa);
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
		unsigned long int transitions_number = this->computeDeterministicTransitionsNumber();
		DEBUG_ASSERT_TRUE( transitions_number >= dfa->size() - 1 );

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
		list<StateDFA*> unreached_states_queue = dfa->getStatesList();
		unreached_states_queue.pop_front();

		/* 1.4) Si estrae a caso uno stato "raggiungibile" e uno non "raggiungibile", e si crea una transizione
		 * dal primo al secondo.
		 * Una condizione importante è che lo stato "from" abbia ancora labels uscenti a disposizione per poter
		 * generare la transizione. Se così non fosse, viene direttamente escluso dalla lista degli stati raggiungibili
		 * poiché non più di alcuna utilità. */
		while (!unreached_states_queue.empty()) {
			StateDFA* from = this->getRandomStateWithUnusedLabels(reached_states, unused_labels);
			StateDFA* to = unreached_states_queue.front();

		/* 1.5) Oltre a ciò, viene anche estratta (ed eliminata) una label casuale fra quelle non utilizzate nello stato "from". */
			string label = this->extractRandomUnusedLabel(unused_labels, from);

			dfa->connectStates(from, to, label);

		/* 1.6) Il secondo stato viene marcato come "raggiungibile". Viene perciò estratto dalla seconda coda e
		 * inserito nella prima. */
			unreached_states_queue.pop_front();
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
			StateDFA* to = this->getRandomState(*dfa);
			string label = this->extractRandomUnusedLabel(unused_labels, from);
			dfa->connectStates(from, to, label);
		}

		// Impostazione delle distanza a partire dallo stato iniziale
		dfa->getInitialState()->initDistancesRecursively(0);

		return dfa;
	}

	/**
	 * Metodo che restituisce un automa stratificato.
	 * Il numero di strati è dato dalla massima distanza (parametro "max_distance") impostata nel generatore.
	 * Ogni strato ha la stessa dimensione, con l'eccezione dello strato con distanza 0 (contenente solo il nodo
	 * iniziale) e l'ultimo (che contiene gli stati rimasti, non necessariamente in numero pari al resto).
	 * Si garantisce che:
	 * - L'automa ha il numero di stati previsto dal parametro "size".
	 * - L'automa sia deterministico.
	 * - L'automa la percentuale di transizioni impostata rispetto al totale.
	 * - Ogni strato contenga nodi con la stessa distanza.
	 * - Ogni nodo abbia transizioni verso nodi dello stesso strato o dello strato successivo.
	 */
	DFA* DFAGenerator::generateStratifiedAutomaton() {
		// Creo il DFA
		DFA* dfa = new DFA();

		// Generazione degli stati
		this->generateStates(*dfa);
		DEBUG_ASSERT_TRUE( this->getSize() == dfa->size() );

		// Ottengo un riferimento allo stato iniziale
		StateDFA *initial_state = dfa->getStatesList().front();		// Nota: in questo caso sto assumendo (correttamente) che lo stato che voglio impostare sia il primo in ordine alfabetico
		dfa->setInitialState(initial_state);

		// Verifico se la massima distanza è stata impostata
		if (this->getMaxDistance() == UNDEFINED_VALUE || this->getMaxDistance() >= this->getSize()) {
			// In caso il valore non sia corretto, la imposto al valore massimo.
			// L'automa che se ne otterrà sarà una lunga catena di stati.
			this->setMaxDistance(this->getSize() - 1);
		}

		/* Computo il numero di nodi per ciascuno strato
		 * Il numero sarà dato dal numero di nodi NON iniziali = (N - 1),
		 * dove N è la dimensione ("size") dell'automa, diviso per la massima
		 * distanza e arrotondato per eccesso.
		 */
		unsigned int strata_size = (unsigned int)((this->getSize() - 1) / this->getMaxDistance());
		/* Computo il numero di strati che avranno un nodo in più, a causa della
		 * divisione intera.
		 */
		unsigned int slightly_bigger_strata_number = (this->getSize() - 1) % this->getMaxDistance();

		/* Verifico che sia possibile creare un automa DETERMINISTICO con quel numero di nodi per ciascuno
		 * strato con le label a disposizione nell'alfabeto.
		 */
		DEBUG_LOG("Alpha size = %lu", this->getAlphabet().size());
		DEBUG_LOG("Strata size = %u", strata_size);
		DEBUG_ASSERT_FALSE(this->getAlphabet().size() < (strata_size + ((slightly_bigger_strata_number) ? 1 : 0)));
		if (this->getAlphabet().size() < (strata_size + ((slightly_bigger_strata_number) ? 1 : 0))) {
			DEBUG_LOG_ERROR("Impossibile creare un automa deterministico con un numero di nodi per strato così alto e un numero di label insufficiente");
			std::cout << "Impossibile creare un automa deterministico con un numero di nodi per strato così alto e un numero di label insufficiente\n";
			// Per il momento, viene generato un errore e il metodo si interrompe
			return NULL;
		}

		// Suddivisione degli stati in strati

		// Inizializzazione degli strati
		vector<vector<StateDFA*>> strata = vector<vector<StateDFA*>>();
		vector<StateDFA*> states = dfa->getStatesVector();

		// Rimuovo lo stato iniziale e lo inserisco come primo strato (a distanza 0)
		strata.push_back(vector<StateDFA*>());
		strata[0].push_back(*(states.begin()));
		states.erase(states.begin());

		// Per tutti gli altri stati, li suddivido equamente
		int stratus_index = 1;
		strata.push_back(vector<StateDFA*>());
		for (StateDFA* state : states) {
			// Inserisco lo stato nello strato corrente
			strata[stratus_index].push_back(state);

			// Se ho raggiunto la dimensione massima per lo strato
			if (strata[stratus_index].size() > (strata_size + ((stratus_index > slightly_bigger_strata_number) ? -1 : 0))) {
				// Passo a quello successivo
				stratus_index++;
				strata.push_back(vector<StateDFA*>());
			}
		}

		// DEBUG - Stampa degli strati
		IF_DEBUG_ACTIVE(
		for (auto stratus : strata) {
			std::cout << "STRATO { ";
			for (StateDFA* state : stratus) {
				std::cout << state->getName() << " ";
			}
			std::cout << "}\n";
		})

		/* Una mappa tiene traccia delle label usate per ciascuno stato, in modo che non si abbiano stati con
		 * transizioni uscenti marcate dalla stessa label.
		 */
		map<StateDFA*, Alphabet> unused_labels;
		for (StateDFA* state : dfa->getStatesVector()) {
			unused_labels[state] = Alphabet(this->getAlphabet());
		}

		// Soddisfacimento della RAGGIUNGIBILITA'
		/* L'iterazione su uno strato [i] prevede che i nodi dello strato vengano
		 * connessi con transizioni entranti a quelli dello strato precedente.
		 */
		for (int stratus_index = 1; stratus_index <= this->getMaxDistance(); stratus_index++) {
			// Rendo gli stati di questo strato raggiungibili
			for (StateDFA* state : strata[stratus_index]) {
				StateDFA* parent = this->getRandomStateWithUnusedLabels(strata[stratus_index - 1], unused_labels);
				string label = extractRandomUnusedLabel(unused_labels, parent);
				dfa->connectStates(parent, state, label);
			}
		}

		// Impostazione delle distanze
		initial_state->initDistancesRecursively(0);

		// Soddisfacimento della PERCENTUALE DI TRANSIZIONI

		/* Viene calcolato il numero di transizioni da generare, considerandone il numero massimo
		 * (ossia il numero di transizioni di un grafo completo, per ciascuna possibile etichetta) e
		 * la percentuale di transizioni desiderata.
		 */
		unsigned long int transitions_number = this->computeDeterministicTransitionsNumber();
		DEBUG_ASSERT_TRUE( transitions_number >= dfa->size() - 1 );

		for (	unsigned long int transitions_created = this->getSize() - 1;
				transitions_created < transitions_number;
				transitions_created++) {

			// Estraggo lo stato genitore
			StateDFA* from = this->getRandomStateWithUnusedLabels(states, unused_labels);
			unsigned int from_dist = from->getDistance();
			// Calcolo la distanza dello stato raggiunto (quindi lo strato di appartenenza)
			unsigned int to_dist = (rand() % 2) ? (from_dist) : (from_dist + 1);
			if (to_dist > this->getMaxDistance()) {
				to_dist = this->getMaxDistance();
			}
			// Estraggo lo stato figlio
			StateDFA* to = this->getRandomStateWithUnusedLabels(strata[to_dist], unused_labels);
			string label = this->extractRandomUnusedLabel(unused_labels, from);
			dfa->connectStates(from, to, label);
		}

		return dfa;
	}

	/**
	 * Genera una lista di oggetti StateDFA, ognuno dei quali con un nome univoco,
	 * e inserisci questi stati all'interno del DFA passato come parametro.
	 * Gli stati non presentano transizioni.
	 * Gli stati sono in numero pari alla dimensione prevista dai parametri dell'oggetto
	 * DFAGenerator.
	 */
	void DFAGenerator::generateStates(DFA& dfa) {
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
			dfa.addState(state);
		}
		DEBUG_ASSERT_TRUE(dfa.size() == this->getSize());

		// Aggiunta forzata di almeno uno stato finale
		if (!hasFinalStates) {
			this->getRandomState(dfa)->setFinal(true);
		}
	}

	/**
	 * Estrae casualmente uno stato dall'automa.
	 */
	StateDFA* DFAGenerator::getRandomState(DFA& dfa) {
		vector<StateDFA*> states = dfa.getStatesVector();
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
				DEBUG_LOG("Ho trovato lo stato %s con %lu labels non utilizzate", from->getName().c_str(), unused_labels[from].size());
				from_state_has_unused_labels = true;
			} else {
				// Eliminazione dello stato dalla lista degli stati da cui attingere
				DEBUG_LOG("Elimino lo stato %s con poiché non ha labels inutilizzate", from->getName().c_str());
				int previous_size = states.size();
				states.erase(states.begin() + random_index);
				DEBUG_ASSERT_TRUE( previous_size < states.size());
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
		string extracted_label = unused_labels[state][label_random_index];
		DEBUG_LOG("Estratta l'etichetta %s dallo stato %s", extracted_label.c_str(), state->getName().c_str());

		// Cancellazione della label utilizzata
		unused_labels[state].erase(unused_labels[state].begin() + label_random_index);
		return extracted_label;
	}

} /* namespace translated_automata */
