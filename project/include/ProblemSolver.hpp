/*
 * ProblemSolver.hpp
 *
 * Modulo che si occupa della risoluzione automatica di problemi.
 * Un problema è definito nel file "problem_generator.hpp" come:
 *  - Traduzione di un alfabeto.
 *  - Automa DFA da tradurre.
 */

#ifndef INCLUDE_PROBLEMSOLVER_HPP_
#define INCLUDE_PROBLEMSOLVER_HPP_

#include "embedded_subset_construction.hpp"
#include "problem_generator.hpp"
#include "ResultCollector.hpp"
#include "subset_construction.hpp"

namespace translated_automata {

	class ProblemSolver {
	private:
		ProblemGenerator* generator;		// Generatore di problemi
		EmbeddedSubsetConstruction* esc; 	// Algoritmo Embedded Subset Construction
		SubsetConstruction* sc;				// Algoritmo Subset Construction
		ResultCollector* collector;			// Archivio dei risultati delle risoluzioni dei problemi

	public:
		ProblemSolver(ProblemGenerator* generator, ResultCollector* collector);
		~ProblemSolver();

		void solve(Problem* problem);
		void solve();
		void solveSeries(unsigned int number);
	};

} /* namespace translated_automata */

#endif /* INCLUDE_PROBLEMSOLVER_HPP_ */
