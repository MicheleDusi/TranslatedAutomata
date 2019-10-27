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

#include "embedded_subset_construction.hpp"

#include <algorithm>

#define DEBUG_MODE
#include "debug.hpp"

namespace translated_automata {

	/** Costruttore vuoto */
	EmbeddedSubsetConstruction::EmbeddedSubsetConstruction() {}

	/** Distruttore vuoto */
	EmbeddedSubsetConstruction::~EmbeddedSubsetConstruction() {}

	/**
	 * Metodo principale della classe, fornisce lo scheletro dell'algoritmo "Embedded Subset Construction".
	 *
	 * L'algoritmo può essere suddiviso in due fasi:
	 * 1) Nella prima viene applicata la traduzione all'automa di partenza, generando un NFA e un DFA
	 * 	  isomorfi le cui etichette sono state tradotte. Inoltre, viene generata la lista di Bud iniziali.
	 * 2) Nella seconda vengono processati i Buds della lista, con l'obiettivo principale di eliminare i
	 * 	  punti di non determinismo all'interno del DFA.
	 */
	DFA* EmbeddedSubsetConstruction::run(DFA* dfa, Translation* tau) {

		// Fase 1: Automaton Translation
		tuple<NFA*, DFA*, list<Bud>> automaton_translation_result = runAutomatonTranslation(dfa, tau);
		NFA* translated_nfa = std::get<0>(automaton_translation_result);
		DFA* translated_dfa = std::get<1>(automaton_translation_result);
		list<Bud> buds_list = std::get<2>(automaton_translation_result);

		// DEBUG
		printf("\nLISTA DEI BUDS:\n");
		for (Bud b : buds_list) {
			printf("BUD: (%s, %s)\n", b.first->getName().c_str(), b.second.c_str());
		}

		// Fase 2: Bud Processing
		this->runBudProcessing(translated_nfa, translated_dfa, buds_list);

		DEBUG_LOG_SUCCESS("Ho terminato la fase di Bud Processing");

		// Restituzione del DFA tradotto
		return translated_dfa;
	}

