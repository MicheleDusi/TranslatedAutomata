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

		// Presentazione dei risultati
		printf("RESULTS:\n");
		printf("Based on %u testcases with automata of size %d and alphabet of cardinality %d.\n", collector->getTestCaseNumber(), AUTOMATON_SIZE, ALPHABET_CARDINALITY);
		printf("ESC success percentage = %f %%\n", (100 * collector->getSuccessPercentage()));
		printf("________________|    MIN    |    AVG    |    MAX    |\n");
		tuple<double, double, double> sc_time = collector->getStat(SC_TIME);
		printf(" SC TIME   (ms) | %9.4f | %9.4f | %9.4f |\n", std::get<0>(sc_time), std::get<1>(sc_time), std::get<2>(sc_time));
		tuple<double, double, double> esc_time = collector->getStat(ESC_TIME);
		printf(" ESC TIME  (ms) | %9.4f | %9.4f | %9.4f |\n", std::get<0>(esc_time), std::get<1>(esc_time), std::get<2>(esc_time));
		tuple<double, double, double> sol_size = collector->getStat(SOL_SIZE);
		printf(" SOL SIZE   (#) | %9.4f | %9.4f | %9.4f |\n", std::get<0>(sol_size), std::get<1>(sol_size), std::get<2>(sol_size));
		tuple<double, double, double> sol_growth = collector->getStat(SOL_GROWTH);
		printf(" SOL GROWTH (%%) | %9.4f | %9.4f | %9.4f |\n", std::get<0>(sol_growth) * 100., std::get<1>(sol_growth) * 100., std::get<2>(sol_growth) * 100.);

		delete generator;
		delete collector;
		delete solver;

		}

	}

	return 0;
}
