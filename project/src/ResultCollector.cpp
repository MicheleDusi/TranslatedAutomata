/*
 * ResultCollector.cpp
 *
 * Modulo che si occupa di archiviare i risultati di un problema.
 */

#include "ResultCollector.hpp"

#include "AutomataDrawer_impl.hpp"
#include "Debug.hpp"
#include "Properties.hpp"

namespace translated_automata {

	// Stringhe per la visualizzazione delle statistiche
	vector<string> stat_headlines = vector<string> {
		"SC_TIME   [ms]",
		"ESC_TIME  [ms]",
		"SOL_SIZE   [#]",
		"SOL_GROWTH [%]"
	};

	/**
	 * Costruttore.
	 */
	ResultCollector::ResultCollector() {
		this->m_results = list<Result*>();
	}

	/**
	 * Distruttore della classe ResultCollector.
	 */
	ResultCollector::~ResultCollector() {}

	/**
	 * Metodo privato.
	 * Restituisce un getter che, applicato ad un risultato, permette di
	 * estrarre la statistica richiesta come parametro.
	 * Esempio:
	 * La chiamata di getStatGetter(SC_TIME) restituisce una funzione che
	 * accetta come parametro un risultato e restituisce il tempo impiegato
	 * per eseguire SC ed ottenere quel risultato.
	 */
	std::function<double(Result*)> ResultCollector::getStatGetter(ResultStat stat) {
		std::function<double(Result*)> getter;
		switch(stat) {

		// Tempo di esecuzione di SC
		case SC_TIME :
			getter = [](Result* result) {
				return (double) (result->sc_elapsed_time);
			};
			break;

		// Tempo di esecuzione di ESC
		case ESC_TIME :
			getter = [](Result* result) {
				return (double) (result->esc_elapsed_time);
			};
			break;

		// Dimensione dell'automa ottenuto nella soluzione. (Nota: si considera come riferimeno SC).
		case SOL_SIZE :
			getter = [](Result* result) {
				return (double) (result->sc_solution->size());
			};
			break;

		// Rapporto fra la dimensione dell'automa ottenuto nella soluzione e la dimensione dell'automa originale.
		case SOL_GROWTH :
			getter = [](Result* result) {
				return ((double) (result->sc_solution->size()) / AUTOMATON_SIZE) * 100;
			};
			break;

		default :
			DEBUG_LOG_ERROR("Valore %d non riconosciuto all'interno dell'enumerazione ResultStat", stat);
			getter = [](Result* result) {
				return -1;
			};
			break;
		}
		return getter;
	}

	/**
	 * Aggiunge un risultato alla lista.
	 * Non essendo ordinata, l'aggiunta è effettuata di default in coda.
	 */
	void ResultCollector::addResult(Result* result) {
		DEBUG_ASSERT_NOT_NULL(result);
		if (result != NULL) {
			this->m_results.push_back(result);
		}
	}

	/**
	 * Rimuove tutti i risultati della lista, chiamandone il distruttore
	 * per ripulire la memoria.
	 */
	void ResultCollector::reset() {
		// TODO Controllare
		while (!this->m_results.empty()) {
			delete (this->m_results.back());
			this->m_results.pop_back();
		}
	}

	/**
	 * Restituisce il numero di testcases attualmente contenuto
	 * nella lista di risultati.
	 * In caso di chiamata al metodo "reset", questo numero viene azzerato
	 * e viene persa la memoria dei testcase precedenti.
	 */
	unsigned int ResultCollector::getTestCaseNumber() {
		return this->m_results.size();
	}

	/**
	 * Restituisce una terna di valori (MIN, AVG, MAX) calcolati secondo una breve
	 * analisi statistica su tutti i testcases attualmente contenuti nella lista.
	 * I valori vengono estratti da ciascun risultato in funzione del parametro in
	 * ingresso.
	 */
	std::tuple<double, double, double> ResultCollector::getStat(ResultStat stat) {
		// Inizializzo le variabili
		double min = 1E20, sum = 0, max = 0;
		// Preparo la funzione estrattore
		auto getter = this->getStatGetter(stat);
		DEBUG_LOG("Ho appena estratto l'estrattore");
		// Scorro su tutti i risultati
		for (Result* result : this->m_results) {
			// Chiamo la funzione estrattore, estraendo la statistica dal risultato
			double current_value = getter(result);
			// Aggiorno i valori
			if (current_value < min) {
				min = current_value;
			}
			sum += current_value;
			if (current_value > max) {
				max = current_value;
			}
		}
		// Costruisco e restituisco la tupla
		return std::make_tuple(min, (sum / this->m_results.size()), max);
	}

