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

	// Creazione delle configurazioni
	Configurations* config = new Configurations();
	config->load();

	// Creazione del sistema di risoluzione
	ProblemSolver* solver = new ProblemSolver(config);

	// Risoluzione effettiva
	solver->solveSeries(config->valueOf<int>(Testcases));

	// Presentazione delle statistiche risultanti
	solver->getResultCollector()->presentResults();

	delete solver;

	}

	return 0;
}
