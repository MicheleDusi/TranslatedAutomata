/******************************************************************************
 * embedded_subset_construction.hpp
 *
 * Project: TranslatedAutomata
 *
 * File sorgente della classe "EmbeddedSubsetConstruction" che implementa l'algoritmo
 * "Embedded Subset Construction" per la traduzione di un automa DFA.
 * Per maggiori informazioni sul funzionamento dell'algoritmo, si veda la documentazione
 * del metodo "run".
 *
 ******************************************************************************/

#include "EmbeddedSubsetConstruction.hpp"

#include <algorithm>

#include "AutomataDrawer_impl.hpp"
//#define DEBUG_MODE
#include "Debug.hpp"
#include "Properties.hpp"

#define REMOVING_LABEL "~"

namespace translated_automata {

	/**
	 * Costruttore vuoto.
	 * Imposta ogni campo ad un valore nullo. Per eseguire l'algoritmo
	 * è infatti necessario chiamare il metodo "loadInputs".
	 */
	EmbeddedSubsetConstruction::EmbeddedSubsetConstruction() {
		this->m_original_dfa = NULL;
		this->m_translation = NULL;
		this->m_buds = NULL;
		this->m_translated_nfa = NULL;
		this->m_translated_dfa = NULL;
	}

	/**
	 * Distruttore.
	 * Elimina gli elementi utilizzati internamente all'algoritmo.
	 * Non elimina gli input (poiché generati all'esterno) e nemmeno il risultato
	 * finale (poiché ancora usato all'esterno).
	 */
	EmbeddedSubsetConstruction::~EmbeddedSubsetConstruction() {
		if (this->m_buds != NULL) {
			delete this->m_buds;
		}
		if (this->m_translated_nfa) {
			delete this->m_translated_nfa;
		}
	}

	/**
	 * Metodo che prepara la classe per l'esecuzione dell'algoritmo.
	 * Permette l'introduzione dei parametri in ingresso e l'istanziazione degli
	 * oggetti che verranno interessati durante il processo.
	 *
	 * INPUT:
	 * @param automaton L'automa da tradurre.
	 * @param translation La traduzione da applicare all'automa.
	 */
	void EmbeddedSubsetConstruction::loadInputs(DFA* automaton, Translation* translation) {
		// Acquisizione degli input
		DEBUG_ASSERT_NOT_NULL(automaton);
		DEBUG_ASSERT_NOT_NULL(translation);
		this->m_original_dfa = automaton;
		this->m_translation = translation;

		// Istanziazione degli oggetti (e rimozione degli eventuali oggetti dell'esecuzione precedente)
		if (this->m_buds != NULL) {
			delete this->m_buds;
		}
		this->m_buds = new BudsList();
		if (this->m_translated_nfa) {
			delete this->m_translated_nfa;
		}
		this->m_translated_nfa = new NFA();
		// Nota: non rimuovo il risultato poiché potrebbe essere ancora utilizzato da metodi esterni
		this->m_translated_dfa = new DFA();
	}

