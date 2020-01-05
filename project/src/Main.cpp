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

#include "Automaton.hpp"
#include "Debug.hpp"
#include "ProblemSolver.hpp"
#include "Properties.hpp"

using std::set;

using namespace translated_automata;

int main(int argc, char **argv) {

	DEBUG_MARK_PHASE( "Translated Automaton - Main" ) {

	Configurations* config;
	DEBUG_MARK_PHASE("Caricamento delle configurazioni") {
		// Creazione delle configurazioni
		config = new Configurations();
		config->load();
	}

	do {
		// Visualizzazione della combinazione corrente di configurazioni
		std::cout << config->getValueString() << std::endl;

		// Creazione del sistema di risoluzione
		ProblemSolver solver = ProblemSolver(config);

		// Risoluzione effettiva
		solver.solveSeries(config->valueOf<int>(Testcases));

		// Presentazione delle statistiche risultanti
		solver.getResultCollector()->presentResults();
		std::cout << std::endl;

	} while (config->nextTestCase());

	}

	return 0;
}
