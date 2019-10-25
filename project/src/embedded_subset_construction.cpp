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
		tuple<NFA*, DFA*, list<Bud>> automaton_translation_result = tau->translate(dfa);
		NFA* translated_nfa = std::get<0>(automaton_translation_result);
		DFA* translated_dfa = std::get<1>(automaton_translation_result);
		list<Bud> buds_list = std::get<2>(automaton_translation_result);

		// Fase 2: Bud Processing
		this->runBudProcessing(translated_nfa, translated_dfa, buds_list);

		// Restituzione del DFA tradotto
		return translated_dfa;
	}

	/**
	 * Metodo privato.
	 * Fornisce un'implementazione della seconda fase dell'algoritmo più generale "Embedded Subset Construction"
	 * per la traduzione di automi (più specificamente, DFA).
	 */
	void EmbeddedSubsetConstruction::runBudProcessing(NFA* translated_nfa, DFA* translated_dfa, list<Bud> buds) {
		// Finché la coda dei bud non si svuota
		while (!buds.empty()) {

			// Estrazione del primo elemento della coda
			Bud current_bud = buds.front();
			buds.pop_front();

			// Preparazione dei riferimenti allo stato e alla label
			StateDFA* current_dfa_state = current_bud.first;
			string current_label = current_bud.second;

			// Transizioni dello stato corrente
			map<string, set<StateDFA*>> current_exiting_transitions = current_dfa_state->getExitingTransitions();

			// Impostazione della front distance e della l-closure
			int front_distance = current_dfa_state->getDistance();
			ExtensionDFA l_closure = current_dfa_state->lClosure(current_label); // Nell'algoritmo è rappresentata con |N.

			// Se dallo stato corrente NON escono transizioni marcate dalla label corrente
			if (current_exiting_transitions[current_label].empty()) {

				// Calcolo del nome che avrebbe uno stato con estensione pari alla l-closure
				string l_closure_name = StateDFA::createNameFromExtension(l_closure);

				// Se esiste uno stato nel DFA con la stessa estensione
				if (translated_dfa->hasState(l_closure_name)) { 																	/* RULE 1 */

					// Aggiunta della transizione dallo stato corrente a quello appena trovato
					StateDFA* child = translated_dfa->getState(l_closure_name);
					current_dfa_state->connectChild(current_label, child);
					this->runDistanceRelocation(child, front_distance + 1);

				}
				// Se nel DFA non c'è nessuno stato con l'estensione prevista
				else { 																												/* RULE 2 */

					// Creazione di un nuovo stato StateDFA apposito e collegamento da quello corrente
					StateDFA* new_state = new StateDFA(l_closure);
					current_dfa_state->connectChild(current_label, new_state);
					new_state->setDistance(front_distance + 1);

					// Per ogni transizione uscente dall'estensione, viene creato e aggiunto alla lista un nuovo Bud
					// Nota: si sta prendendo a riferimento l'NFA associato
					for (string label : new_state->getExitingLabels()) {
						Bud new_bud = Bud(new_state, label);
						buds.push_back(new_bud);
					}

				}

			}
			// Se invece dallo stato corrente escono transizioni marcate dalla label corrente
			else {

				// Per tutte le transizioni marcate dalla label corrente che NON arrivano
				// in uno stato con estensione pari alla l-closure
				for (StateDFA* child : current_dfa_state->getExitingTransitions()[current_label]) {

					// Flag per la gestione delle condizioni
					bool child_is_initial = (*child == *(translated_dfa->getInitialState()));

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

						// TODO EXTENSION UPDATE

					}

					// Se lo stato raggiunto (child) è lo stato iniziale, oppure se possiede una transizione entrante
					// da uno stato con distanza inferiore o pari alla front distance, che non sia lo stato corrente
					else if (child_is_initial || child_minimum_parents_distance <= front_distance) {

						// Calcolo del nome che avrebbe uno stato con estensione pari alla l-closure
						string l_closure_name = StateDFA::createNameFromExtension(l_closure);

						// Se esiste uno stato nel DFA con la stessa estensione
						if (translated_dfa->hasState(l_closure_name)) { 																/* RULE 4 */

							// Ridirezione della transizione dallo stato corrente a quello appena trovato
							StateDFA* old_child = translated_dfa->getState(l_closure_name);
							current_dfa_state->connectChild(current_label, old_child);
							current_dfa_state->disconnectChild(current_label, child);
							this->runDistanceRelocation(old_child, front_distance + 1);

						}
						// Se nel DFA non c'è nessuno stato con l'estensione prevista
						else { 																											/* RULE 5 */

							// Creazione di un nuovo stato StateDFA apposito e collegamento da quello corrente
							StateDFA* new_state = new StateDFA(l_closure);
							current_dfa_state->connectChild(current_label, new_state);
							current_dfa_state->disconnectChild(current_label, child);
							new_state->setDistance(front_distance + 1);

							// Per ogni transizione uscente dall'estensione, viene creato e aggiunto alla lista un nuovo Bud
							// Nota: si sta prendendo a riferimento l'NFA associato
							for (string label : new_state->getExitingLabels()) {
								Bud new_bud = Bud(new_state, label);
								buds.push_back(new_bud);
							}

						}

					}

					// Altrimenti, se non si verificano le condizioni precedenti
					else {																												/* RULE 6 */

						// Per tutte le transizioni ENTRANTI nel figlio
						for (auto &pair : child->getIncomingTransitionsRef()) {
							for (StateDFA* parent : pair.second) {

								// Preparazione delle informazioni sullo stato genitore
								ExtensionDFA parent_x_closure = parent->lClosure(pair.first);
								string x_closure_name = StateDFA::createNameFromExtension(parent_x_closure);

								// Se lo stato genitore ha un'estensione differente dallo stato corrente
								if (x_closure_name != StateDFA::createNameFromExtension(l_closure)) {

									// Rimozione della transizione e aggiunta di un nuovo Bud
									parent->disconnectChild(pair.first, child);
									Bud new_bud = Bud(parent, pair.first);
									buds.push_back(new_bud);

								}

								// TODO EXTENSION UPDATE su (child, l_closure)

							}
						}
					}
				}
			}
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
	void EmbeddedSubsetConstruction::runExtensionUpdate(StateDFA* d_state, ExtensionDFA new_extension, list<Bud>& buds, DFA* dfa) {
		// Computazione degli stati aggiuntivi dell'update
		ExtensionDFA difference_states = StateDFA::subtractExtensions(new_extension, d_state->getExtension());

		// Aggiornamento delle transizioni aggiuntive
		for (StateNFA* nfa_state : difference_states) {
			for (auto &trans : nfa_state->getExitingTransitionsRef()) {
				string label = trans.first;

				// Se il bud NON è contenuto nella lista di Bud
				auto search = std::find_if(buds.begin(), buds.end(),
						[d_state, label](Bud bud) {
							return (bud.first == d_state && bud.second == label);
						}
				);

				// Se non è stato trovato, lo aggiungo alla lista
				if (search == buds.end()) {
					buds.push_back(Bud(d_state, label));
				}
			}
		}

		// Aggiornamento dell'estensione dello stato DFA
		d_state->replaceExtensionWith(new_extension);

		// Verifica dell'esistenza di un secondo stato nel DFA che abbia estensione uguale a "new_extension"
		StateDFA* equivalent_d_state = dfa->getState(StateDFA::createNameFromExtension(new_extension));
		if (equivalent_d_state != NULL && equivalent_d_state != d_state) {

			StateDFA* min_dist_state;
			StateDFA* max_dist_state;

			// Identificazione dello stato con distanza inferiore
			if (d_state->getDistance() < equivalent_d_state->getDistance()) {
				min_dist_state = d_state;
				max_dist_state = equivalent_d_state;
			} else {
				min_dist_state = equivalent_d_state;
				max_dist_state = d_state;
			}

			// Sotto-procedura di sostituzione dello stato con distanza massima con quello con distanza minima:

			// Re-direzione di tutte le transizioni dello stato con distanza massima su quello con distanza minima
			// (Le transizioni duplicate non vengono copiate)
			min_dist_state->copyAllTransitionsFrom(max_dist_state);

			// Rimozione dello stato dall'automa DFA
			dfa->removeState(max_dist_state);		// Rimuove il riferimento dello stato
			max_dist_state->detach();				// Rimuove tutte le sue transizioni

			// All'interno della lista di bud, ogni occorrenza dello stato con dist.max. viene sostituita con quello con dist.min.
			for (Bud bud : buds) {
				if (bud.first == max_dist_state) {
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