	/**
	 * Metodo che implementa l'algoritmo "Automaton Translation".
	 * Applica la traduzione sull'automa originale, e inoltre genera due automi isomorfi (un NFA e un DFA) che
	 * verranno utilizzati durante la fase di determinizzazione (Bud Processing).
	 */
	void EmbeddedSubsetConstruction::runAutomatonTranslation() {

		// Variabili locali ausiliarie
		map<StateDFA*, pair<StateNFA*, ConstructedStateDFA*>> states_map = map<StateDFA*, pair<StateNFA*, ConstructedStateDFA*>>();
				/* Mantiene le associazioni fra gli stati tradotti:
				 * per ciascun stato dell'automa in input, memorizzo il puntatore allo stato DFA e allo stato NFA in output */

		// Iterazione su tutti gli stati dell'automa in input per creare gli stati corrispondenti
		for (StateDFA* state : this->m_original_dfa->getStatesVector()) {

			// Creo uno stato copia nell'NFA
			StateNFA* translated_nfa_state = new StateNFA(state->getName(), state->isFinal());
			// Lo aggiungo all'NFA
			this->m_translated_nfa->addState(translated_nfa_state);

			// Creo uno stato copia nel DFA
			ExtensionDFA extension;
			extension.insert(translated_nfa_state);
			ConstructedStateDFA* translated_dfa_state = new ConstructedStateDFA(extension);
			// Lo aggiungo al DFA
			this->m_translated_dfa->addState(translated_dfa_state);

			// Associo allo stato originale i due nuovi stati, in modo da poterli ritrovare facilmente
			states_map[state] = pair<StateNFA*, ConstructedStateDFA*>(translated_nfa_state, translated_dfa_state);

		}

		// Termino il primo ciclo su tutti gli stati dell'automa, in modo da procedere
		// solamente quando le associazioni fra gli stati sono complete

		// Iterazione su tutti gli stati dell'automa in input per copiare le transizioni
		for (StateDFA* state : this->m_original_dfa->getStatesVector()) {

			// Vengono recuperati gli stati creati in precedenza, associati allo stato dell'automa originale
			StateNFA* translated_nfa_state = states_map[state].first;
			ConstructedStateDFA* translated_dfa_state = states_map[state].second;

			// Iterazione su tutte le transizioni uscenti dallo stato dell'automa
			for (auto &pair : state->getExitingTransitions()) {

				// Traduzione della label
				string translated_label = this->m_translation->translate(pair.first);

				// Distinguo due casi, basandomi sulla label tradotta
				if (translated_label == EPSILON) {
					// caso EPSILON-TRANSIZIONE

					// Per tutti gli stati figli raggiunti da transizioni marcate con la label originaria
					for (StateDFA* child : pair.second) {

						// Escludo il caso con epsilon-transizione ad anello
						if (child == state) {
							continue;
						}

						// Inserisco la transizione tradotta nell'automa NON DETERMINISTICO N
						translated_nfa_state->connectChild(translated_label, states_map[child].first);

						// Prima di inserire la transizione anche nell'automa DFA tradotto, verifico se le impostazioni
						// richiedono l'uso della "label da rimozione"
						if (DO_USE_REMOVING_LABEL) {
							// Inserisco la transizione tradotta (ma con label DA RIMOZIONE) nell'automa DETERMINISTICO D'
							translated_dfa_state->connectChild(REMOVING_LABEL, states_map[child].second);
							// Inserisco il nuovo bud nella lista, che servirà per rimuovere la transizione.
							this->addBudToList(translated_dfa_state, REMOVING_LABEL);
						}
						else {
							// Inserisco la transizione NON TRADOTTA nell'automa DETERMINISTICO D'
							translated_dfa_state->connectChild(pair.first, states_map[child].second);
							// Inserisco un nuovo bud nella lista con la label NON TRADOTTA. Questo permetterà di rimuovere la transizione durante l'esecuzione.
							this->addBudToList(translated_dfa_state, pair.first);
						}

						// Se lo stato corrente è lo stato iniziale
						if (this->m_original_dfa->isInitial(state)) {
							this->addBudToList(translated_dfa_state, EPSILON);
						}
						// Altrimenti, se lo stato corrente NON è lo stato iniziale, considero
						// tutte le transizioni entranti che non sono marcate da epsilon
						else {
							unsigned int current_distance = state->getDistance();
							for (auto &parent_pair : state->getIncomingTransitionsRef()) {
								string translated_parent_label = this->m_translation->translate(parent_pair.first);
								// Se le transizioni sono marcate da epsilon, non le considero
								if (translated_parent_label != EPSILON) {

									// Altrimenti, per ciascuno stato genitore con distanza minore o uguale
									// (ma solo se le impostazioni prevedono l'uso del controllo sulla distanza)
									for (StateDFA* parent : parent_pair.second) {
										if (!(DO_CHECK_DISTANCE_IN_TRANSLATION) || parent->getDistance() <= current_distance) {

											// Aggiungo il bud (stato_genitore, label)
											this->addBudToList(states_map[parent].second, translated_parent_label);
										}
									}
								}
							}
						}
					}

				}
				else {
					// caso TRANSIZIONE NORMALE

					// Per tutti gli stati figli raggiunti da transizioni marcate con la label originaria
					for (StateDFA* child : pair.second) {
						// Inserisco la transizione tradotta nell'automa NON DETERMINISTICO N
						translated_nfa_state->connectChild(translated_label, states_map[child].first);
						// Inserisco la transizione tradotta nell'automa DETERMINISTICO D'
						translated_dfa_state->connectChild(translated_label, states_map[child].second);
					}

					// Verifico i punti di non determinismo:
					// se gli stati raggiunti dalle transizioni marcate con quest'etichetta sono più di uno,
					// allora aggiungo un bud alla lista.
					auto children_with_translated_label = translated_dfa_state->getExitingTransitionsRef().at(translated_label);
					if (children_with_translated_label.size() > 1) {
						this->addBudToList(translated_dfa_state, translated_label);
					}
				}

			}

		}

		// Marco gli stati iniziali
		this->m_translated_nfa->setInitialState(states_map[this->m_original_dfa->getInitialState()].first);
		this->m_translated_dfa->setInitialState(states_map[this->m_original_dfa->getInitialState()].second);
	}

