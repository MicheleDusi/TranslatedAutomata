/*
 * ProblemSolver.cpp
 *
 * Modulo che si occupa di risolvere problemi "ad alto livello".
 * I problemi vengono generati tramite un ProblemGenerator, vengono
 * risolti da i due algoritmi previsti all'interno del programma, e
 * i risultati vengono archiviati all'interno di un ResultCollector.
 */

#include "ProblemSolver.hpp"

#include <cstdio>
#include <chrono>

#include "AutomataDrawer_impl.hpp"
#include "Debug.hpp"

#define DO_PRINT_AUTOMATA false
#define DO_DRAW_AUTOMATA false
#define DO_PRINT_RESULTS false

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	ProblemSolver::ProblemSolver(ProblemGenerator* generator, ResultCollector* collector) {
		DEBUG_ASSERT_NOT_NULL(generator);
		DEBUG_ASSERT_NOT_NULL(collector);
		if (generator == NULL || collector == NULL) {
			std::cerr << "Impossibile istanziare un oggetto \"ProblemSolver\" con parametri del costruttore nulli" << std::endl;
		}

		this->generator = generator;
		this->collector = collector;

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
//		if (DO_PRINT_AUTOMATA || DO_DRAW_AUTOMATA) {
//		DEBUG_MARK_PHASE("Presentazione del problema") {
//
//			DFADrawer drawer = DFADrawer(problem->getDFA());
//
//			if (DO_PRINT_AUTOMATA) {
//				std::cout << "PROBLEMA:\n";
//				std::cout << drawer.asString();
//				Alphabet computed_alpha = problem->getDFA()->getAlphabet();
//				std::cout << problem->getTranslation()->toString(computed_alpha);
//				std::cout << "- - - - - - - - - - - - -\n";
//			}
//
//			if (DO_DRAW_AUTOMATA) {
//				// Stampa su file dell'automa originale
//				string filename = "original.gv";
//				drawer.asDotFile(filename);
//				string command = "dot -Tpdf \"" + filename + "\" -o original.pdf";
//				system(command.c_str());
//			}
//		}}

		Result* result = new Result();

		DEBUG_MARK_PHASE("Subset Construction") {
			auto sc_start_time = chrono::high_resolution_clock::now();

			// Fase di traduzione
			NFA* nfa = problem->getTranslation()->translate(problem->getDFA());
			// Fase di costruzione
			result->sc_solution = this->sc->run(nfa);

			auto sc_end_time = chrono::high_resolution_clock::now();
			auto sc_duration = (sc_end_time - sc_start_time);
			result->sc_elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(sc_duration).count();
		}

		DEBUG_MARK_PHASE("Embedded Subset Construction") {
			auto esc_start_time = chrono::high_resolution_clock::now();

			result->esc_solution = this->esc->run(problem->getDFA(), problem->getTranslation());

			auto esc_end_time = chrono::high_resolution_clock::now();
			auto esc_duration = (esc_end_time - esc_start_time);
			result->esc_elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(esc_duration).count();
		}

		this->collector->addResult(result);

//		if (DO_PRINT_AUTOMATA || DO_DRAW_AUTOMATA) {
//		DEBUG_MARK_PHASE("Stampa della soluzione") {
//
//			DFADrawer sc_drawer = DFADrawer(result->sc_solution);
//			DFADrawer esc_drawer = DFADrawer(result->esc_solution);
//
//			if (DO_PRINT_AUTOMATA) {
//				// [SC] Stampa in formato testuale
//				std::cout << "SOLUZIONE di SC:\n";
//				std::cout << std::endl << sc_drawer.asString() << std::endl;
//
//				// [ESC] Stampa in formato testuale
//				std::cout << "SOLUZIONE di ESC:\n";
//				std::cout << std::endl << esc_drawer.asString() << std::endl;
//			}
//
//			if (DO_DRAW_AUTOMATA) {
//				// [SC] Stampa su file
//				string sc_filename = "sc_solution.gv";
//				sc_drawer.asDotFile(sc_filename);
//				string sc_command = "dot -Tpdf \"" + sc_filename + "\" -o sc_result.pdf";
//				system(sc_command.c_str());
//
//				// [ESC] Stampa su file
//				string esc_filename = "esc_solution.gv";
//				esc_drawer.asDotFile(esc_filename);
//				string esc_command = "dot -Tpdf \"" + esc_filename + "\" -o esc_result.pdf";
//				system(esc_command.c_str());
//			}
//		}}

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
