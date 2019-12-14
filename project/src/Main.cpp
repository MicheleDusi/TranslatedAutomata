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
		ProblemGenerator* generator;

		DEBUG_MARK_PHASE("Creazione dei generatori") {

		// Creazione del problema
		generator = new ProblemGenerator();
		generator->getDFAGenerator()->setSize(AUTOMATON_SIZE);
		generator->getDFAGenerator()->setFinalProbability(AUTOMATON_FINAL_PROBABILITY);
		generator->getDFAGenerator()->setTransitionPercentage(AUTOMATON_TRANSITION_PERCENTAGE);
		generator->getDFAGenerator()->setMaxDistance(AUTOMATON_MAX_DISTANCE);
		generator->getTranslationGenerator()->setMixingFactor(TRANSLATION_MIXING_FACTOR);
		generator->getTranslationGenerator()->setOffset(TRANSLATION_OFFSET);
		generator->getTranslationGenerator()->setEpsilonPercentage(TRANSLATION_EPSILON_PERCENTAGE);

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
