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
#include "automata_drawer_impl.hpp"

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
	DFA EmbeddedSubsetConstruction::run(DFA& original_dfa, Translation& tau) {

		NFA translated_nfa = NFA();
		DFA translated_dfa = DFA();
		BudsList buds_list = BudsList();

		// Fase 1: Automaton Translation
		this->runAutomatonTranslation(original_dfa, tau, translated_nfa, translated_dfa, buds_list);

		NFADrawer* nfa_drawer = new NFADrawer(translated_nfa);
		std::cout << "NFA:\n" << nfa_drawer->asString() << std::endl;
		DFADrawer* dfa_drawer = new DFADrawer(translated_dfa);
		std::cout << "DFA appena dopo la traduzione:\n" << dfa_drawer->asString() << std::endl;

		// Fase 2: Bud Processing
		this->runBudProcessing(translated_nfa, translated_dfa, buds_list);

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
	void EmbeddedSubsetConstruction::runAutomatonTranslation(DFA& automaton, Translation& translation, NFA& translated_nfa, DFA& translated_dfa, BudsList& buds_list) {

		// Variabili locali ausiliarie
		map<StateDFA*, pair<StateNFA*, ConstructedStateDFA*>> states_map;
				/* Mantiene le associazioni fra gli stati tradotti:
				 * per ciascun stato dell'automa in input, memorizzo il puntatore allo stato DFA e allo stato NFA in output */

		// Iterazione su tutti gli stati dell'automa in input per creare gli stati corrispondenti
		for (StateDFA* state : automaton.getStatesVector()) {

			// Creo uno stato copia nell'NFA
			StateNFA* translated_nfa_state = new StateNFA(state->getName(), state->isFinal());
			// Lo aggiungo all'NFA
			translated_nfa.addState(translated_nfa_state);

			// Creo uno stato copia nel DFA
			ExtensionDFA extension;
			extension.insert(translated_nfa_state);
			ConstructedStateDFA* translated_dfa_state = new ConstructedStateDFA(extension);
			// Lo aggiungo al DFA
			translated_dfa.addState(translated_dfa_state);

			// Associo allo stato originale i due nuovi stati, in modo da poterli ritrovare facilmente
			states_map[state] = pair<StateNFA*, ConstructedStateDFA*>(translated_nfa_state, translated_dfa_state);

		}

		// Termino il primo ciclo su tutti gli stati dell'automa, in modo da procedere
		// solamente quando le associazioni fra gli stati sono complete

		// Iterazione su tutti gli stati dell'automa in input per copiare le transizioni
		for (StateDFA* state : automaton.getStatesVector()) {

			// Vengono recuperati gli stati creati in precedenza, associati allo stato dell'automa originale
			StateNFA* translated_nfa_state = states_map[state].first;
			ConstructedStateDFA* translated_dfa_state = states_map[state].second;

			// Iterazione su tutte le transizioni uscenti dallo stato dell'automa
			for (auto &pair : state->getExitingTransitions()) {

				// Traduzione della label
				string translated_label = translation.translate(pair.first);

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
					Bud* new_bud = new Bud(translated_dfa_state, pair.first);
					buds_list.insert(new_bud);
					DEBUG_LOG("Aggiungo il Bud %s", new_bud->toString().c_str());
				}
			}

		}

		// Marco gli stati iniziali
		translated_nfa.setInitialState(states_map[automaton.getInitialState()].first);
		translated_dfa.setInitialState(states_map[automaton.getInitialState()].second);
	}

	/**
	 * Metodo privato.
	 * Fornisce un'implementazione della seconda fase dell'algoritmo più generale "Embedded Subset Construction"
	 * per la traduzione di automi (più specificamente, DFA).
	 */
	void EmbeddedSubsetConstruction::runBudProcessing(NFA& translated_nfa, DFA& translated_dfa, BudsList& buds_list) {
		// Finché la coda dei bud non si svuota
		while (!buds_list.empty()) {

			DEBUG_MARK_PHASE( "Nuova iterazione per un nuovo bud" ) {
			DEBUG_LOG( "Lista dei Bud attuale:");
			buds_list.printBuds();

			// Estrazione del primo elemento della coda
			Bud* current_bud = buds_list.pop();
			DEBUG_LOG( "Estrazione del Bud corrente: %s", current_bud->toString().c_str());

			// Preparazione dei riferimenti allo stato e alla label
			ConstructedStateDFA* current_dfa_state = current_bud->getState();
			string current_label = current_bud->getLabel();

			// Transizioni dello stato corrente
			map<string, set<StateDFA*>> current_exiting_transitions = current_dfa_state->getExitingTransitions();

			// Impostazione della front distance e della l-closure
			unsigned int front_distance = current_dfa_state->getDistance();

			DEBUG_LOG("Front distance = %u", front_distance);

			ExtensionDFA l_closure = current_dfa_state->computeLClosureOfExtension(current_label); // Nell'algoritmo è rappresentata con un N in grassetto.
			string l_closure_name = ConstructedStateDFA::createNameFromExtension(l_closure);
			DEBUG_LOG("|N| = %s", l_closure_name.c_str());

			// Se dallo stato corrente NON escono transizioni marcate dalla label corrente
			if (current_exiting_transitions[current_label].empty()) {

				// Se esiste uno stato nel DFA con la stessa estensione
				if (translated_dfa.hasState(l_closure_name)) { 																	/* RULE 1 */
					DEBUG_LOG( "RULE 1" );

					// Aggiunta della transizione dallo stato corrente a quello appena trovato
					StateDFA* child = translated_dfa.getState(l_closure_name);
					current_dfa_state->connectChild(current_label, child);
					DEBUG_LOG("Creazione della transizione %s --(%s)--> %s",
							current_dfa_state->getName().c_str(), current_label.c_str(), child->getName().c_str());

					this->runDistanceRelocation(child, front_distance + 1);

				}
				// Se nel DFA non c'è nessuno stato con l'estensione prevista
				else { 																												/* RULE 2 */
					DEBUG_LOG( "RULE 2" );

					// Creazione di un nuovo stato StateDFA apposito e collegamento da quello corrente
					ConstructedStateDFA* new_state = new ConstructedStateDFA(l_closure);
					translated_dfa.addState(new_state);
					current_dfa_state->connectChild(current_label, new_state);
					new_state->setDistance(front_distance + 1);

					// Per ogni transizione uscente dall'estensione, viene creato e aggiunto alla lista un nuovo Bud
					// Nota: si sta prendendo a riferimento l'NFA associato
					for (string label : new_state->getLabelsExitingFromExtension()) {
						DEBUG_LOG("Aggiungo alla lista il bud (%s, %s)", new_state->getName().c_str(), label.c_str());
						buds_list.insert(new Bud(new_state, label));
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
						continue;
					}

					// Flag per la gestione delle condizioni
					// (Effettuato mediante confronto di puntatori)
					bool child_is_initial = (child == translated_dfa.getInitialState());

					// Parametri per la gestione delle condizioni
					current_dfa_state->setDistance(front_distance + 1);
					int child_minimum_parents_distance = child->getMinimumParentsDistance();
					current_dfa_state->setDistance(front_distance);
							/* Nota: l'operazione di settaggio della distanza viene fatta per escludere lo StateDFA corrente dalla lista
							 * di genitori con la distanza minima durante il calcolo. Questo vincolo è necessario nella verifica delle
							 * condizioni per la rule 4 (poco sotto), dove la minima distanza NON deve considerare lo stato corrente. */

					// Se lo stato raggiunto NON è lo stato iniziale, e contemporaneamente
					// non esistono altre transizioni entranti diverse da quella che arriva dal nodo corrente
					// (che sappiamo esistere per certo, per come è stato trovato lo stato child)
					if (!child_is_initial && child->getIncomingTransitionsCount() == 1) {												/* RULE 3 */
						DEBUG_LOG( "RULE 3" );

						DEBUG_MARK_PHASE( "Extension Update" )
						// Aggiornamento dell'estensione
						this->runExtensionUpdate(child, l_closure, buds_list, translated_dfa);

					}

					// Se lo stato raggiunto (child) è lo stato iniziale, oppure se possiede una transizione entrante
					// da uno stato con distanza inferiore o pari alla front distance, che non sia lo stato corrente
					else if (child_is_initial || child_minimum_parents_distance <= front_distance) {

						// Calcolo del nome che avrebbe uno stato con estensione pari alla l-closure
						string l_closure_name = ConstructedStateDFA::createNameFromExtension(l_closure);

						// Se esiste uno stato nel DFA con la stessa estensione
						if (translated_dfa.hasState(l_closure_name)) { 																/* RULE 4 */
							DEBUG_LOG( "RULE 4" );

							// Ridirezione della transizione dallo stato corrente a quello appena trovato
							StateDFA* old_child = translated_dfa.getState(l_closure_name);
							current_dfa_state->connectChild(current_label, old_child);
							current_dfa_state->disconnectChild(current_label, child);
							this->runDistanceRelocation(old_child, front_distance + 1);

						}
						// Se nel DFA non c'è nessuno stato con l'estensione prevista
						else { 																											/* RULE 5 */
							DEBUG_LOG( "RULE 5" );

							// Creazione di un nuovo stato StateDFA apposito e collegamento da quello corrente
							ConstructedStateDFA* new_state = new ConstructedStateDFA(l_closure);
							translated_dfa.addState(new_state);
							current_dfa_state->connectChild(current_label, new_state);
							current_dfa_state->disconnectChild(current_label, child);
							new_state->setDistance(front_distance + 1);

							DEBUG_MARK_PHASE( "Aggiunta di tutte le labels" )
							// Per ogni transizione uscente dall'estensione, viene creato e aggiunto alla lista un nuovo Bud
							// Nota: si sta prendendo a riferimento l'NFA associato
							for (string label : new_state->getLabelsExitingFromExtension()) {
								DEBUG_LOG("Inserisco il nuovo bud (%s, %s) nella lista", new_state->getName().c_str(), label.c_str());
								buds_list.insert(new Bud(new_state, label));
							}

						}

					}

					// Altrimenti, se non si verificano le condizioni precedenti
					else {																												/* RULE 6 */
						DEBUG_LOG( "RULE 6" );

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
							buds_list.insert(new Bud(pair.first, pair.second));
						}

						DEBUG_MARK_PHASE( "Extension Update" )
						this->runExtensionUpdate(child, l_closure, buds_list, translated_dfa);
					}
				}
			}

			DEBUG_LOG("Arrivato al termine dell'iterazione per lo stato %s", current_dfa_state->getName().c_str() ); }
		}
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
	void EmbeddedSubsetConstruction::runExtensionUpdate(ConstructedStateDFA* d_state, ExtensionDFA& new_extension, BudsList& buds, DFA& dfa) {
		// Computazione degli stati aggiuntivi dell'update
		ExtensionDFA difference_states = ConstructedStateDFA::subtractExtensions(new_extension, d_state->getExtension());

		int size = dfa.size();
		DEBUG_LOG( "Dimensione attuale dell'automa: %d", size );

		// Aggiornamento delle transizioni aggiuntive
		// Per tutte le transizioni uscenti dagli stati dell'estensione che non sono contenuti già nella vecchia estensione
		for (StateNFA* nfa_state : difference_states) {
			for (auto &trans : nfa_state->getExitingTransitionsRef()) {
				string label = trans.first;
				DEBUG_LOG("Devo aggiornare la transizione su N: %s --(%s)-->", nfa_state->getName().c_str(), label.c_str());
				bool insertion_result = buds.insert(new Bud(d_state, label));
				if (insertion_result) {
					DEBUG_LOG("Aggiungo il Bud (%s, %s) nella lista", d_state->getName().c_str(), label.c_str());
				} else {
					DEBUG_LOG("Il Bud (%s, %s) è già nella lista, quindi non viene aggiunto", d_state->getName().c_str(), label.c_str());
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
		vector<StateDFA*> namesake_states = dfa.getStatesByName(new_extension_name);

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
			min_dist_state->copyAllTransitionsOf(max_dist_state);

			// Rimozione dello stato dall'automa DFA
			bool removed = dfa.removeState(max_dist_state);		// Rimuove il riferimento dello stato
			max_dist_state->detachAllTransitions();				// Rimuove tutte le sue transizioni
			DEBUG_ASSERT_TRUE( removed );

			// All'interno della lista di bud, elimino ogni occorrenza allo stato con distanza massima,
			// salvando tuttavia le label dei bud che erano presenti.
			set<string> max_dist_buds_labels = buds.removeBudsOfState(max_dist_state);

			// Per tutte le label salvate, se il relativo bud legato allo stato con distanza minima NON è presente, lo aggiungo
			for (string bud_label : max_dist_buds_labels) {
				bool insertion_result = buds.insert(new Bud(min_dist_state, bud_label));
				if (insertion_result) {
					DEBUG_LOG("Aggiungo il Bud (%s, %s) nella lista", d_state->getName().c_str(), bud_label.c_str());
				} else {
					DEBUG_LOG("Il Bud (%s, %s) è già nella lista, quindi non viene aggiunto", d_state->getName().c_str(), bud_label.c_str());
				}
			}

			// Rimozione dello stato con distanza massima
			dfa.removeState(max_dist_state);

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

		}
	}

} /* namespace translated_automata */