	/**
	 * Metodo che implementa l'algoritmo "Automaton Translation".
	 *
	 * INPUT:
	 * @param automaton L'automa da tradurre.
	 * @param translation (parametro implicito) La traduzione da applicare all'automa
	 *
	 * OUTPUT:
	 * @return L'automa NFA tradotto.
	 * @return L'automa DFA isormorfo al precedente, che tuttavia contiene ancora punti di non-determinismo. Risulta utile ai fini dell'algoritmo "Bud Processing".
	 * @return La lista di Bud (coppie <StateDFA, string>) generata durante la traduzione.
	 *
	 */
	tuple<NFA*, DFA*, list<Bud>> EmbeddedSubsetConstruction::runAutomatonTranslation(DFA *automaton, Translation *translation) {
		// Preparazione degli output
		NFA* translated_nfa = new NFA();
		DFA* translated_dfa = new DFA();
		list<Bud> buds;

		// Variabili locali ausiliarie
		map<StateDFA*, pair<StateNFA*, ConstructedStateDFA*>> states_map;
				/* Mantiene le associazioni fra gli stati tradotti:
				 * per ciascun stato dell'automa in input, memorizzo il puntatore allo stato DFA e allo stato NFA in output */

		// Iterazione su tutti gli stati dell'automa in input per creare gli stati corrispondenti
		for (StateDFA* state : automaton->getStatesVector()) {

			// Creo uno stato copia nell'NFA
			StateNFA* translated_nfa_state = new StateNFA(state->getName(), state->isFinal());
			// Lo aggiungo all'NFA
			translated_nfa->addState(translated_nfa_state);

			// Creo uno stato copia nel DFA
			ExtensionDFA extension;
			extension.insert(translated_nfa_state);
			ConstructedStateDFA* translated_dfa_state = new ConstructedStateDFA(extension);
			// Lo aggiungo al DFA
			translated_dfa->addState(translated_dfa_state);

			// Associo allo stato originale i due nuovi stati, in modo da poterli ritrovare facilmente
			states_map[state] = pair<StateNFA*, ConstructedStateDFA*>(translated_nfa_state, translated_dfa_state);

		}

		// Termino il primo ciclo su tutti gli stati dell'automa, in modo da procedere
		// solamente quando le associazioni fra gli stati sono complete

		// Iterazione su tutti gli stati dell'automa in input per copiare le transizioni
		for (StateDFA* state : automaton->getStatesVector()) {

			// Vengono recuperati gli stati creati in precedenza, associati allo stato dell'automa originale
			StateNFA* translated_nfa_state = states_map[state].first;
			ConstructedStateDFA* translated_dfa_state = states_map[state].second;

			// Iterazione su tutte le transizioni uscenti dallo stato dell'automa
			for (auto &pair : state->getExitingTransitions()) {

				// Traduzione della label
				string translated_label = translation->translate(pair.first);

				// Per tutti gli stati figli raggiunti da transizioni marcate con la label originaria
				for (StateDFA* child : pair.second) {

					// Inserisco le transizioni (tradotte) negli automi copiati
					translated_nfa_state->connectChild(translated_label, states_map[child].first);
					translated_dfa_state->connectChild(translated_label, states_map[child].second);

				}

			}

			// Verifica dei punti di non-determinismo
			// Per tutte le transizioni uscenti dallo stato del DFA
			for (auto &pair : translated_dfa_state->getExitingTransitions()) {

				// Verifico se gli stati raggiunti dalle transizioni marcate con quest'etichetta
				// sono più di uno.
				if (pair.second.size() > 1) {
					Bud new_bud = Bud(translated_dfa_state, pair.first);
					buds.push_back(new_bud);
				}
			}

		}

		// Marco gli stati iniziali
		translated_nfa->setInitialState(states_map[automaton->getInitialState()].first);
		translated_dfa->setInitialState(states_map[automaton->getInitialState()].second);

		// Restituzione dei valori in output
		return tuple<NFA*, DFA*, list<Bud>>(translated_nfa, translated_dfa, buds);
	}

