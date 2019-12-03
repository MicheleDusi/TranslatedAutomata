/*
 * problem_solver.cpp
 *
 * Modulo che si occupa di risolvere problemi e raccoglierne le statistiche.
 */

#include "problem_solver.hpp"

#include <cstdio>

#include "automata_drawer_impl.hpp"
//#define DEBUG_MODE
#include "debug.hpp"

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	ProblemSolver::ProblemSolver(ProblemGenerator& generator) {
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
	 */
	void ProblemSolver::solve(Problem& problem) {
		DEBUG_MARK_PHASE("Stampa del problema") {
			std::cout << "PROBLEMA:\n";
			DFADrawer drawer = DFADrawer(problem.getDFA());
			std::cout << drawer.asString();
			Alphabet computed_alpha = problem.getDFA().getAlphabet();
			std::cout << problem.getTranslation().toString(computed_alpha);
			std::cout << "- - - - - - - - - - - - -\n";
		}

//		DFA sc_result;
		DFA esc_result;

		DEBUG_MARK_PHASE("Subset Construction") {
			/* TODO Manca la traduzione di AUTOMA->NFA per poi applicarci SC
			NFA* nfa = problem.translation.
			sc_result = this->sc->run(problem.getDFA(), problem.getTranslation());
			*/
		}
		DEBUG_MARK_PHASE("Embedded Subset Construction") {
			long int esc_start_time = time(NULL);
			esc_result = this->esc->run(problem.getDFA(), problem.getTranslation());
			long int esc_end_time = time(NULL);

			printf("\nEMBEDDED SUBSET CONSTRUCTION\nTempo impiegato = %ld\n",(esc_end_time - esc_start_time));
//			std::cout << "Risultato:\n";
//			std::cout << esc_result.toString();
		}

//		DEBUG_MARK_PHASE("Controllo finale") {
//			// TODO Confronto dei due automi
//		}

		DEBUG_MARK_PHASE("Stampa della soluzione") {
			std::cout << "SOLUZIONE di ESC:\n";
			DFADrawer drawer = DFADrawer(esc_result);
			std::cout << drawer.asString();

//			std::cout << "SOLUZIONE di SC:\n";
//			DFADrawer drawer = DFADrawer(sc_result);
//			std::cout << drawer.asString();
		}
	}

	/**
	 * Risolver un singolo problema generato casualmente mediante
	 * il generatore passato come argomento al costruttore.
	 */
	void ProblemSolver::solve() {
		Problem problem = this->generator.generate();
		this->solve(problem);
	}

	/**
	 * Risolve una sequenza di problemi generati casualmente.
	 */
	void ProblemSolver::solveSeries(unsigned int number) {
		for (int i = 0; i < number; i++) {
			this->solve();
			DEBUG_LOG_SUCCESS("Risolto il problema (%d)!", (i+1));
		}
	}

} /* namespace translated_automata */
