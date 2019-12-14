/*
 * ResultCollector.cpp
 *
 * Modulo che si occupa di archiviare i risultati di un problema.
 */

#include "ResultCollector.hpp"

#include "Debug.hpp"
#include "Properties.hpp"

namespace translated_automata {

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
				return (double) (result->sc_solution->size()) / AUTOMATON_SIZE;
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

//	/**
//	 * Restituisce il tempo medio impiegato per eseguire l'algoritmo
//	 * Subset Construction, calcolandolo su tutti i tempi dei testcase.
//	 */
//	double ResultCollector::getSCMeanTime() {
//		double sum = 0;
//		for (Result* result : this->m_results) {
//			sum += (double)(result->sc_elapsed_time);
//		}
//		return (sum / this->m_results.size());
//	}
//
//	/**
//	 * Restituisce il tempo medio impiegato per eseguire l'algoritmo
//	 * Embedded Subset Construction, calcolandolo su tutti i tempi dei testcase.
//	 */
//	double ResultCollector::getESCMeanTime() {
//		double sum = 0;
//		for (Result* result : this->m_results) {
//			sum += (double)(result->esc_elapsed_time);
//		}
//		return (sum / this->m_results.size());
//	}
//
//	/**
//	 * Restituisce il tempo massimo impiegato per eseguire l'algoritmo
//	 * Subset Construction, calcolandolo su tutti i tempi dei testcase.
//	 */
//	unsigned long int ResultCollector::getSCWorstTime() {
//		unsigned long int max = 0;
//		for (Result* result : this->m_results) {
//			if (result->sc_elapsed_time > max) {
//				max = result->sc_elapsed_time;
//			}
//		}
//		return max;
//	}
//
//	/**
//	 * Restituisce il tempo massimo impiegato per eseguire l'algoritmo
//	 * Embedded Subset Construction, calcolandolo su tutti i tempi dei testcase.
//	 */
//	unsigned long int ResultCollector::getESCWorstTime() {
//		unsigned long int max = 0;
//		for (Result* result : this->m_results) {
//			if (result->esc_elapsed_time > max) {
//				max = result->esc_elapsed_time;
//			}
//		}
//		return max;
//	}
//
//	/**
//	 * Restituisce il tempo minimo impiegato per eseguire l'algoritmo
//	 * Subset Construction, calcolandolo su tutti i tempi dei testcase.
//	 */
//	unsigned long int ResultCollector::getSCBestTime() {
//		unsigned long int min = 1E10;
//		for (Result* result : this->m_results) {
//			if (result->sc_elapsed_time < min) {
//				min = result->sc_elapsed_time;
//			}
//		}
//		return min;
//	}
//
//	/**
//	 * Restituisce il tempo minimo impiegato per eseguire l'algoritmo
//	 * Embedded Subset Construction, calcolandolo su tutti i tempi dei testcase.
//	 */
//	unsigned long int ResultCollector::getESCBestTime() {
//		unsigned long int min = 1E10;
//		for (Result* result : this->m_results) {
//			if (result->esc_elapsed_time < min) {
//				min = result->esc_elapsed_time;
//			}
//		}
//		return min;
//	}


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

} /* namespace translated_automata */