	/**
	 * Fornisce un'implementazione della seconda fase dell'algoritmo "Embedded Subset Construction"
	 * per la traduzione di automi (più specificamente, DFA).
	 */
	void EmbeddedSubsetConstruction::runBudProcessing() {
		// Finché la coda dei bud non si svuota
		while (!this->m_buds->empty()) {

			DEBUG_MARK_PHASE( "Nuova iterazione per un nuovo bud" ) {

//			DEBUG_LOG( "Stampa dell'automa finale FINO A QUI:" );
//			IF_DEBUG_ACTIVE( DFADrawer drawer = DFADrawer(translated_dfa) );
//			IF_DEBUG_ACTIVE( std::cout << drawer.asString() << std::endl );

			DEBUG_LOG( "Lista dei Bud attuale:");
			IF_DEBUG_ACTIVE( buds_list.printBuds() );

			// Estrazione del primo elemento della coda
			Bud* current_bud = this->m_buds->pop();
			DEBUG_LOG( "Estrazione del Bud corrente: %s", current_bud->toString().c_str());

			// Preparazione dei riferimenti allo stato e alla label
			ConstructedStateDFA* current_dfa_state = current_bud->getState();
			string current_label = current_bud->getLabel();

			// Verifico se si tratta del bud iniziale, l'unico con la label "EPSILON"
			// (In tal caso, non convien proseguire con il ciclo)
			if (current_label == EPSILON && this->m_translated_dfa->isInitial(current_dfa_state)) {											/* RULE 0 */
				DEBUG_LOG( "RULE 0" );
				// Computazione della epsilon-chiusura
				ExtensionDFA epsilon_closure = ConstructedStateDFA::computeEpsilonClosure(current_dfa_state->getExtension());
				// Procedura "Extension Update" sullo stato inziale e sulla sua epsilon-chiusura
				this->runExtensionUpdate(current_dfa_state, epsilon_closure);

				continue;
			}

			// Transizioni dello stato corrente
			map<string, set<StateDFA*>> current_exiting_transitions = current_dfa_state->getExitingTransitions();

			// Impostazione della front distance e della l-closure
			unsigned int front_distance = current_dfa_state->getDistance();

			DEBUG_LOG("Front distance = %u", front_distance);

			ExtensionDFA l_closure = current_dfa_state->computeLClosureOfExtension(current_label); // Nell'algoritmo è rappresentata con un N in grassetto.
			string l_closure_name = ConstructedStateDFA::createNameFromExtension(l_closure);
			DEBUG_LOG("|N| = %s", l_closure_name.c_str());

			// Se le impostazioni lo prevedono, verifico se l'estensione è vuota
			if (DO_AUTOMATON_PRUNING && l_closure.empty()) {
				DEBUG_LOG( "RULE 1" );																								/* RULE 1 */
				DEBUG_MARK_PHASE("Automaton pruning sul bud %s", current_bud->toString().c_str()) {
					this->runAutomatonPruning(current_bud);
				}
			}
			// Se dallo stato corrente NON escono transizioni marcate dalla label corrente
			else if (current_exiting_transitions[current_label].empty()) {

				// Se esiste uno stato nel DFA con la stessa estensione
				if (this->m_translated_dfa->hasState(l_closure_name)) { 																	/* RULE 2 */
					DEBUG_LOG( "RULE 2" );

					// Aggiunta della transizione dallo stato corrente a quello appena trovato
					StateDFA* child = this->m_translated_dfa->getState(l_closure_name);
					current_dfa_state->connectChild(current_label, child);
					DEBUG_LOG("Creazione della transizione %s --(%s)--> %s",
							current_dfa_state->getName().c_str(), current_label.c_str(), child->getName().c_str());

					this->runDistanceRelocation(child, front_distance + 1);

				}
				// Se nel DFA non c'è nessuno stato con l'estensione prevista
				else { 																												/* RULE 3 */
					DEBUG_LOG( "RULE 3" );

					// Creazione di un nuovo stato StateDFA apposito e collegamento da quello corrente
					ConstructedStateDFA* new_state = new ConstructedStateDFA(l_closure);
					this->m_translated_dfa->addState(new_state);
					current_dfa_state->connectChild(current_label, new_state);
					new_state->setDistance(front_distance + 1);

					// Per ogni transizione uscente dall'estensione, viene creato e aggiunto alla lista un nuovo Bud
					// Nota: si sta prendendo a riferimento l'NFA associato
					for (string label : new_state->getLabelsExitingFromExtension()) {
						if (label != EPSILON) {
							this->addBudToList(new_state, label);
						}
					}

				}

			}
			// Se invece dallo stato corrente escono transizioni marcate dalla label corrente
			else {

				// Per tutte le transizioni marcate dalla label corrente che NON arrivano
				// in uno stato con estensione pari alla l-closure
				for (StateDFA* child_ : current_exiting_transitions[current_label]) {
					ConstructedStateDFA* child = (ConstructedStateDFA*) child_;
					DEBUG_LOG("Considero la transizione:  %s --(%s)--> %s", current_dfa_state->getName().c_str(), current_label.c_str(), child->getName().c_str());

					// Escludo gli stati con estensione diversa da |N|
					if (child->getName() == l_closure_name) {
						continue;
					}

					// Flag per la gestione delle condizioni
					bool child_is_initial = (this->m_translated_dfa->isInitial(child));

					// Parametri per la gestione delle condizioni
					current_dfa_state->setDistance(front_distance + 1);
					int child_minimum_parents_distance = child->getMinimumParentsDistance();
					current_dfa_state->setDistance(front_distance);
							/* Nota: l'operazione di settaggio della distanza viene fatta per escludere lo StateDFA corrente dalla lista
							 * di genitori con la distanza minima durante il calcolo. Questo vincolo è necessario nella verifica delle
							 * condizioni per la rule 5 (poco sotto), dove la minima distanza NON deve considerare lo stato corrente. */

					// Se lo stato raggiunto NON è lo stato iniziale, e contemporaneamente
					// non esistono altre transizioni entranti diverse da quella che arriva dal nodo corrente
					// (che sappiamo esistere per certo, per come è stato trovato lo stato child)
					if (!child_is_initial && child->getIncomingTransitionsCount() == 1) {												/* RULE 4 */
						DEBUG_LOG( "RULE 4" );

						DEBUG_MARK_PHASE( "Extension Update" ) {
						// Aggiornamento dell'estensione
						this->runExtensionUpdate(child, l_closure);
						}
					}

					// Se lo stato raggiunto (child) è lo stato iniziale, oppure se possiede una transizione entrante
					// da uno stato con distanza inferiore o pari alla front distance, che non sia lo stato corrente
					else if (child_is_initial || child_minimum_parents_distance <= front_distance) {

//						// Calcolo del nome che avrebbe uno stato con estensione pari alla l-closure // CREDO SIA RIPETUTO
//						string l_closure_name = ConstructedStateDFA::createNameFromExtension(l_closure);

						// Se esiste uno stato nel DFA con la stessa estensione
						if (this->m_translated_dfa->hasState(l_closure_name)) { 																/* RULE 5 */
							DEBUG_LOG( "RULE 5" );

							// Ridirezione della transizione dallo stato corrente a quello appena trovato
							StateDFA* old_child = this->m_translated_dfa->getState(l_closure_name);
							current_dfa_state->connectChild(current_label, old_child);
							current_dfa_state->disconnectChild(current_label, child);
							DEBUG_MARK_PHASE("Distance Relocation su %s, distanza %ul", old_child->getName().c_str(), (front_distance + 1)) {
								this->runDistanceRelocation(old_child, front_distance + 1);
							}

						}
						// Se nel DFA non c'è nessuno stato con l'estensione prevista
						else { 																											/* RULE 6 */
							DEBUG_LOG( "RULE 6" );

							// Creazione di un nuovo stato StateDFA apposito e collegamento da quello corrente
							ConstructedStateDFA* new_state = new ConstructedStateDFA(l_closure);
							this->m_translated_dfa->addState(new_state);
							current_dfa_state->connectChild(current_label, new_state);
							current_dfa_state->disconnectChild(current_label, child);
							new_state->setDistance(front_distance + 1);

							DEBUG_MARK_PHASE( "Aggiunta di tutte le labels" )
							// Per ogni transizione uscente dall'estensione, viene creato e aggiunto alla lista un nuovo Bud
							// Nota: si sta prendendo a riferimento l'NFA associato
							for (string label : new_state->getLabelsExitingFromExtension()) {
								if (label != EPSILON) {
									this->addBudToList(new_state, label);
								}
							}

						}

					}

					// Altrimenti, se non si verificano le condizioni precedenti
					else {																												/* RULE 7 */
						DEBUG_LOG( "RULE 7" );

						set<std::pair<ConstructedStateDFA*, string>> transitions_to_remove = set<std::pair<ConstructedStateDFA*, string>>();

						// Per tutte le transizioni ENTRANTI nel figlio
						for (auto &pair : child->getIncomingTransitionsRef()) {
//						for (auto pair_iterator = child->getIncomingTransitions().begin();
//								pair_iterator != child->getIncomingTransitions().end();
//								pair_iterator++) {
//							auto &pair = *pair_iterator;

//							for (StateDFA* parent_ : pair.second) {
							for (auto trans_iterator = pair.second.begin(); trans_iterator != pair.second.end(); trans_iterator++) {
								ConstructedStateDFA* parent = (ConstructedStateDFA*) *trans_iterator;

								DEBUG_ASSERT_NOT_NULL( parent );

								DEBUG_LOG("Sto considerando la transizione :  %s --(%s)--> %s", parent->getName().c_str(), pair.first.c_str(), child->getName().c_str());

								// Escludo la transizione corrente
								if (parent == current_dfa_state && pair.first == current_label) {
									DEBUG_LOG("Questa è la transizione corrente, non va considerata.");
									continue;
								}

								// Preparazione delle informazioni sullo stato genitore
								ExtensionDFA parent_x_closure = parent->computeLClosureOfExtension(pair.first);
								string x_closure_name = ConstructedStateDFA::createNameFromExtension(parent_x_closure);

								DEBUG_LOG("Confronto le due estensioni: %s VS %s", l_closure_name.c_str(), x_closure_name.c_str());

								// Se lo stato genitore ha un'estensione differente dallo stato corrente
								if (x_closure_name != l_closure_name) {

									DEBUG_LOG("Le due estensioni sono differenti!");
									DEBUG_LOG("Al termine, rimuoverò la transizione :  %s --(%s)--> %s", parent->getName().c_str(), pair.first.c_str(), child->getName().c_str());

									// Aggiungo lo stato "parent" alla lista dei nodi da eliminare
									// NOTA: Non è possibile eliminarlo QUI perché creerebbe problemi al ciclo
									auto t = std::pair<ConstructedStateDFA*, string>(parent, pair.first);
									transitions_to_remove.insert(t);

								} else {
									DEBUG_LOG("Le due estensioni sono uguali, non rimuovo nulla");
								}

							}
						}

						for (auto &pair : transitions_to_remove) {
							// Rimozione della transizione e aggiunta di un nuovo Bud
							/*
							 * pair.first = nodo genitore
							 * pair.second = label
							 */
							pair.first->disconnectChild(pair.second, child);

							DEBUG_LOG("Se non presente, aggiungo il BUD : (%s, %s)", pair.first->getName().c_str(), pair.second.c_str());
							this->addBudToList(pair.first, pair.second);
						}

						DEBUG_MARK_PHASE( "Extension Update" )
						this->runExtensionUpdate(child, l_closure);
					}
				}
			}

			DEBUG_LOG("Arrivato al termine dell'iterazione per lo stato %s", current_dfa_state->getName().c_str() ); }
		}

		// Fase finale: eliminazione dello stato con estensione vuota

		// Verifico se è disattivata l'opzione di Automaton Pruning (che evita la creazione di stati vuoti)
		// e contemporaneamente verifico che siano presenti epsilon-transizioni
		if (!(DO_AUTOMATON_PRUNING) && (TRANSLATION_EPSILON_PERCENTAGE > 0)) {
			StateDFA* empty_state = this->m_translated_dfa->getState(EMPTY_EXTENSION_NAME);
			if (empty_state != NULL) {
				// Se effettivamente esiste uno stato vuoto, viene eliminato
				this->m_translated_dfa->removeState(empty_state);
				DEBUG_LOG("Eliminazione dello stato vuoto completata");
				auto removed_states = this->m_translated_dfa->removeUnreachableStates();
				DEBUG_LOG("Ho eliminato %lu stati irraggiungibili", removed_states.size());
			}
		}
	}

