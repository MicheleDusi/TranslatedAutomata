/*
 * Main.cpp
 *
 * Project: TranslatedAutomata
 *
 * Main del progetto. Gestisce il flusso di esecuzione in maniera
 * centralizzata, richiamando le funzioni necessarie.
 * Attualmente si occupa di generare un automa secondo i parametri impostati,
 * quindi genera una traduzione e infine esegue l'algoritmo "Embedded
 * Subset Construction".
 * Si vedano le rispettive classi per informazioni più dettagliate.
 *
 */

#include <iostream>
#include <tuple>

#include "../include/ProblemSolver.hpp"
#include "Automaton.hpp"
#include "Debug.hpp"
#include "Properties.hpp"

using std::set;

using namespace translated_automata;

int main(int argc, char **argv) {

	DEBUG_MARK_PHASE( "Translated Automaton - Main" ) {
		ProblemGenerator* generator;

		DEBUG_MARK_PHASE("Creazione dei generatori") {

		// Creazione del generatore di problemi
		generator = new ProblemGenerator();

		}

		DEBUG_MARK_PHASE("Risoluzione dei problemi") {

		// Risoluzione del problema
		ResultCollector* collector = new ResultCollector();
		ProblemSolver* solver = new ProblemSolver(generator, collector);
		solver->solveSeries(TESTCASES);

		// Presentazione delle statistiche risultanti
		collector->presentResults();

		delete generator;
		delete collector;
		delete solver;

		}

	}

	return 0;
}
