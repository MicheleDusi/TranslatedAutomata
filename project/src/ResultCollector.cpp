/*
 * ResultCollector.cpp
 *
 * Modulo che si occupa di archiviare i risultati di un problema.
 */

#include "ResultCollector.hpp"

#include "Debug.hpp"

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
	 * Restituisce il tempo medio impiegato per eseguire l'algoritmo
	 * Subset Construction, calcolandolo su tutti i tempi dei testcase.
	 */
	double ResultCollector::getSCMeanTime() {
		double sum = 0;
		for (Result* result : this->m_results) {
			sum += (double)(result->sc_elapsed_time);
		}
		return (sum / this->m_results.size());
	}

	/**
	 * Restituisce il tempo medio impiegato per eseguire l'algoritmo
	 * Embedded Subset Construction, calcolandolo su tutti i tempi dei testcase.
	 */
	double ResultCollector::getESCMeanTime() {
		double sum = 0;
		for (Result* result : this->m_results) {
			sum += (double)(result->esc_elapsed_time);
		}
		return (sum / this->m_results.size());
	}

	/**
	 * Restituisce il tempo massimo impiegato per eseguire l'algoritmo
	 * Subset Construction, calcolandolo su tutti i tempi dei testcase.
	 */
	unsigned long int ResultCollector::getSCMaxTime() {
		unsigned long int max = 0;
		for (Result* result : this->m_results) {
			if (result->sc_elapsed_time > max) {
				max = result->sc_elapsed_time;
			}
		}
		return max;
	}

	/**
	 * Restituisce il tempo massimo impiegato per eseguire l'algoritmo
	 * Embedded Subset Construction, calcolandolo su tutti i tempi dei testcase.
	 */
	unsigned long int ResultCollector::getESCMaxTime() {
		unsigned long int max = 0;
		for (Result* result : this->m_results) {
			if (result->esc_elapsed_time > max) {
				max = result->esc_elapsed_time;
			}
		}
		return max;
	}

	/**
	 * Restituisce la percentuale di successo dell'algoritmo ESC, confrontato
	 * sul campione di tutti i testcase disponibili.
	 */
	double ResultCollector::getESCSuccessPercentage() {
		int correct_result_counter = 0;
		for (Result* result : this->m_results) {
			if (*(result->sc_solution) == *(result->esc_solution)) {
				correct_result_counter++;
			}
		}
		return ((double)(correct_result_counter)) / this->m_results.size();
	}

} /* namespace translated_automata */
