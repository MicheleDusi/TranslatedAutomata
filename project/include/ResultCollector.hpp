/*
 * ResultCollector.hpp
 *
 * Modulo che si occupa di archiviare i risultati di un problema.
 * Permette di effettuare semplici analisi statistiche su di essi, ottenendo
 * dati aggregati sulle performance del processo di risoluzione di un problema.
 */

#ifndef INCLUDE_RESULTCOLLECTOR_HPP_
#define INCLUDE_RESULTCOLLECTOR_HPP_

#include <functional>
#include <list>
#include <tuple>

#include "ProblemGenerator.hpp"

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
	 * Statistiche ottenibili da un (singolo!) risultato.
	 * Queste vengono poi processate e aggregate secondo alcune operazioni
	 * comuni come elemento minimo, media, elemento massimo, somma, ...
	 * NOTA: Ogni statistica dovrebbe essere esprimibile come valore <double>.
	 */
	enum ResultStat {
		SC_TIME,		// Tempo dedicato alla fase di costruzione nell'algoritmo SC (SC effettivo)
		ESC_TIME,		// Tempo dedicato alla fase di costruzione nell'algoritmo ESC (Esclusa la traduzione)
		SOL_SIZE,		// Dimensione della soluzione trovata dall'algoritmo
		SOL_GROWTH		// Rapporto fra la dimensione dell'automa della soluzione e l'automa originale
	};

	/**
	 * Classe che raccoglie i risultati e permette l'analisi
	 * di semplici statistiche.
	 */
	class ResultCollector {

	private:
		list<Result*> m_results;
		std::function<double(Result*)> getStatGetter(ResultStat stat);

	public:
		ResultCollector();
		virtual ~ResultCollector();

		// Gestione della lista di risultati
		void addResult(Result* result);
		void reset();

		// Statistiche
		unsigned int getTestCaseNumber();
		std::tuple<double, double, double> getStat(ResultStat stat);
		double getSuccessPercentage();
		void presentResult(Result* result);
		void presentResults();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_RESULTCOLLECTOR_HPP_ */