	/**
	 * Restituisce il risultato attuale dell'algoritmo ESC.
	 * Se chiamato al termine di "runBudProcessing" fornisce la traduzione determinizzata dell'automa.
	 * Se chiamato al termine di "runAutomatonTranslation" fornisce l'automa di partenza per l'esecuzione di Bud Processing,
	 * consistente in una semplice traduzione dell'automa originale (senza epsilon-transizioni).
	 */
	DFA* EmbeddedSubsetConstruction::getResult() {
		return this->m_translated_dfa;
	}

	/**
	 * Metodo privato.
	 * Fornisce un'implementazione della procedura "Distance Relocation".
	 * Modifica la distanza di una sequenza di nodi secondo i valori passati come argomento. La modifica
	 * viene poi propagata sui figli finché la nuova distanza risulta migliore. La propagazione avviene
	 * in maniera "width-first".
	 */
	void EmbeddedSubsetConstruction::runDistanceRelocation(list<pair<StateDFA*, int>> relocation_sequence) {
		while (!relocation_sequence.empty()) {
			auto current = relocation_sequence.front();
			relocation_sequence.pop_front();
			StateDFA* current_state = current.first;

			DEBUG_LOG("Esecuzione di \"Distance Relocation\" sullo stato %s", current_state->getName().c_str());

			// Se la distanza "nuova" è inferiore
			if (current_state->getDistance() > current.second) {
				DEBUG_LOG("La distanza è stata effettivamente ridotta da %u a %u", current_state->getDistance(), current.second);
				current_state->setDistance(current.second);

				// Propago la modifica ai figli
				for (auto &trans : current_state->getExitingTransitionsRef()) {
					for (StateDFA* child : trans.second) {

						// Aggiungo il figlio in coda
						relocation_sequence.push_back(pair<StateDFA*, int>(child, current.second + 1));
					}
				}
			}
		}
	}

