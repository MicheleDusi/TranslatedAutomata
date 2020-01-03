/*
 * ProblemSolver.hpp
 *
 * Modulo che si occupa della risoluzione automatica di problemi.
 * I problemi possono essere somministrati dall'esterno o generati internamente.
 */

#ifndef INCLUDE_PROBLEMSOLVER_HPP_
#define INCLUDE_PROBLEMSOLVER_HPP_

#include "EmbeddedSubsetConstruction.hpp"
#include "ProblemGenerator.hpp"
#include "ResultCollector.hpp"
#include "SubsetConstruction.hpp"

namespace translated_automata {

	class ProblemSolver {
	private:
		ProblemGenerator* generator;		// Generatore di problemi
		ResultCollector* collector;			// Archivio dei risultati delle risoluzioni dei problemi
		EmbeddedSubsetConstruction* esc; 	// Algoritmo Embedded Subset Construction
		SubsetConstruction* sc;				// Algoritmo Subset Construction

	public:
		ProblemSolver(Configurations* configurations);
		~ProblemSolver();

		ResultCollector* getResultCollector();

		void solve(Problem* problem);
		void solve(TranslationProblem* problem);
		void solve(DeterminizationProblem* problem);
		void solve();
		void solveSeries(unsigned int number);
	};

} /* namespace translated_automata */

#endif /* INCLUDE_PROBLEMSOLVER_HPP_ */