	/**
	 * Metodo privato.
	 * Fornisce un'implementazione della seconda fase dell'algoritmo più generale "Embedded Subset Construction"
	 * per la traduzione di automi (più specificamente, DFA).
	 */
	void EmbeddedSubsetConstruction::runBudProcessing(NFA* translated_nfa, DFA* translated_dfa, list<Bud> buds) {
		// Finché la coda dei bud non si svuota
		while (!buds.empty()) {

			DEBUG_MARK_PHASE( Nuova iterazione: è stato pescato un nuovo bud ) {

			// Estrazione del primo elemento della coda
			Bud current_bud = buds.front();
			buds.pop_front();

			DEBUG_LOG_SUCCESS( "BUD PESCATO: (%s, %s)", current_bud.first->getName().c_str(), current_bud.second.c_str());
			DEBUG_LOG( "Mi rimangono %lu buds", buds.size() );

			// Preparazione dei riferimenti allo stato e alla label
			ConstructedStateDFA* current_dfa_state = current_bud.first;
			string current_label = current_bud.second;

			DEBUG_LOG("Etichetta corrente: %s", current_label.c_str() );

			// Transizioni dello stato corrente
			map<string, set<StateDFA*>> current_exiting_transitions = current_dfa_state->getExitingTransitions();

			// Impostazione della front distance e della l-closure
			unsigned int front_distance = current_dfa_state->getDistance();

			DEBUG_LOG_SUCCESS("Distanza del nodo %s = %u", current_dfa_state->getName().c_str(), front_distance);

			ExtensionDFA l_closure = current_dfa_state->computeLClosureOfExtension(current_label); // Nell'algoritmo è rappresentata con |N.
			string l_closure_name = ConstructedStateDFA::createNameFromExtension(l_closure);
			DEBUG_LOG("|N| = %s", l_closure_name.c_str());

			// Se dallo stato corrente NON escono transizioni marcate dalla label corrente
			if (current_exiting_transitions[current_label].empty()) {

				// Se esiste uno stato nel DFA con la stessa estensione
				if (translated_dfa->hasState(l_closure_name)) { 																	/* RULE 1 */
					DEBUG_LOG_SUCCESS( "RULE 1 - state %s", current_dfa_state->getName().c_str() );


					// Aggiunta della transizione dallo stato corrente a quello appena trovato
					StateDFA* child = translated_dfa->getState(l_closure_name);
					current_dfa_state->connectChild(current_label, child);
					this->runDistanceRelocation(child, front_distance + 1);

				}
				// Se nel DFA non c'è nessuno stato con l'estensione prevista
				else { 																												/* RULE 2 */
					DEBUG_LOG_SUCCESS( "RULE 2 - state %s", current_dfa_state->getName().c_str() );

					// Creazione di un nuovo stato StateDFA apposito e collegamento da quello corrente
					ConstructedStateDFA* new_state = new ConstructedStateDFA(l_closure);
					translated_dfa->addState(new_state);
					current_dfa_state->connectChild(current_label, new_state);
					new_state->setDistance(front_distance + 1);

					// Per ogni transizione uscente dall'estensione, viene creato e aggiunto alla lista un nuovo Bud
					// Nota: si sta prendendo a riferimento l'NFA associato
					for (string label : new_state->getLabelsExitingFromExtension()) {
						Bud new_bud = Bud(new_state, label);
						buds.push_back(new_bud);
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

					if (child->getName() == l_closure_name) {
						DEBUG_LOG("Ho incontrato lo stato |N| come figlio");
						continue;
					}

					// Flag per la gestione delle condizioni
					bool child_is_initial = (*child == *(translated_dfa->getInitialState()));

					// Parametri per la gestione delle condizioni
					current_dfa_state->setDistance(front_distance + 1);
					int child_minimum_parents_distance = child->getMinimumParentsDistance();
					current_dfa_state->setDistance(front_distance);
							/* Nota: l'operazione di settaggio della distanza viene fatta per escludere lo StateDFA corrente dalla lista
							 * di genitori con la distanza minima durante il calcolo. Questo vincolo è necessario nella verifica delle
							 * condizioni per la rule 4 (poco sotto), dove la minima distanza NON deve considerare lo stato corrente. */
					DEBUG_LOG("Distanza minima dei genitori trovata: %d", child_minimum_parents_distance);

					// Se lo stato raggiunto NON è lo stato iniziale, e contemporaneamente
					// non esistono altre transizioni entranti diverse da quella che arriva dal nodo corrente
					// (che sappiamo esistere per certo, per come è stato trovato lo stato child)
					if (!child_is_initial && child->getIncomingTransitionsCount() == 1) {												/* RULE 3 */
						DEBUG_LOG_SUCCESS( "RULE 3 - state %s", current_dfa_state->getName().c_str() );

						DEBUG_MARK_PHASE( Extension Update )
						// Aggiornamento dell'estensione
						this->runExtensionUpdate(child, l_closure, buds, translated_dfa);

					}

					// Se lo stato raggiunto (child) è lo stato iniziale, oppure se possiede una transizione entrante
					// da uno stato con distanza inferiore o pari alla front distance, che non sia lo stato corrente
					else if (child_is_initial || child_minimum_parents_distance <= front_distance) {

						// Calcolo del nome che avrebbe uno stato con estensione pari alla l-closure
						string l_closure_name = ConstructedStateDFA::createNameFromExtension(l_closure);

						// Se esiste uno stato nel DFA con la stessa estensione
						if (translated_dfa->hasState(l_closure_name)) { 																/* RULE 4 */
							DEBUG_LOG_SUCCESS( "RULE 4 - state %s", current_dfa_state->getName().c_str() );

							// Ridirezione della transizione dallo stato corrente a quello appena trovato
							StateDFA* old_child = translated_dfa->getState(l_closure_name);
							current_dfa_state->connectChild(current_label, old_child);
							current_dfa_state->disconnectChild(current_label, child);
							this->runDistanceRelocation(old_child, front_distance + 1);

						}
						// Se nel DFA non c'è nessuno stato con l'estensione prevista
						else { 																											/* RULE 5 */
							DEBUG_LOG_SUCCESS( "RULE 5 - state %s", current_dfa_state->getName().c_str() );

							// Creazione di un nuovo stato StateDFA apposito e collegamento da quello corrente
							ConstructedStateDFA* new_state = new ConstructedStateDFA(l_closure);
							translated_dfa->addState(new_state);
							current_dfa_state->connectChild(current_label, new_state);
							current_dfa_state->disconnectChild(current_label, child);
							new_state->setDistance(front_distance + 1);

							DEBUG_LOG_SUCCESS("Istanziazione dello stato %s terminata correttamente", new_state->getName().c_str() );

							DEBUG_MARK_PHASE( Aggiunta di tutte le labels )
							// Per ogni transizione uscente dall'estensione, viene creato e aggiunto alla lista un nuovo Bud
							// Nota: si sta prendendo a riferimento l'NFA associato
							for (string label : new_state->getLabelsExitingFromExtension()) {
								DEBUG_LOG_SUCCESS("Inserisco il nuovo bud (%s, %s)", new_state->getName().c_str(), label.c_str());
								Bud new_bud = Bud(new_state, label);
								buds.push_back(new_bud);
							}

						}

					}

					// Altrimenti, se non si verificano le condizioni precedenti
					else {																												/* RULE 6 */
						DEBUG_LOG_SUCCESS( "RULE 6 - state %s", current_dfa_state->getName().c_str() );

						// Per tutte le transizioni ENTRANTI nel figlio
						for (auto &pair : child->getIncomingTransitionsRef()) {
							for (StateDFA* parent_ : pair.second) {
								ConstructedStateDFA* parent = (ConstructedStateDFA*) parent_;

								DEBUG_ASSERT_NOT_NULL( parent_ );
								DEBUG_ASSERT_NOT_NULL( parent );

								DEBUG_LOG("Sto considerando la transizione :  %s --(%s)--> %s", parent->getName().c_str(), pair.first.c_str(), child->getName().c_str());

								// Preparazione delle informazioni sullo stato genitore
								ExtensionDFA parent_x_closure = parent->computeLClosureOfExtension(pair.first);
								string x_closure_name = ConstructedStateDFA::createNameFromExtension(parent_x_closure);

								DEBUG_LOG("Confronto le due estensioni: %s VS %s", l_closure_name.c_str(), x_closure_name.c_str());

								// Se lo stato genitore ha un'estensione differente dallo stato corrente
								if (x_closure_name != l_closure_name) {

									DEBUG_LOG("Rimuovo la transizione :  %s --(%s)--> %s", parent->getName().c_str(), pair.first.c_str(), child->getName().c_str());
									DEBUG_LOG("Aggiungo il BUD : (%s, %s)", parent->getName().c_str(), pair.first.c_str());

									// Rimozione della transizione e aggiunta di un nuovo Bud
									parent->disconnectChild(pair.first, child);
									Bud new_bud = Bud(parent, pair.first);
									buds.push_back(new_bud);

								}

							}
						}

						DEBUG_MARK_PHASE( Extension Update )
						this->runExtensionUpdate(child, l_closure, buds, translated_dfa);
					}
				}
			}

			DEBUG_LOG_SUCCESS("Arrivato al termine dell'iterazione per lo stato %s", current_dfa_state->getName().c_str() ); }
		}

		DEBUG_LOG_SUCCESS( "Terminata fase \"BUD PROCESSING\"" );
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

			// Se la distanza "nuova" è inferiore
			if (current_state->getDistance() > current.second) {
				current_state->setDistance(current.second);

				// Propago la modifica ai figli
				for (auto &trans : current_state->getExitingTransitionsRef()) {
					for (StateDFA* child : trans.second) {

						// Aggiungo il figlio in coda
						relocation_sequence.push_back(
								pair<StateDFA*, int>(child, current.second + 1));
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
	void EmbeddedSubsetConstruction::runExtensionUpdate(ConstructedStateDFA* d_state, ExtensionDFA new_extension, list<Bud>& buds, DFA* dfa) {
		// Computazione degli stati aggiuntivi dell'update
		ExtensionDFA difference_states = ConstructedStateDFA::subtractExtensions(new_extension, d_state->getExtension());

		int size = dfa->size();
		DEBUG_LOG( "Dimensione attuale dell'automa: %d", size );

		// Aggiornamento delle transizioni aggiuntive
		// Per tutte le transizioni uscenti dagli stati dell'estensione che non sono contenuti già nella vecchia estensione
		for (StateNFA* nfa_state : difference_states) {
			for (auto &trans : nfa_state->getExitingTransitionsRef()) {
				string label = trans.first;
				DEBUG_LOG("Devo aggiornare la transizione: %s --(%s)-->", nfa_state->getName().c_str(), label.c_str());


				DEBUG_LOG("Verifico se il BUD (%s, %s) è nella lista dei buds", d_state->getName().c_str(), label.c_str());
				// Se il bud NON è contenuto nella lista di Bud
				auto search = std::find_if(buds.begin(), buds.end(),
						[d_state, label](Bud bud) {
							return (bud.first == d_state && bud.second == label);
						}
				);

				// Se non è stato trovato, lo aggiungo alla lista
				if (search == buds.end()) {
					DEBUG_LOG("Aggiungo il BUD (%s, %s) poiché NON è nella lista", d_state->getName().c_str(), label.c_str());
					buds.push_back(Bud(d_state, label));
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

		if (dfa->hasState(new_extension_name)) {
			ConstructedStateDFA* equivalent_d_state = (ConstructedStateDFA*) dfa->getState(new_extension_name);
			// Se lo stato trovato è equivalente a quello interessato, la procedura termina
			if (equivalent_d_state == d_state) {
				DEBUG_LOG("Esiste uno stato, ma è quello di prima");
				return;
			}

			DEBUG_LOG_SUCCESS("Ho trovato un vecchio stato nell'automa con la stessa estensione!");

			ConstructedStateDFA* min_dist_state;
			ConstructedStateDFA* max_dist_state;

			// Identificazione dello stato con distanza inferiore
			if (d_state->getDistance() < equivalent_d_state->getDistance()) {
				min_dist_state = d_state;
				max_dist_state = equivalent_d_state;
			} else {
				min_dist_state = equivalent_d_state;
				max_dist_state = d_state;
			}

			DEBUG_ASSERT_TRUE( min_dist_state->getDistance() <= max_dist_state->getDistance() );

			// Sotto-procedura di sostituzione dello stato con distanza massima con quello con distanza minima:

			// Re-direzione di tutte le transizioni dello stato con distanza massima su quello con distanza minima
			// (Le transizioni duplicate non vengono copiate)
			min_dist_state->copyAllTransitionsOf(max_dist_state);

			// Rimozione dello stato dall'automa DFA
			bool removed = dfa->removeState(max_dist_state);		// Rimuove il riferimento dello stato
			DEBUG_ASSERT_TRUE(removed);
			max_dist_state->detachAllTransitions();	// Rimuove tutte le sue transizioni

			// All'interno della lista di bud, ogni occorrenza dello stato con dist.max. viene sostituita con quello con dist.min.
			for (Bud bud : buds) {
				if (bud.first == max_dist_state) {
					DEBUG_LOG("Sostituisco il bud dopo un merge");
					bud.first = min_dist_state;
				}
			}

			// Procedura "Distance Relocation" su tutti i figli dello stato con dist.min, poiché i figli "adottati" dallo stato
			// con dist.max. devono essere modificati
			list<pair<StateDFA*, int>> to_be_relocated_list;
			for (auto &trans : min_dist_state->getExitingTransitionsRef()) {
				for (StateDFA* child : trans.second) {
					to_be_relocated_list.push_back(pair<StateDFA*, int>(child, min_dist_state->getDistance() + 1));
				}
			}
			this->runDistanceRelocation(to_be_relocated_list);

		}
	}

} /* namespace translated_automata */