	/**
	 * Metodo privato.
	 * Wrapper per la funzione "runDistanceRelocation" che richiede in ingresso una lista di coppie
	 * (StateDFA*, int). Poiché più di una volta, all'interno dell'algoritmo "Bud Processing", viene
	 * richiamata la procedura "Distance Relocation" con un singolo argomento, questo metodo fornisce
	 * un'utile interfaccia per semplificare la costruzione dei parametri della chiamata.
	 */
	void EmbeddedSubsetConstruction::runDistanceRelocation(StateDFA* state, int new_distance) {
		pair<StateDFA*, int> new_pair(state, new_distance);
		list<pair<StateDFA*, int>> list;
		list.push_back(new_pair);
		this->runDistanceRelocation(list);
	}

	/**
	 * Metodo privato.
	 * Fornisce un'implementazione per la procedura "Extension Update", che modifica l'estensione di uno
	 * stato DFA aggiungendo eventuali stati NFA non presenti.
	 */
	void EmbeddedSubsetConstruction::runExtensionUpdate(ConstructedStateDFA* d_state, ExtensionDFA& new_extension) {
		// Computazione degli stati aggiuntivi dell'update
		ExtensionDFA difference_states_1 = ConstructedStateDFA::subtractExtensions(new_extension, d_state->getExtension());
		ExtensionDFA difference_states_2 = ConstructedStateDFA::subtractExtensions(d_state->getExtension(), new_extension);

		IF_DEBUG_ACTIVE( int size = this->m_translated_dfa->size(); )
		DEBUG_LOG( "Dimensione attuale dell'automa: %d", size );

		// Aggiornamento delle transizioni aggiuntive
		// Per tutte le transizioni uscenti dagli stati dell'estensione che non sono contenuti già nella vecchia estensione
		// Nota: In teoria si dovrebbero unire i due insiemi, ma scorrendo su entrambi separatamente è più efficiente.
		for (StateNFA* nfa_state : difference_states_1) {
			for (auto &trans : nfa_state->getExitingTransitionsRef()) {
				string label = trans.first;
				if (label != EPSILON) {
					DEBUG_LOG("Data sull'automa N la transizione: %s --(%s)-->", nfa_state->getName().c_str(), label.c_str());
					this->addBudToList(d_state, label);
				}
			}
		}
		for (StateNFA* nfa_state : difference_states_2) {
			for (auto &trans : nfa_state->getExitingTransitionsRef()) {
				string label = trans.first;
				if (label != EPSILON) {
					DEBUG_LOG("Data sull'automa N la transizione: %s --(%s)-->", nfa_state->getName().c_str(), label.c_str());
					this->addBudToList(d_state, label);
				}
			}
		}

		DEBUG_LOG("Estensione prima dell'aggiornamento: %s", ConstructedStateDFA::createNameFromExtension(d_state->getExtension()).c_str());
		// Aggiornamento dell'estensione dello stato DFA
		d_state->replaceExtensionWith(new_extension);
		DEBUG_LOG("Estensione dopo l'aggiornamento: %s", ConstructedStateDFA::createNameFromExtension(d_state->getExtension()).c_str());

		// Verifica dell'esistenza di un secondo stato nel DFA che abbia estensione uguale a "new_extension"
		string new_extension_name = ConstructedStateDFA::createNameFromExtension(new_extension);

		DEBUG_LOG("Verifico se esiste un altro stato in D con estensione pari a : %s", new_extension_name.c_str());

		// Estrazione di tutti gli stati con il nome previsto
		vector<StateDFA*> namesake_states = this->m_translated_dfa->getStatesByName(new_extension_name);

		// Controllo se esiste più di uno stato con la medesima estensione
		if (namesake_states.size() > 1) {
			DEBUG_LOG("E' stato trovato più di uno stato con la stessa estensione \"%s\"", new_extension_name.c_str());

			ConstructedStateDFA* min_dist_state;
			ConstructedStateDFA* max_dist_state;

			// Identificazione dello stato con distanza minore / maggiore
			if (namesake_states[0]->getDistance() < namesake_states[1]->getDistance()) {
				min_dist_state = (ConstructedStateDFA*) namesake_states[0];
				max_dist_state = (ConstructedStateDFA*) namesake_states[1];
			} else {
				min_dist_state = (ConstructedStateDFA*) namesake_states[1];
				max_dist_state = (ConstructedStateDFA*) namesake_states[0];
			}

			DEBUG_ASSERT_TRUE( min_dist_state->getDistance() <= max_dist_state->getDistance() );

			// Sotto-procedura di sostituzione dello stato con distanza massima con quello con distanza minima:

			// Re-direzione di tutte le transizioni dello stato con distanza massima su quello con distanza minima
			// (Le transizioni duplicate non vengono copiate)
			DEBUG_MARK_PHASE("Copia delle transizioni") {
//			std::cout << "MIN:\n" << min_dist_state->toString() << std::endl;
//			std::cout << "MAX:\n" << max_dist_state->toString() << std::endl;
			min_dist_state->copyAllTransitionsOf(max_dist_state);
//			std::cout << "MIN:\n" << min_dist_state->toString() << std::endl;
//			std::cout << "MAX:\n" << max_dist_state->toString() << std::endl;
			}

			// Rimozione dello stato dall'automa DFA
			bool removed = this->m_translated_dfa->removeState(max_dist_state);		// Rimuove il riferimento dello stato
			DEBUG_ASSERT_TRUE( removed );

			// All'interno della lista di bud, elimino ogni occorrenza allo stato con distanza massima,
			// salvando tuttavia le label dei bud che erano presenti.
			set<string> max_dist_buds_labels = this->m_buds->removeBudsOfState(max_dist_state);

			// Per tutte le label salvate, se il relativo bud legato allo stato con distanza minima NON è presente, lo aggiungo
			for (string bud_label : max_dist_buds_labels) {
				if (bud_label != EPSILON) {
					this->addBudToList(min_dist_state, bud_label);
				}
			}

			// Procedura "Distance Relocation" su tutti i figli dello stato con dist.min, poiché i figli acquisiti dallo stato
			// con dist.max. devono essere modificati
			list<pair<StateDFA*, int>> to_be_relocated_list;
			for (auto &trans : min_dist_state->getExitingTransitionsRef()) {
				for (StateDFA* child : trans.second) {
					DEBUG_LOG("Aggiungo alla lista di cui fare la distance_relocation: (%s, %u)", child->getName().c_str(), min_dist_state->getDistance() + 1);
					to_be_relocated_list.push_back(pair<StateDFA*, int>(child, min_dist_state->getDistance() + 1));
				}
			}
			this->runDistanceRelocation(to_be_relocated_list);
			this->m_buds->sort();

		}
	}

