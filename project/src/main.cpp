/******************************************************************************
 * main.cpp
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
 ******************************************************************************/

#include "main.hpp"

#include <iostream>
#include <tuple>

#include "alphabet_generator.hpp"
#include "automata_generator_dfa.hpp"
#include "automaton.hpp"
#include "debug.hpp"
#include "embedded_subset_construction.hpp"
#include "problem_generator.hpp"
#include "problem_solver.hpp"
#include "translation.hpp"
#include "translation_generator.hpp"

#define ALPHABET_CARDINALITY 				4
#define AUTOMATON_SIZE 						10
#define AUTOMATON_FINAL_PROBABILITY 		.3
#define AUTOMATON_TRANSITION_PERCENTAGE 	.5
#define TRANSLATION_MIXING_FACTOR 			.5
#define TRANSLATION_OFFSET 					3
#define TRANSLATION_EPSILON_PERCENTAGE		.25

using std::set;

using namespace translated_automata;

int main(int argc, char **argv) {

	DEBUG_MARK_PHASE( "Translated Automaton - Main" ) {

		// Creazione del problema
		ProblemGenerator* generator = new ProblemGenerator();
		generator->getDFAGenerator()->setSize(AUTOMATON_SIZE);
		generator->getDFAGenerator()->setFinalProbability(AUTOMATON_FINAL_PROBABILITY);
		generator->getDFAGenerator()->setTransitionPercentage(AUTOMATON_TRANSITION_PERCENTAGE);
		generator->getTranslationGenerator()->setMixingFactor(TRANSLATION_MIXING_FACTOR);
		generator->getTranslationGenerator()->setOffset(TRANSLATION_OFFSET);
		generator->getTranslationGenerator()->setEpsilonPercentage(TRANSLATION_EPSILON_PERCENTAGE);

		// Risoluzione del problema
		ProblemSolver* solver = new ProblemSolver(*generator);
		solver->solveSeries(1);

	}

	return 0;
}