	/**
	 * Restituisce la percentuale di successo dell'algoritmo ESC, confrontato
	 * sul campione di tutti i testcase disponibili.
	 */
	double ResultCollector::getSuccessPercentage() {
		int correct_result_counter = 0;
		for (Result* result : this->m_results) {
			if (*(result->sc_solution) == *(result->esc_solution)) {
				correct_result_counter++;
			}
		}
		return ((double)(correct_result_counter)) / this->m_results.size();
	}

	/**
	 * Presentazione di un singolo problema e delle sue soluzioni.
	 * Il contenuto in output dipende dalle impostazioni del programma.
	 */
	void ResultCollector::presentResult(Result* result) {
		DEBUG_MARK_PHASE("Presentazione dell'automa DFA originale") {
			DFADrawer drawer = DFADrawer(result->original_problem->getDFA());

			if (DO_PRINT_TRANSLATION) {
				std::cout << "TRANSLATION:\n";
				Alphabet computed_alpha = result->original_problem->getDFA()->getAlphabet();
				std::cout << result->original_problem->getTranslation()->toString(computed_alpha);
			}

			if (DO_PRINT_ORIGINAL_AUTOMATON) {
				std::cout << "ORIGINAL DFA:\n";
				std::cout << drawer.asString();
			}

			if (DO_DRAW_ORIGINAL_AUTOMATON) {
				// Stampa su file dell'automa originale
				string filename = "original.gv";
				drawer.asDotFile(filename);
				string command = "dot -Tpdf \"" + filename + "\" -o original.pdf";
				system(command.c_str());
			}
		}

		DEBUG_MARK_PHASE("Presentazione della soluzione ottenuta con SC") {
			DFADrawer sc_drawer = DFADrawer(result->sc_solution);

			if (DO_PRINT_SC_SOLUTION) {
				// [SC] Stampa in formato testuale
				std::cout << "SOLUZIONE di SC:\n";
				std::cout << std::endl << sc_drawer.asString() << std::endl;
			}

			if (DO_DRAW_SC_SOLUTION) {
				// [SC] Stampa su file
				string sc_filename = "sc_solution.gv";
				sc_drawer.asDotFile(sc_filename);
				string sc_command = "dot -Tpdf \"" + sc_filename + "\" -o sc_result.pdf";
				system(sc_command.c_str());
			}
		}

		DEBUG_MARK_PHASE("Presentazione della soluzione ottenuta con ESC") {
			DFADrawer esc_drawer = DFADrawer(result->esc_solution);

			if (DO_PRINT_ESC_SOLUTION) {
				// [ESC] Stampa in formato testuale
				std::cout << "SOLUZIONE di ESC:\n";
				std::cout << std::endl << esc_drawer.asString() << std::endl;
			}

			if (DO_DRAW_ESC_SOLUTION) {
				// [ESC] Stampa su file
				string esc_filename = "esc_solution.gv";
				esc_drawer.asDotFile(esc_filename);
				string esc_command = "dot -Tpdf \"" + esc_filename + "\" -o esc_result.pdf";
				system(esc_command.c_str());
			}
		}
	}

	/**
	 * Presentazione di tutti i risultati e delle statistiche.
	 */
	void ResultCollector::presentResults() {
		for (Result* result : this->m_results) {
			this->presentResult(result);
		}
		DEBUG_MARK_PHASE("Presentazione delle statistiche") {
		if (DO_PRINT_STATS) {
			printf("STATS:\n");
			printf("Based on %u testcases with automata of size %d and alphabet of cardinality %d.\n", this->getTestCaseNumber(), AUTOMATON_SIZE, ALPHABET_CARDINALITY);
			printf("ESC success percentage = %f %%\n", (100 * this->getSuccessPercentage()));
			printf("________________|    MIN    |    AVG    |    MAX    |\n");
			for (int int_stat = SC_TIME; int_stat <= SOL_GROWTH; int_stat++) {
				ResultStat stat = static_cast<ResultStat>(int_stat);
				// TODO Ricordarsi di aggiornare l'ultimo valore, in caso di aggiunta di statistiche
				tuple<double, double, double> stat_values = this->getStat(stat);
				printf(" %12s | %9.4f | %9.4f | %9.4f |\n",
						stat_headlines[stat].c_str(),
						std::get<0>(stat_values),
						std::get<1>(stat_values),
						std::get<2>(stat_values));
			}
		}}
	}

} /* namespace translated_automata */
