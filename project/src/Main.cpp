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

#define TESTCASES							50

#define ALPHABET_CARDINALITY 				10

#define AUTOMATON_SIZE 						50
#define AUTOMATON_FINAL_PROBABILITY 		.1
#define AUTOMATON_TRANSITION_PERCENTAGE 	0.3
#define AUTOMATON_MAX_DISTANCE			 	5

#define TRANSLATION_MIXING_FACTOR 			0.5
#define TRANSLATION_OFFSET 					1
#define TRANSLATION_EPSILON_PERCENTAGE		0

using std::set;

using namespace translated_automata;

int main(int argc, char **argv) {

	DEBUG_MARK_PHASE( "Translated Automaton - Main" ) {
		ProblemGenerator* generator;

		DEBUG_MARK_PHASE("Creazione dei generatori") {

		// Creazione del problema
		generator = new ProblemGenerator(ALPHABET_CARDINALITY);
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
		printf("Based on %u testcases.\n", collector->getTestCaseNumber());
		printf("ESC success percentage = %f %%\n", (100 * collector->getESCSuccessPercentage()));
		printf("SC mean time = %f ms\n", collector->getSCMeanTime());
		printf("ESC mean time = %f ms\n", collector->getESCMeanTime());
		printf("SC worst time = %lu ms\n", collector->getSCMaxTime());
		printf("ESC worst time = %lu ms\n", collector->getESCMaxTime());

		delete generator;
		delete collector;
		delete solver;

		}

	}

	return 0;
}