	/**
	 * Aggiunge un Bud alla lista, occupandosi della creazione e del fatto che possano esserci duplicati.
	 * Eventualmente, segnala anche gli errori.
	 */
	void EmbeddedSubsetConstruction::addBudToList(ConstructedStateDFA* bud_state, string bud_label) {
		Bud* new_bud = new Bud(bud_state, bud_label);
		// Provo ad inserire il bud nella lista
		if (this->m_buds->insert(new_bud)) {
			// Caso in cui non sono presenti bud uguali
			DEBUG_LOG("Aggiungo alla lista il Bud %s" , new_bud->toString().c_str());
		} else {
			// Caso in cui esistono bud duplicati
			DEBUG_LOG("Il Bud %s è già presente nella lista, pertanto non è stato aggiunto" , new_bud->toString().c_str());
			delete new_bud;
		}
	}

	/**
	 * Metodo che si occupa della gestione del caso "estensione vuota" durante la procedura "Bud Processing".
	 * In pratica, rimuove tutti e soli gli stati non più raggiungibili, poiché connessi solamente tramite lo stato
	 * e la transizione marcate con una label specifica.
	 *
	 * @param bud Il bud corrente che ha generato un'estensione |N| vuota; contiene lo stato da cui partire e la label interessata,
	 */
	void EmbeddedSubsetConstruction::runAutomatonPruning(Bud* bud) {
		// Lista di (potenziali) candidati, ossia coloro che verranno eliminati
		list<ConstructedStateDFA*> candidates = list<ConstructedStateDFA*>();

		// Lista di (potenziali) Entry Points, ossia stati raggiunti dall'esterno
		list<ConstructedStateDFA*> entry_points = list<ConstructedStateDFA*>();

		// Lista degli stati effettivamente raggiunti dall'esterno
		list<ConstructedStateDFA*> reached_states = list<ConstructedStateDFA*>();

		ConstructedStateDFA* starting_state = bud->getState();
		string starting_label = bud->getLabel();
		auto starting_state_exiting_transitions = starting_state->getExitingTransitionsRef();

		DEBUG_MARK_PHASE("Ciclo (1) - Primi figli dell'estensione vuota") {
		// Per tutte le transizioni uscenti dallo stato iniziale che generano l'estensione vuota
		for (StateDFA* _empty_child : starting_state_exiting_transitions[starting_label]) {
			ConstructedStateDFA* empty_child = (ConstructedStateDFA*) _empty_child;
			DEBUG_LOG("Aggiungo alla lista dei candidati lo stato %s", empty_child->getName().c_str());
			candidates.push_back(empty_child);
			// La marcatura indica che lo stato fa parte dei candidati
			// In questo modo è possibile sapere subito se uno stato è nella lista, senza doverlo cercare.
			empty_child->setMarked(true);
			// Infine, viene rimossa la transizione che genera l'estensione vuota
			DEBUG_LOG("Viene rimossa la transizione %s --(%s)--> %s", starting_state->getName().c_str(), starting_label.c_str(), empty_child->getName().c_str());
			starting_state->disconnectChild(starting_label, empty_child);
		}
		}

		DEBUG_MARK_PHASE("Ciclo (2) - Lista dei candidati") {
		// Scorro sulla lista di candidati
		// Si noti che questo ciclo provoca l'aggiunta di stati (sempre in coda) alla lista su cui si itera
		for (auto it = candidates.begin(); it != candidates.end(); /* L'aggiornamento dell'iteratore è effettuato in coda */) {
			// Dereferenzio l'iteratore
			ConstructedStateDFA* current = *it;
			DEBUG_LOG("\tConsidero il possibile candidato all'eliminazione %s", current->getName().c_str());
			current->setMarked(true);

			/* Verifico se esista almeno una transizione che:
			 * 1) Sia entrante nello stato corrente
			 * 2) Provenga da uno stato NON candidato
			 * 3) Provenga da uno stato con distanza strettamente minore della distanza dello stato corrente OPPURE sia lo stato iniziale.
			 *
			 * Se si verificano tutte queste tre condizioni, lo stato corrente è raggiunto "dall'esterno".
			 * Pertanto NON è un candidato, e può essere rimosso dalla lista
			 *
			 * Altrimenti, lo stato è effettivamente un candidato; tutti i suoi figli NON candidati vengono
			 * aggiunti alla lista dei candidati.
			 */
			bool is_reachable = false;
			bool is_possible_entry_point = false;
			// Verifico se è lo stato iniziale
			if (this->m_translated_dfa->isInitial(current)) {
				is_reachable = true;
			}
			else {
				// Scorro su tutte le transizioni entranti nello stato corrente [condizione 1]
				for (auto &pair : current->getIncomingTransitionsRef()) {
					for (StateDFA* _parent : pair.second) {
						ConstructedStateDFA* parent = (ConstructedStateDFA*) _parent;
						DEBUG_LOG("\t\tConsidero la transizione %s --(%s)--> %s", parent->getName().c_str(), pair.first.c_str(), current->getName().c_str());

						// Considero solo le transizioni da stati NON candidati [condizione 2]
						if (!parent->isMarked()) {
							DEBUG_LOG("\t\t\tIl nodo genitore %s non è marcato", parent->getName().c_str());
							// Se la transizione arriva da uno stato con distanza minore [condizione 3]
							if (parent->getDistance() < current->getDistance()) {
								is_reachable = true;
								break;
							}
							else {
								is_possible_entry_point = true;
							}
						}
					}
					// Se è raggiunto, è inutile continuare ad iterare
					if (is_reachable) {
						break;
					}
				}
			}

			// Se lo stato corrente è raggiungibile, non è candidato per l'eliminazione
			if (is_reachable) {
				DEBUG_LOG("Lo stato %s è risultato raggiungibile dall'esterno dell'insieme dei candidati, pertanto non è più marcato", current->getName().c_str());
				current->setMarked(false);
				candidates.erase(it++);
			}
			else {
				// Se è risultato un possibile entry point, viene aggiunto alla lista
				if (is_possible_entry_point) {
					DEBUG_LOG("Lo stato %s è risultato un possibile entry point", current->getName().c_str());
					entry_points.push_back(current);
				}

				DEBUG_LOG("Tutti i figli di %s non marcati sono possibili candidati:", current->getName().c_str());
				// Tutti i figli vengono aggiunti come possibili candidati
				for (auto &pair : current->getExitingTransitionsRef()) {
					for (StateDFA* _child : pair.second) {
						ConstructedStateDFA* child = (ConstructedStateDFA*) _child;
						if (!child->isMarked()) {
							DEBUG_LOG("Aggiungo alla lista dei candidati lo stato %s", child->getName().c_str());
							candidates.push_back(child);
						}
						/* Piccola nota: in questo punto della procedura, ogni figlio dello stato corrente che non è marcato
						 * ha sicuramente una distanza maggiore del padre; infatti, se avesse una distanza inferiore, per come
						 * vengono inseriti (e processati) gli stati potenziali candidati, esso sarebbe già marcato.
						 * Per questo motivo non è necessario inserire un controllo.
						 */
					}
				}

				++it;
			}

			// Termine dell'iterazione sullo stato corrente
		}
		}

		DEBUG_MARK_PHASE("Ciclo (3) - Controllo degli entry points") {
		// Controllo dei possibili entry points
		/* Iterando sulla lista dei potenziali entry points, viene verificato per ciascuno di essi
		 * che esista almeno una transizione entrante che proviene da uno stato NON candidato.
		 * Poiché gli stati candidati sono stati identificati completamente (e sono marcati), questa condizione
		 * dovrebbe essere sufficiente a decretare se uno stato è raggiungibile dall'esterno dell'insieme dei candidati.
		 */
		for (auto it = entry_points.begin(); it != entry_points.end(); ) {
			ConstructedStateDFA* entry_pt = *it;

			bool is_entry_point = false;
			for (auto &pair : entry_pt->getIncomingTransitionsRef()) {
				for (StateDFA* _parent : pair.second) {
					ConstructedStateDFA* parent = (ConstructedStateDFA*) _parent;
					if (!parent->isMarked()) {
						// E' davvero un entry point!
						DEBUG_LOG("Lo stato %s è davvero un entry point!", entry_pt->getName().c_str());
						is_entry_point = true;
						break;
					}
				}
				if (is_entry_point) {
					break;
				}
			}

			// Al termine dell'analisi delle transizioni entranti, posso decretare se lo stato è davvero raggiunto da stati esterni all'insieme di candidati
			if (!is_entry_point) {
				// Se NON è un'entry point, lo rimuovo dalla lista.
				// Questo significa che è effettivamente uno stato candidato
				entry_points.erase(it++);
				DEBUG_ASSERT_TRUE(entry_pt->isMarked());
			}
			else {
				// Altrimenti, se E' un entry point, lo rimuovo dalla lista dei candidati
//				candidates.remove(entry_pt);
					/*
					 * Nota: la precedente operazione potrebbe risultare lunga, pertanto si potrebbe decidere
					 * di spostare il "carico" di ricordarsi quali stati sono da eliminare sulla marcatura.
					 * Al termine, si scorre sui candidati e si verifica quali sono marcati e quali no, per stabilire
					 * quali eliminare.
					 */
				entry_pt->setMarked(false);
				DEBUG_ASSERT_FALSE(entry_pt->isMarked());//
				// Inoltre, lo aggiungo alla lista degli stati effettivamente raggiunti
				reached_states.push_back(entry_pt);
				++it;
			}
		}
		}

		DEBUG_MARK_PHASE("Ciclo (4) - Chiusura degli stati raggiungibili") {
		/* A questo punto, ho in "reached_states" solo stati che vengono raggiunti dall'esterno dell'insieme dei candidati
		 * (gli entry points, appunto).
		 * Pertanto si cerca di computarne la chiusura, ossia tutti gli stati raggiungibili dagli entry points ma che stiano
		 * anche nell'insieme dei candidati (i.e. che abbiano la marcatura attiva).
		 */
		for (auto it = reached_states.begin(); it != reached_states.end(); it++) {
			ConstructedStateDFA* reached_state = *it;

			// In teoria, ogni stato in questa lista non dovrebbe più essere marcato
			DEBUG_ASSERT_FALSE(reached_state->isMarked());

			// Per tutti i figli MARCATI
			for (auto &pair : reached_state->getExitingTransitionsRef()) {
				for (StateDFA* _child : pair.second) {
					ConstructedStateDFA* child = (ConstructedStateDFA*) _child;

					// Se fa ancora parte dei candidati
					if (child->isMarked()) {
						// Rimuovo la marcatura
						child->setMarked(false);
						// Lo aggiungo alla lista degli stati raggiunti
						reached_states.push_back(child);
					}
				}
			}

			/* Infine, per ciascun entry point "annullo" la distanza, in modo tale che possa essere computata
			 * successivamente in maniera comoda e corretta, senza basarsi su informazioni pregresse che (con
			 * l'eliminazione degli stati) risultano errate.
			 */
			reached_state->setDistance(DEFAULT_VOID_DISTANCE);
		}
		}

		DEBUG_MARK_PHASE("Ciclo (5) - Rimozioni dei candidati da eliminare") {
		/* A questo punto in "candidates" sono presenti tutti i candidati per l'eliminazione, ma solo quelli
		 * marcati risultano effettivamente da rimuovere.
		 */
		for (ConstructedStateDFA* candidate : candidates) {
			if (candidate->isMarked()) {
				DEBUG_LOG("Rimuovo lo stato %s", candidate->getName().c_str());
				// Rimuovo lo stato dall'automa (rimuovendo anche le sue transizioni
				this->m_translated_dfa->removeState(candidate);
				// Rimuovo lo stato dalla lista dei bud
				this->m_buds->removeBudsOfState(candidate);
			}
		}
		}

		DEBUG_MARK_PHASE("Ciclo (6) - Ricostruzione delle distanze") {
		/* Per ciascuno degli stati rimasti, computo la distanza.
		 * Nota: l'ordine degli stati (che viene mantenuto in tutta la procedura) si basa sulla distanza; pertanto
		 * dovrebbe essere sufficiente per effettuare il calcolo correttamente.
		 */
		for (ConstructedStateDFA* entry_pt : entry_points) {
			unsigned int new_distance = entry_pt->getMinimumParentsDistance();
			DEBUG_ASSERT_FALSE(new_distance == DEFAULT_VOID_DISTANCE);
			entry_pt->initDistancesRecursively(new_distance + 1);
		}
		// Ri-ordino la lista di bud
		this->m_buds->sort();
		}

	}

} /* namespace translated_automata */





