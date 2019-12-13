/*
 * ResultCollector.hpp
 *
 * Modulo che si occupa di archiviare i risultati di un problema.
 * Permette di effettuare semplici analisi statistiche su di essi, ottenendo
 * dati aggregati sulle performance del processo di risoluzione di un problema.
 */

#ifndef INCLUDE_RESULTCOLLECTOR_HPP_
#define INCLUDE_RESULTCOLLECTOR_HPP_

#include "ProblemGenerator.hpp"

#include <list>

namespace translated_automata {

	/**
	 * Struttura che rappresenta un singolo risultato ottenuto con la
	 * risoluzione di un singolo problema.
	 */
	struct Result {
		Problem* original_problem;
		DFA* sc_solution;
		DFA* esc_solution;
		unsigned long int sc_elapsed_time;
		unsigned long int esc_elapsed_time;
	};

	/**
	 * Classe che raccoglie i risultati e permette l'analisi
	 * di semplici statistiche.
	 */
	class ResultCollector {

	private:
		list<Result*> m_results;

	public:
		ResultCollector();
		virtual ~ResultCollector();

		// Gestione della lista di risultati
		void addResult(Result* result);
		void reset();

		// Statistiche
		unsigned int getTestCaseNumber();
		double getSCMeanTime();
		double getESCMeanTime();
		unsigned long int getSCMaxTime();
		unsigned long int getESCMaxTime();
		double getESCSuccessPercentage();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_RESULTCOLLECTOR_HPP_ */
