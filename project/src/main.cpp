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

#include "automata_generator_dfa.hpp"
#include "automaton_dfa.hpp"
#include "debug.hpp"
#include "embedded_subset_construction.hpp"
#include "translation.hpp"
#include "translation_generator.hpp"

#define ALPHABET_CARDINALITY 				3
#define AUTOMATON_SIZE 						5
#define AUTOMATON_FINAL_PROBABILITY 		.3
#define AUTOMATON_TRANSITION_PERCENTAGE 	.8
#define TRANSLATION_MIXING_FACTOR 			.7333
#define TRANSLATION_OFFSET 					13.25

using std::set;

using namespace translated_automata;

int main(int argc, char **argv) {

	DEBUG_MARK_PHASE( "Translated Automaton - Main" ) {

		// Generazione dell'automa
		DFAGenerator *gen = new DFAGenerator();
		Alphabet alpha = gen->generateAlphabet(DFAGenerator::letters, ALPHABET_CARDINALITY);
		gen->setAlphabet(alpha);
		gen->setSize(AUTOMATON_SIZE);
		gen->setFinalProbability(AUTOMATON_FINAL_PROBABILITY);
		gen->setTransitionPercentage(AUTOMATON_TRANSITION_PERCENTAGE);
		DFA dfa = gen->generateRandomAutomaton();

		// Stampa iniziale
		std::cout << dfa.toString();
		std::cout << "\n - - - - - \n";

		// Generazione della traduzione
		TranslationGenerator* t_gen = new TranslationGenerator();
		t_gen->setMixingFactor(TRANSLATION_MIXING_FACTOR);
		t_gen->setOffset(TRANSLATION_OFFSET);
		Translation tau = t_gen->generateTranslation(alpha);

		// Stampa della traduzione
		std::cout << tau.toString(alpha);
		std::cout << "\n - - - - - \n";

		DEBUG_MARK_PHASE( "Embedded Subset Construction" ) {

			// Algoritmo "Embedded Subset Construction
			EmbeddedSubsetConstruction *esc = new EmbeddedSubsetConstruction();

			DFA translated_dfa = esc->run(dfa, tau);

			// Stampa del risultato
			std::cout << translated_dfa.toString();

		}

	}

	return 0;
}
