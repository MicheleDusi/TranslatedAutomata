/*
 * problem_solver.cpp
 *
 * Modulo che si occupa di risolvere problemi e raccoglierne le statistiche.
 */

#include "problem_solver.hpp"

#include <cstdio>

#include "automata_drawer_impl.hpp"
#include "debug.hpp"

#define DO_PRINT_AUTOMATA false
#define DO_PRINT_RESULTS true

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	ProblemSolver::ProblemSolver(ProblemGenerator* generator) {
		DEBUG_ASSERT_NOT_NULL(generator);
		this->generator = generator;
		this->sc = new SubsetConstruction();
		this->esc = new EmbeddedSubsetConstruction();
	}

	/**
	 * Distruttore.
	 * Distrugge i riferimenti alle due classi relative agli algoritmi da confrontare.
	 */
	ProblemSolver::~ProblemSolver() {
		DEBUG_MARK_PHASE("Eliminazione del risolutore") {
			delete this->sc;
			delete this->esc;
		}
	}

	/**
	 * Risolve un singolo problema passato come parametro.
	 * La risoluzione avviene attraverso due algoritmi:
	 * - Traduzione + Subset Construction
	 * - Embedded Subset Construction
	 */
	void ProblemSolver::solve(Problem* problem) {
		if (DO_PRINT_AUTOMATA)
		DEBUG_MARK_PHASE("Stampa del problema") {
			std::cout << "PROBLEMA:\n";
			DFADrawer drawer = DFADrawer(problem->getDFA());
			std::cout << drawer.asString();
			Alphabet computed_alpha = problem->getDFA()->getAlphabet();
			std::cout << problem->getTranslation()->toString(computed_alpha);
			std::cout << "- - - - - - - - - - - - -\n";
		}

		DFA* sc_result;
		DFA* esc_result;

		DEBUG_MARK_PHASE("Subset Construction") {
			long int esc_start_time = time(NULL);
			// Fase di traduzione
			NFA* nfa = problem->getTranslation()->translate(problem->getDFA());
			// Fase di costruzione
			sc_result = this->sc->run(nfa);
			long int esc_end_time = time(NULL);

			if (DO_PRINT_RESULTS) {
				printf(" SC - time = %ld\n",(esc_end_time - esc_start_time));
			}
		}

		DEBUG_MARK_PHASE("Embedded Subset Construction") {
			long int esc_start_time = time(NULL);
			esc_result = this->esc->run(problem->getDFA(), problem->getTranslation());
			long int esc_end_time = time(NULL);

			if (DO_PRINT_RESULTS) {
				printf("ESC - time = %ld\n",(esc_end_time - esc_start_time));
			}
		}

		DEBUG_MARK_PHASE("Controllo finale") {
			if (*sc_result == *esc_result) {
				std::cout << "\033[32mFUNZIONA!\033[0m\n\n";
			} else {
				std::cout << "\033[31mQualcosa ancora non funziona...\033[0m\n\n";
			}
		}

		if (DO_PRINT_AUTOMATA)
		DEBUG_MARK_PHASE("Stampa della soluzione") {

			std::cout << "SOLUZIONE di SC:\n";
			DFADrawer sc_drawer = DFADrawer(sc_result);
			std::cout << std::endl << sc_drawer.asString() << std::endl;

			std::cout << "SOLUZIONE di ESC:\n";
			DFADrawer esc_drawer = DFADrawer(esc_result);
			std::cout << std::endl << esc_drawer.asString() << std::endl;
		}

	}

	/**
	 * Risolver un singolo problema generato casualmente mediante
	 * il generatore passato come argomento al costruttore.
	 */
	void ProblemSolver::solve() {
		Problem* problem = this->generator->generate();
		this->solve(problem);
	}

	/**
	 * Risolve una sequenza di problemi generati casualmente.
	 */
	void ProblemSolver::solveSeries(unsigned int number) {
		DEBUG_MARK_PHASE("Generazione di una serie di problemi")
		for (int i = 0; i < number; i++) {
			this->solve();
			DEBUG_LOG_SUCCESS("Risolto il problema (%d)!", (i+1));
		}
	}

} /* namespace translated_automata */
