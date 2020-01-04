/*
 * ResultCollector.cpp
 *
 * Modulo che si occupa di archiviare i risultati di un problema.
 */

#include "ResultCollector.hpp"

#include <fstream>

#include "AutomataDrawer_impl.hpp"
#include "Debug.hpp"
#include "Properties.hpp"

namespace translated_automata {

	// Stringhe per la visualizzazione delle statistiche
	vector<string> stat_headlines = vector<string> {
		"SC_TIME     [ms]",		// Tempo dedicato alla fase di costruzione nell'algoritmo SC (SC effettivo)
		"ESC_TIME    [ms]",		// Tempo dedicato alla fase di costruzione nell'algoritmo ESC
		"SOL_SIZE    [#] ",		// Dimensione della soluzione trovata dall'algoritmo
		"SOL_GROWTH  [%] "		// Rapporto fra la dimensione dell'automa della soluzione e l'automa originale
	};

	/**
	 * Costruttore.
	 */
	ResultCollector::ResultCollector(Configurations* configurations) {
		this->m_results = list<Result*>();
		this->m_config_reference = configurations;
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
		int aux_size = 0;
		std::function<double(Result*)> getter;
		switch(stat) {

		// Tempo di costruzione di SC
		case SC_TIME :
			getter = [](Result* result) {
				return (double) (result->sc_elapsed_time);
			};
			break;

		// Tempo di costruzione di ESC
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
			aux_size = this->m_config_reference->valueOf<unsigned int>(AutomatonSize);
			getter = [aux_size](Result* result) {
				return ((double) (result->sc_solution->size()) / aux_size) * 100;
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
		// Se voglio visualizzare solo i casi di errore
//		if (*(result->sc_solution) == *(result->esc_solution)) {
//			return;
//		}

		DEBUG_MARK_PHASE("Presentazione del problema di partenza") {

			switch (result->original_problem->getType()) {

			case Problem::TRANSLATION_PROBLEM : {
				TranslationProblem* translation_problem = (TranslationProblem*) result->original_problem;
				DFADrawer dfa_drawer = DFADrawer(translation_problem->getDFA());

				if (this->m_config_reference->valueOf<bool>(PrintTranslation)) {
					std::cout << "TRANSLATION:\n";
					Alphabet computed_alpha = translation_problem->getDFA()->getAlphabet();
					std::cout << translation_problem->getTranslation()->toString(computed_alpha);
				}

				if (this->m_config_reference->valueOf<bool>(PrintOriginalAutomaton)) {
					std::cout << "ORIGINAL DFA:\n";
					std::cout << dfa_drawer.asString();
				}

				if (this->m_config_reference->valueOf<bool>(DrawOriginalAutomaton)) {
					// Stampa su file dell'automa originale
					string filename = std::string(DIR_RESULTS) + FILE_NAME_ORIGINAL_AUTOMATON + FILE_EXTENSION_GRAPHVIZ;
					dfa_drawer.asDotFile(filename);
					string command = "dot -Tpdf \"" + filename + "\" -o " + DIR_RESULTS + FILE_NAME_ORIGINAL_AUTOMATON + FILE_EXTENSION_PDF;
					system(command.c_str());
				}
			}
				break;

			case Problem::DETERMINIZATION_PROBLEM : {
				DeterminizationProblem* determinization_problem = (DeterminizationProblem*) result->original_problem;
				NFADrawer nfa_drawer = NFADrawer(determinization_problem->getNFA());

				if (this->m_config_reference->valueOf<bool>(PrintOriginalAutomaton)) {
					std::cout << "ORIGINAL NFA:\n";
					std::cout << nfa_drawer.asString();
				}

				if (this->m_config_reference->valueOf<bool>(DrawOriginalAutomaton)) {
					// Stampa su file dell'automa originale
					string filename = std::string(DIR_RESULTS) + FILE_NAME_ORIGINAL_AUTOMATON + FILE_EXTENSION_GRAPHVIZ;
					nfa_drawer.asDotFile(filename);
					string command = "dot -Tpdf \"" + filename + "\" -o " + DIR_RESULTS + FILE_NAME_ORIGINAL_AUTOMATON + FILE_EXTENSION_PDF;
					system(command.c_str());
				}
			}
				break;

			default :
				DEBUG_LOG_ERROR("Impossibile interpretare il valore %d come appartenente all'enum ProblemType", result->original_problem->getType());
				break;
			}
		}

		DEBUG_MARK_PHASE("Presentazione della soluzione ottenuta con SC") {
			DFADrawer sc_drawer = DFADrawer(result->sc_solution);

			if (this->m_config_reference->valueOf<bool>(PrintSCSolution)) {
				// [SC] Stampa in formato testuale
				std::cout << "SOLUZIONE di SC:\n";
				std::cout << std::endl << sc_drawer.asString() << std::endl;
			}

			if (this->m_config_reference->valueOf<bool>(DrawSCSolution)) {
				// [SC] Stampa su file
				string sc_filename = std::string(DIR_RESULTS) + FILE_NAME_SC_SOLUTION + FILE_EXTENSION_GRAPHVIZ;
				sc_drawer.asDotFile(sc_filename);
				string sc_command = "dot -Tpdf \"" + sc_filename + "\" -o " + DIR_RESULTS + FILE_NAME_SC_SOLUTION + FILE_EXTENSION_PDF;
				system(sc_command.c_str());
			}
		}

		DEBUG_MARK_PHASE("Presentazione della soluzione ottenuta con ESC") {
			DFADrawer esc_drawer = DFADrawer(result->esc_solution);

			if (this->m_config_reference->valueOf<bool>(PrintESCSOlution)) {
				// [ESC] Stampa in formato testuale
				std::cout << "SOLUZIONE di ESC:\n";
				std::cout << std::endl << esc_drawer.asString() << std::endl;
			}

			if (this->m_config_reference->valueOf<bool>(DrawESCSOlution)) {
				// [ESC] Stampa su file
				string esc_filename = std::string(DIR_RESULTS) + FILE_NAME_ESC_SOLUTION + FILE_EXTENSION_GRAPHVIZ;
				esc_drawer.asDotFile(esc_filename);
				string esc_command = "dot -Tpdf \"" + esc_filename + "\" -o " + DIR_RESULTS + FILE_NAME_ESC_SOLUTION + FILE_EXTENSION_PDF;
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
		if (this->m_config_reference->valueOf<bool>(PrintStatistics)) {
			printf("STATS:\n");
			printf("Based on %u testcases with automata of size %d and alphabet of cardinality %d.\n",
					this->getTestCaseNumber(),
					this->m_config_reference->valueOf<int>(AutomatonSize),
					this->m_config_reference->valueOf<int>(AlphabetCardinality));
			printf("ESC success percentage = %f %%\n", (100 * this->getSuccessPercentage()));
			printf("__________________|    MIN    |    AVG    |    MAX    |\n");
			for (int int_stat = SC_TIME; int_stat <= SOL_GROWTH; int_stat++) {
				ResultStat stat = static_cast<ResultStat>(int_stat);
				// XXX Ricordarsi di aggiornare l'ultimo valore, in caso di aggiunta di statistiche
				tuple<double, double, double> stat_values = this->getStat(stat);
				printf(" %12s | %9.4f | %9.4f | %9.4f |\n",
						stat_headlines[stat].c_str(),
						std::get<0>(stat_values),
						std::get<1>(stat_values),
						std::get<2>(stat_values));
			}
		}}

		DEBUG_MARK_PHASE("Logging dei risultati aggregati") {

		// Scrittura su file dei risultati del blocco di testcase
		ofstream file_out("stats.txt", ios::app);


		file_out << this->m_config_reference->toString(Testcases) << " { ";
		// Stampo tutti i valori rilevanti per il blocco di test, in coda al file
		for (int param_enum = Testcases; param_enum <= ActiveDistanceCheckInTranslation; ++param_enum) {
			SettingID param_id = static_cast<SettingID>(param_enum);
			// XXX Ricordarsi di aggiornare l'ultimo valore, in caso di aggiunta di parametri
			if (this->m_config_reference->isTestParam(param_id)) {
				file_out << this->m_config_reference->toString(param_id) << " ";
			}
		}
		file_out << "} ";

		// Stampo i risultati
		tuple<double, double, double> sc_stat_values = this->getStat(SC_TIME);
			file_out << "SC( " 	<< std::to_string(std::get<0>(sc_stat_values)) << ", "
								<< std::to_string(std::get<1>(sc_stat_values)) << ", "
								<< std::to_string(std::get<2>(sc_stat_values)) << " ) ";

		tuple<double, double, double> esc_stat_values = this->getStat(ESC_TIME);
			file_out << "ESC( " << std::to_string(std::get<0>(esc_stat_values)) << ", "
								<< std::to_string(std::get<1>(esc_stat_values)) << ", "
								<< std::to_string(std::get<2>(esc_stat_values)) << " )";


		// Chiudo il file
		file_out << std::endl;
		file_out.close();
		}
	}

} /* namespace translated_automata */
