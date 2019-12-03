/******************************************************************************
 * problem_generator.hpp
 *
 * Project: TranslatedAutomata
 *
 * Implementazione della classe ProblemGenerator, avente la responsabilità
 * di generare istanze di problemi secondo differenti caratteristiche.
 * Ogni problema prevede:
 * - Un alfabeto.
 * - Un DFA di partenza definito sull'alfabeto.
 * - Una traduzione sull'alfabeto.
 *
 ******************************************************************************/

#include "problem_generator.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "alphabet_generator.hpp"
#define DEBUG_MODE
#include "debug.hpp"

namespace translated_automata {

	/**
	 * Costruttore della "struttura" Problem.
	 * Richiede un automa DFA da tradurre e una traduzione
	 * sull'alfabeto.
	 */
	Problem::Problem(DFA& dfa, Translation& translation)
	: m_dfa(dfa), m_translation(translation) {}

	/**
	 * Distruttore.
	 */
	Problem::~Problem() {
		DEBUG_LOG("Distruzione dell'oggetto Problem");
	}

	/**
	 * Restituisce l'automa DFA del problema.
	 */
	DFA& Problem::getDFA() {
		return this->m_dfa;
	}

	/**
	 * Restituisce la traduzione del problema.
	 */
	Translation& Problem::getTranslation() {
		return this->m_translation;
	}

	/**
	 * Costruttore di un generatore di problemi.
	 * Si occupa di istanziare i generatori delegati.
	 * Inoltre, imposta alcuni parametri per la randomicità del programma.
	 */
	ProblemGenerator::ProblemGenerator() {
		// Istanzio un nuovo gestore di randomicità
		RandomnessManager* random = new RandomnessManager();

		// Impostazione dell'alfabeto comune
		AlphabetGenerator* alphabet_generator = new AlphabetGenerator();
		alphabet_generator->setCardinality(4); // FIXME
		this->m_alphabet = alphabet_generator->generate();
		delete alphabet_generator;

		// Istanzio i generatori delegati
		this->m_dfa_generator = new DFAGenerator(this->m_alphabet);
		this->m_translation_generator = new TranslationGenerator();
	}

	/**
	 * Distruttore.
	 */
	ProblemGenerator::~ProblemGenerator() {
		DEBUG_MARK_PHASE("Eliminazione dell'oggetto ProblemGenerator") {
			delete this->m_dfa_generator;
			delete this->m_translation_generator;
		}
	}

	/**
	 * Restituisce un puntatore al generatore di automi.
	 */
	DFAGenerator* ProblemGenerator::getDFAGenerator() {
		return this->m_dfa_generator;
	}

	/**
	 * Restituisce un puntatore al generatore di traduzioni.
	 */
	TranslationGenerator* ProblemGenerator::getTranslationGenerator() {
		return this->m_translation_generator;
	}

	/**
	 * Genera un nuovo problema, richiamando i generatori delegati.
	 */
	Problem ProblemGenerator::generate() {

		DFA automaton = this->m_dfa_generator->generateRandomAutomaton();
		Translation translation = this->m_translation_generator->generateTranslation(this->m_alphabet);

		Problem problem = Problem(automaton, translation);
		return problem;
	}

	/* Classe RandomnessManager */

	/**
	 * Costruttore.
	 * Genera un nuovo seme.
	 */
	RandomnessManager::RandomnessManager() {
		this->m_seed = 0;	// Inizializzazione fittizia, viene sovrascritto nella funzione successiva
		this->newSeed();
	}

	/**
	 * Distruttore
	 */
	RandomnessManager::~RandomnessManager() {};

	/**
	 * Genera un nuovo seme causale sulla base dell'istante in cui ci si trova.
	 */
	void RandomnessManager::newSeed() {
		this->m_seed = time(0);
		srand(this->m_seed);
		DEBUG_LOG("Impostazione di un nuovo seme casuale: %lu", this->m_seed);
	}

	/**
	 * Restituisce il seme attuale.
	 */
	unsigned long int RandomnessManager::getSeed() {
		return this->m_seed;
	}

	/**
	 * Imposta il valore passato come argomento come nuovo seme.
	 */
	void RandomnessManager::setSeed(unsigned long int new_seed) {
		this->m_seed = new_seed;
		srand(this->m_seed);
	}

	/**
	 * Stampa il seme attuale.
	 */
	void RandomnessManager::printSeed() {
		std::cout << "Seme attuale = " << this->m_seed << std::endl;
	}

} /* namespace translated_automata */
