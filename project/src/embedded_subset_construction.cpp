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
	void EmbeddedSubsetConstruction::runBudProcessing(NFA* translated_nfa, DFA* translated_dfa, std::list<Bud> buds) {
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
					child->setBetterDistancesRecursively(front_distance + 1);
					// FIXME Al momento la DistanceRelocation viene fatta ricorsivamente. Nell'algoritmo, tuttavia, l'implementazione è iterativa.

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
							 * condizioni per la rule 4 (poco sotto), dove la minima distanza NON deve comprendere lo stato corrente. */

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
							old_child->setBetterDistancesRecursively(front_distance + 1);
							// FIXME Al momento la DistanceRelocation viene fatta ricorsivamente. Nell'algoritmo, tuttavia, l'implementazione è iterativa.

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

} /* namespace translated_automata */
