/*
 * ProblemSolver.cpp
 *
 * Modulo che si occupa di risolvere problemi "ad alto livello".
 * I problemi vengono generati tramite un ProblemGenerator, vengono
 * risolti da i due algoritmi previsti all'interno del programma, e
 * i risultati vengono archiviati all'interno di un ResultCollector.
 */

#include "../include/ProblemSolver.hpp"

#include <cstdio>
#include <chrono>

#include "Debug.hpp"
#include "Properties.hpp"

namespace translated_automata {

	/**
	 * Macro function per concatenare la valutazione dei due argomenti.
	 */
	#define CONCAT( x, y ) _CONCAT( x, y )
	#define _CONCAT( x, y ) x ## y

	/**
	 * Macro function che si occupa di misurare il tempo impiegato per eseguire un blocco di codice.
	 * Memorizza il tempo in una variabile (dichiarata internamente alla macro) il cui nome può essere
	 * inserito come parametro.
	 */
	#define MEASURE_MILLISECONDS( ms_result ) 											\
		unsigned long int ms_result = 0; 												\
		auto CONCAT( ms_result, _start ) = chrono::high_resolution_clock::now(); 		\
		for (	int CONCAT( ms_result, _for_counter ) = 0; 								\
				CONCAT( ms_result, _for_counter ) < 1;									\
				CONCAT( ms_result, _for_counter++ ),									\
				ms_result = std::chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - CONCAT( ms_result, _start )).count() )

	/**
	 * Costruttore.
	 */
	ProblemSolver::ProblemSolver(Configurations* configurations) {
		// Creazione del generatore e dell'analizzatore di risultati
		this->generator = new ProblemGenerator(configurations);
		this->collector = new ResultCollector(configurations);

		this->sc = new SubsetConstruction();
		this->esc = new EmbeddedSubsetConstruction(configurations);
	}

	/**
	 * Distruttore.
	 * Distrugge i riferimenti alle due classi relative agli algoritmi da confrontare.
	 */
	ProblemSolver::~ProblemSolver() {
		DEBUG_MARK_PHASE("Eliminazione del risolutore") {
			delete this->generator;
			delete this->collector;
			delete this->sc;
			delete this->esc;
		}
	}

	/**
	 * Restituisce il collettore di risultati.
	 */
	ResultCollector* ProblemSolver::getResultCollector() {
		return this->collector;
	}

	/**
	 * Risolve un singolo problema di traduzione passato come parametro.
	 * La risoluzione avviene attraverso due algoritmi:
	 * - Subset Construction
	 * - Embedded Subset Construction
	 */
	void ProblemSolver::solve(TranslationProblem* problem) {
		DEBUG_ASSERT_NOT_NULL(problem);
		Result* result = new Result();
		result->original_problem = problem;

		DEBUG_MARK_PHASE("Subset Construction") {

			// Fase di traduzione
			NFA* nfa = problem->getTranslation()->translate(problem->getDFA());

			// Fase di costruzione
			MEASURE_MILLISECONDS( sc_time ) {
				result->sc_solution = this->sc->run(nfa); // Chiamata all'algoritmo
			}
			result->sc_elapsed_time = sc_time;
		}

		DEBUG_MARK_PHASE("Embedded Subset Construction") {

			// Fase di traduzione
			this->esc->runAutomatonTranslation(problem->getDFA(), problem->getTranslation()); // Chiamata all'algoritmo per la fase di traduzione

			// Fase di costruzione
			MEASURE_MILLISECONDS( esc_time ) {
				this->esc->runBudProcessing(); // Chiamata all'algoritmo per la fase di costruzione
			}
			result->esc_elapsed_time = esc_time;

			result->esc_solution = this->esc->getResult();
		}

		this->collector->addResult(result);
	}

	/**
	 * Risolve un singolo problema di determinizzazione passato come parametro.
	 * La risoluzione avviene attraverso due algoritmi:
	 * - Subset Construction
	 * - Embedded Subset Construction
	 */
	void ProblemSolver::solve(DeterminizationProblem* problem) {
		DEBUG_ASSERT_NOT_NULL(problem);
		Result* result = new Result();
		result->original_problem = problem;

		DEBUG_MARK_PHASE("Subset Construction") {

			// Fase di costruzione
			MEASURE_MILLISECONDS( sc_time ) {
				result->sc_solution = this->sc->run(problem->getNFA()); // Chiamata all'algoritmo
			}
			result->sc_elapsed_time = sc_time;
		}

		DEBUG_MARK_PHASE("Embedded Subset Construction") {

			// Fase di checkup + costruzione
			MEASURE_MILLISECONDS( esc_time ) {
				this->esc->runAutomatonCheckup(problem->getNFA());
				this->esc->runBudProcessing(); // Chiamata all'algoritmo per la fase di costruzione
			}
			result->esc_elapsed_time = esc_time;

			result->esc_solution = this->esc->getResult();
		}

		this->collector->addResult(result);
	}

	/**
	 * Risolve una singola istanza di un generico problema, agendo a seconda del tipo del problema.
	 * Nella pratica delega la risoluzione ai metodi specifici per la tipologia del problema.
	 */
	void ProblemSolver::solve(Problem* problem) {
		DEBUG_ASSERT_NOT_NULL(problem);
		switch (problem->getType()) {
		case Problem::TRANSLATION_PROBLEM :
			return this->solve((TranslationProblem*) problem);
		case Problem::DETERMINIZATION_PROBLEM :
			return this->solve((DeterminizationProblem*) problem);
		default :
			DEBUG_LOG_ERROR("Impossibile identificare il valore %d come istanza dell'enum ProblemType", problem->getType());
			throw "Valore sconosciuto per l'enum ProblemType";
		}
	}

	/**
	 * Risolver un singolo problema generato casualmente mediante
	 * il generatore passato come argomento al costruttore.
	 */
	void ProblemSolver::solve() {
		Problem* problem = this->generator->generate();
		DEBUG_ASSERT_NOT_NULL(problem);
		this->solve(problem);
	}

	#define BARWIDTH 70
	void printProgressBar(float progress) {
		std::cout << "[";
		int pos = BARWIDTH * progress;
		for (int i = 0; i < BARWIDTH; ++i) {
			if (i < pos) {
				std::cout << "=";
			}
			else if (i == pos) {
				std::cout << ">";
			}
			else {
				std::cout << " ";
			}
		}
		std::cout << "] " << int(progress * 100.0) << " %\r";
		std::cout.flush();
	}

	/**
	 * Risolve una sequenza di problemi generati casualmente.
	 */
	void ProblemSolver::solveSeries(unsigned int number) {
		DEBUG_MARK_PHASE("Risoluzione di una serie di problemi") {
		std::cout << "Solving " << std::to_string(number) << " problems...\n";
		printProgressBar(0);
		for (int i = 0; i < number; i++) {
			this->solve();
			printProgressBar(float(i+1) / number);
			DEBUG_LOG_SUCCESS("Risolto il problema (%d)!", (i+1));
		}
		std::cout << std::endl;
		}
	}

} /* namespace translated_automata */
