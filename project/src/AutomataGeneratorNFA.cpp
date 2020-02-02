/*
 * AutomataGeneratorNFA.cpp
 *
 * Project: TranslatedAutomata
 *
 * File sorgente contenente la classe concreta "NFAGenerator", figlia della classe "AutomataGenerator<DFA>".
 * Permette la generazione di un automa a stati finiti deterministico, secondo i parametri previsti dalla classe
 * padre "AutomataGenerator".
 *
 * - "generateStratifiedWithSafeZoneAutomaton" genera un automa a strati, dove "max_distance" è il numero di strati previsto.
 * 	Se il parametro "max_distance" non viene impostato, risulta pari alla dimensione stessa. Inoltre, si assicura che tutti
 * 	gli stati prima di una certa distanza (denominata "safe_zone_distance") non presentino punti di non determinismo, ossia
 * 	epsilon-transizioni o transizioni multiple con la stessa label.
 */

#include "AutomataGeneratorNFA.hpp"

#include <cmath>

//#define DEBUG_MODE
#include "Debug.hpp"
#include "Configurations.hpp"

#define RANDOM_PERCENTAGE ((double) rand() / (RAND_MAX))
#define INTRA_STRATUM_TRANSITIONS_PERCENTAGE 0.5

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	NFAGenerator::NFAGenerator(Alphabet alphabet, Configurations* configurations) : AutomataGenerator(alphabet, configurations) {}

	/**
	 * Distruttore.
	 */
	NFAGenerator::~NFAGenerator() {}

	/**
	 * Genera un automa NFA a strati, dove i punti di non determinismo compaiono solamente da una certa distanza in poi.
	 * In questo modo l'impatto di ESC dovrebbe essere ridotto.
	 */
	NFA* NFAGenerator::generateStratifiedWithSafeZoneAutomaton() {
		// Creo l'NFA
		NFA* nfa = new NFA();

		// Generazione degli stati
		this->generateStates(nfa);
		DEBUG_ASSERT_TRUE( this->getSize() == nfa->size() );

		// Ottengo un riferimento allo stato iniziale
		StateNFA *initial_state = nfa->getStatesList().front();		// Nota: in questo caso sto assumendo (correttamente) che lo stato che voglio impostare sia il primo in ordine alfabetico
		nfa->setInitialState(initial_state);

		// Verifico se la massima distanza è stata impostata
		if (this->getMaxDistance() == UNDEFINED_VALUE) {
			// In tal caso, imposto la distanza massima con valore pari al numero di stati - 1
			// L'automa che se ne otterrà sarà una lunga catena di stati.
			this->setMaxDistance(this->getSize() - 1);
		}

		// Verifico se la distanza massima soddisfa le condizioni sul numero degli stati
			// Condizione [1]: Il numero di stati deve essere maggiore della distanza massima
		if (this->getSize() <= this->getMaxDistance()) {
			DEBUG_LOG_ERROR("Impossibile generare un automa con %lu stati e distanza massima pari a %u", this->getSize(), this->getMaxDistance());
			throw "Impossibile generare un automa NFA con una distanza massima maggiore al numero di stati";
		} 	// Condizione [2]: Il numero di stati non deve superare il valore massimo possibile che permetta di mantenere il determinismo nell'automa:
		else if ( (this->getMaxDistance() + 1) * log(this->getAlphabet().size()) < log(this->getSize() * (this->getAlphabet().size() - 1) + 1) ) {
			DEBUG_LOG_ERROR("Impossibile generare un automa con %lu stati e distanza massima pari a %u: numero di stati eccessivo", this->getSize(), this->getMaxDistance());
			throw "Impossibile generare un automa NFA con un numero di stati eccessivi per essere disposti deterministicamente entro la distanza massima";
		}
		/* Nota: In teoria un automa non deterministico può evitare di verificare la condizione [2], perché può riutilizzare la stessa label per
		 * più transizioni. Il vincolo [2], quindi, sarebbe relativo agli automi DFA.
		 * Tuttavia, per permettere una safe-zone sufficientemente ampia (ossia una zona deterministica ampia) devo verificare la condizione anche nel
		 * caso estremo (ossia nel caso safe-zone = tutto l'automa). In casi intermedi il calcolo sarebbe troppo complicato.
		 */

		// Suddivisione degli stati in strati

		// Inizializzazione degli strati
		vector<vector<StateNFA*>> strata = vector<vector<StateNFA*>>();
		vector<vector<StateNFA*>> safe_zone_strata = vector<vector<StateNFA*>>();
		/*
		 * Nota: vengono creati due vettori di strati:
		 * - Il primo è il vettore di strati, non viene modificato una volta riempito
		 * - Il secondo corrisponde alla SAFE-ZONE, e serve per tenere traccia degli stati che hanno ancora label disponibili,
		 * senza che l'estrazione casuale generi attese infinite
		 */

		vector<StateNFA*> states = nfa->getStatesVector();

		// Creo tanti strati in numero pari alla distanza massima + 1
		for (int d = 0; d <= this->getMaxDistance(); d++) {
			strata.push_back(vector<StateNFA*>());
		}
		// Creo tanti strati per la SAFE ZONE
		for (int d = 0; d <= this->getSafeZoneDistance(); d++) {
			safe_zone_strata.push_back(vector<StateNFA*>());
		}

		// Li riempio mettendone uno per strato, andando a riempire in "orizzontale".
		int stratum_starting_index = 0;
		int stratum_index = stratum_starting_index;
		for (int s = 0; s < states.size(); s++) {
			// Aggiungo uno stato allo strato corrente
			strata[stratum_index].push_back(states[s]);
			DEBUG_LOG("Aggiungo lo stato %d allo strato %d", s, stratum_index);

			// Verifico se lo strato corrente ha raggiunto il numero massimo di stati
			if ( log( strata[stratum_index].size() ) >= stratum_index * log( this->getAlphabet().size() )) {
				DEBUG_LOG("Lo strato %d ha raggiunto il numero massimo di stati al suo interno", stratum_index);
				stratum_starting_index++;
				DEBUG_LOG("Al successivo ciclo partirò dallo strato di indice %d", stratum_starting_index);
				/*
				 * Nota: perché questo controllo sul ciclo funzioni, mi baso sul fatto che ogni strato è più capiente del precedente,
				 * quindi si esauriranno in ordine. Perciò posso incrementare un solo indice che memorizzi il primo strato ancora non
				 * pieno, invece che un flag per ciascuno che dica se uno strato è pieno o meno.
				 */
			}

			// Passo allo strato successivo
			stratum_index++;

			// Verifico se devo ricominciare da capo
			if (stratum_index >= strata.size()) {
				DEBUG_LOG("Sono arrivato all'ultimo strato, ricomincerò dallo strato %d", stratum_starting_index);
				stratum_index = stratum_starting_index;
			}
		}

		// Copio i primi strati della safe-zone nel vettore apposito
		for (int d = 0; d < safe_zone_strata.size() && d < strata.size(); d++) {
			// Per ciascuno stato nello strato d-esimo
			for (int i = 0; i < strata[d].size(); i++) {
				// Copio lo stato negli strati della safe-zone
				safe_zone_strata[d].push_back(strata[d][i]);
			}
		}


		// DEBUG - Stampa degli strati
		DEBUG_LOG("Gli strati ottenuti sono %lu, e comprendono i seguenti stati:", strata_from.size());
		IF_DEBUG_ACTIVE(
		for (auto stratus : strata_from) {
			std::cout << "STRATO { ";
			for (StateNFA* state : stratus) {
				std::cout << state->getName() << " ";
			}
			std::cout << "}\n";
		})

		/* Una mappa tiene traccia delle label usate per ogni stato della safe-zone.
		 * In questo modo ci si assicura il determinismo evitando duplicati nelle label.
		 * Si richiede il determinismo a TUTTI E SOLI gli stati con distanza MINORE della SafeZoneDistance.
		 */
		map<StateNFA*, Alphabet> unused_labels = map<StateNFA*, Alphabet>();
		unsigned int limit = (this->getSafeZoneDistance() < strata.size()) ? this->getSafeZoneDistance() : strata.size();
		for (stratum_index = 0; stratum_index < limit; stratum_index++) {
			for (StateNFA* state : strata[stratum_index]) {
				unused_labels[state] = Alphabet(this->getAlphabet());
			}
		}

		// Soddisfacimento della RAGGIUNGIBILITA'
		/* L'iterazione su uno strato [i] prevede che i nodi dello strato vengano
		 * connessi con transizioni entranti a quelli dello strato precedente.
		 */
		for (stratum_index = 1; stratum_index <= this->getMaxDistance(); stratum_index++) {
			// Rendo gli stati di questo strato raggiungibili

			// CASO 1
			// Lo strato precedente è nella safe-zone, devo assicurare il determinismo
			if (stratum_index <= this->getSafeZoneDistance()) {
				for (StateNFA* state : strata[stratum_index]) {
					StateNFA* parent = this->getRandomStateWithUnusedLabels(strata[stratum_index - 1], unused_labels);
					string label = extractRandomUnusedLabel(unused_labels, parent);
					nfa->connectStates(parent, state, label);
				}
			}
			// CASO 2
			// Sono fuori dalla safe-zone, posso estrarre una label a caso (eventualmente anche EPSILON)
			else {
				for (StateNFA* state : strata[stratum_index]) {
					StateNFA* parent = this->getRandomState(strata[stratum_index - 1]);

					string random_label;
					if (RANDOM_PERCENTAGE <= this->getEpsilonProbability()) {
						random_label = EPSILON;
					} else {
						random_label = this->getAlphabet()[rand() % this->getAlphabet().size()];
					}

					nfa->connectStates(parent, state, random_label);
				}
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
		DEBUG_ASSERT_TRUE( transitions_number >= nfa->size() - 1 );

		for (	unsigned long int transitions_created = this->getSize() - 1;
				transitions_created < transitions_number;
				transitions_created++) {

			// Estraggo casualmente lo strato da cui creare la transizione
			stratum_index = rand() % (this->getMaxDistance() + 1);

			StateNFA* from;
			string label;

			// CASO 1
			// Sono nella safe-zone
			if (stratum_index < this->getSafeZoneDistance()) {

				// In tal caso, estraggo uno stato genitore assicurandomi di poter garantire il determinismo
				// Sicuramente gli stati rimasti hanno ancora delle label utilizzabili
				from = this->getRandomStateWithUnusedLabels(unused_labels);
				// Estraggo una label ancora inutilizzata
				label = this->extractRandomUnusedLabel(unused_labels, from);
				/*
				 * Nota: in questo caso viene corretta la distanza dello stato, ossia lo strato
				 */
				stratum_index = from->getDistance();

			}
			// CASO 2
			// Sono fuori dalla safe-zone
			else {

				// In tal caso, estraggo uno stato genitore casuale
				from = this->getRandomState(strata[stratum_index]);
				// Estraggo una label casuale, che puà anche essere EPSILON
				if (RANDOM_PERCENTAGE <= this->getEpsilonProbability()) {
					label = EPSILON;
				} else {
					label = this->getAlphabet()[rand() % this->getAlphabet().size()];
				}

			}

			// Calcolo la distanza dello stato raggiunto (quindi lo strato di appartenenza)
			unsigned int to_dist = (RANDOM_PERCENTAGE <= INTRA_STRATUM_TRANSITIONS_PERCENTAGE) ?
					(stratum_index) :
					(stratum_index + 1);
			// Non è possibile superare la distanza massima
			if (to_dist > this->getMaxDistance()) {
				to_dist = this->getMaxDistance();
			}
			StateNFA* to = this->getRandomState(strata[to_dist]);

			// Creo la connessione
			DEBUG_ASSERT_NOT_NULL(from);
			DEBUG_ASSERT_NOT_NULL(to);
			nfa->connectStates(from, to, label);
			/* Nota: al momento non ho modo di garantire che la connessione non esista già, nel caso 2 */
		}

		return nfa;
	}

	/**
	 * Genera un automa NFA privo di cicli.
	 * Non utilizza il parametro "SafeZoneDistance" né "maxDistance".
	 */
	NFA* NFAGenerator::generateAcyclicAutomaton() {
		// Creo l'NFA
		NFA* nfa = new NFA();

		// Generazione degli stati
		this->generateStates(nfa);
		vector<StateNFA*> states = nfa->getStatesVector(); // Riferimento agli stati
		DEBUG_ASSERT_TRUE( this->getSize() == nfa->size() );

		// Soddisfacimento della RAGGIUNGIBILITA'
		for (int i = 1; i < states.size(); i++) {
			// Determino una label casuale (eventualmente anche epsilon)
			string random_label;
			if (RANDOM_PERCENTAGE <= this->getEpsilonProbability()) {
				random_label = EPSILON;
			} else {
				random_label = this->getAlphabet()[rand() % this->getAlphabet().size()];
			}

			// Effettuo la connessione:
			// FROM = uno stato generico precedente a quello corrente nell'ordine del vettore
			// TO = lo stato corrente del vettore all'indice i
			nfa->connectStates(states[rand() % i], states[i], random_label);
		}

		// Soddisfacimento della PERCENTUALE DI TRANSIZIONI

		/* Viene calcolato il numero di transizioni da generare, considerandone il numero massimo
		 * (ossia il numero di transizioni di un grafo completo, per ciascuna possibile etichetta) e
		 * la percentuale di transizioni desiderata.
		 */
		unsigned long int transitions_number = this->computeDeterministicTransitionsNumber();
		DEBUG_ASSERT_TRUE( transitions_number >= nfa->size() - 1 );

		for (	unsigned long int transitions_created = this->getSize() - 1;
				transitions_created < transitions_number;
				transitions_created++) {

			// Estraggo una label casuale, che può anche essere EPSILON
			string label;
			if (RANDOM_PERCENTAGE <= this->getEpsilonProbability()) {
				label = EPSILON;
			} else {
				label = this->getAlphabet()[rand() % this->getAlphabet().size()];
			}

			// Vengono estratti gli indici
			int random_index_1 = (rand() % (states.size() - 1)) + 1;
			int random_index_2 = (rand() % (states.size() - 1)) + 1;

			int from_index;
			int to_index;

			if (random_index_1 > random_index_2) {
				from_index = random_index_2;
				to_index = random_index_1;
			} else {
				from_index = random_index_1;
				to_index = random_index_2;
			}

			// Creo la connessione
			nfa->connectStates(
					states[from_index], 	// FROM
					states[to_index],		// TO
					label);
			/* Nota: al momento non ho modo di garantire che la connessione non esista già, nel caso 2 */
		}

		// Impostazione delle distanze e dello stato iniziale
		nfa->setInitialState(states[0]);
		states[0]->initDistancesRecursively(0);

		return nfa;
	}

	/**
	 * Genera una lista di oggetti StateNFA, ognuno dei quali con un nome univoco,
	 * e inserisci questi stati all'interno del NFA passato come parametro.
	 * Gli stati non presentano transizioni.
	 * Gli stati sono in numero pari alla dimensione prevista dai parametri dell'oggetto
	 * NFAGenerator.
	 */
	void NFAGenerator::generateStates(NFA* nfa) {
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
			StateNFA *state = new StateNFA(name, final);
			nfa->addState(state);
		}
		DEBUG_ASSERT_TRUE((nfa->size()) == this->getSize());

		// Aggiunta forzata di almeno uno stato finale
		if (!hasFinalStates) {
			this->getRandomState(nfa)->setFinal(true);
		}
	}

	/**
	 * Estrae casualmente uno stato dall'automa NFA passato come parametro.
	 * NOTA: Questo metodo richiede ad ogni chiamata la costruzione del vettore
	 * di stati dell'automa, il che potrebbe non essere computazionalmente troppo efficiente.
	 */
	StateNFA* NFAGenerator::getRandomState(NFA* nfa) {
		vector<StateNFA*> states = nfa->getStatesVector();
		return states.at(rand() % states.size());
	}

	/**
	 * Estrae casualmente uno stato dal vector di stati passato come parametro.
	 */
	StateNFA* NFAGenerator::getRandomState(vector<StateNFA*>& states) {
		return states.at(rand() % states.size());
	}

	/**
	 * Restituisce uno stato casuale scelto da una lista assicurandosi che abbia ancora delle labels inutilizzate e disponibili
	 * per la creazione di transizioni.
	 * Nota: NON rimuove la label dalla mappa degli utilizzi.
	 */
	StateNFA* NFAGenerator::getRandomStateWithUnusedLabels(vector<StateNFA*> &states, map<StateNFA*, Alphabet> &unused_labels) {
		// Creo un vettore ausiliario con gli stati in ingresso fra cui selezionare
		vector<StateNFA*> states_aux = vector<StateNFA*>(states);

		StateNFA* from;
		bool from_state_has_unused_labels = false; // Flag pessimista
		do {
			// Verifico che siano presenti stati nel vettore
			if (states_aux.empty()) {
				DEBUG_LOG_ERROR("Impossibile estrarre uno stato da una lista vuota");
				throw "Impossibile estrarre uno stato da una lista vuota";
			}

			// Estrazione di uno stato casuale dal vettore
			int random_index = rand() % states_aux.size();
			from = states_aux[random_index];

			// Verifica dell'esistenza di label inutilizzate ancora disponibili
			if (unused_labels.count(from) > 0 && unused_labels.at(from).size() > 0) {
				DEBUG_LOG("Ho trovato lo stato %s con %lu labels non utilizzate", from->getName().c_str(), unused_labels.at(from).size());
				from_state_has_unused_labels = true;
			}
			// Altrimenti elimino lo stato dalla lista
			else {
				DEBUG_LOG("Lo stato %s non ha più label inutilizzate; eviterò di selezionarlo nelle iterazioni successive", from->getName().c_str());
				states_aux.erase(states_aux.begin() + random_index);
			}

		} while (!from_state_has_unused_labels);

		return from;
	}

	StateNFA* NFAGenerator::getRandomStateWithUnusedLabels(map<StateNFA*, Alphabet> &unused_labels) {
		StateNFA* from;
		bool from_state_has_unused_labels = false; // Flag pessimista
		do {
			// Verifico che siano presenti stati nel vettore
			if (unused_labels.empty()) {
				DEBUG_LOG_ERROR("Impossibile estrarre uno stato da una lista vuota");
				throw "Impossibile estrarre uno stato da una lista vuota";
			}

			// Estrazione di uno stato casuale dalla lista
			int random_index = rand() % unused_labels.size();
			// Ciclo finché l'indice non corrisponde
			int i = 0;
			for (auto &pair : unused_labels) {
				if (i == random_index) {
					from = pair.first;
					// Verifico direttamente se ci sono label disponibili
					if (pair.second.size() > 0) {
						DEBUG_LOG("Ho trovato lo stato %s con %lu labels non utilizzate", from->getName().c_str(), unused_labels.at(from).size());
						from_state_has_unused_labels = true;
						return from;
					}

					// Esco dal ciclo
					break;

				} else {
					i++;
				}
			}

			DEBUG_LOG("Lo stato %s non ha più label inutilizzate; eviterò di selezionarlo nelle iterazioni successive", from->getName().c_str());
			unused_labels.erase(from);

		} while (!from_state_has_unused_labels);
		/*
		 * NOTA: in teoria qui si parte dall'assunto che la condizione rimanga sempre falsa e che il ciclo venga ripetuto.
		 * Se infatti diventa vera, lo stato viene restituito prima.
		 */

		return from;
	}

	/**
	 * Estrae (ed elimina) una label casuale dalla lista di label non ancora utilizzate di uno specifico stato.
	 */
	string NFAGenerator::extractRandomUnusedLabel(map<StateNFA*, Alphabet> &unused_labels, StateNFA* state) {
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
