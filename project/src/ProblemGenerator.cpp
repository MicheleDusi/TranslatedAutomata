/*
 * ProblemGenerator.hpp
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
 */

#include "ProblemGenerator.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "AlphabetGenerator.hpp"
#include "Debug.hpp"
#include "Properties.hpp"

namespace translated_automata {

	/**
	 * Costruttore della "struttura" TranslationProblem.
	 * Richiede un automa DFA da tradurre e una traduzione sull'alfabeto.
	 */
	TranslationProblem::TranslationProblem(DFA* dfa, Translation* translation)
	: Problem(TRANSLATION_PROBLEM) {

		DEBUG_ASSERT_NOT_NULL(dfa);
		this->m_dfa = dfa;
		DEBUG_ASSERT_NOT_NULL(translation);
		this->m_translation = translation;
	}

	/**
	 * Distruttore.
	 * Un problema possiede i propri attributi; pertanto, quando viene eliminato,
	 * richiama il distruttore di tali attributi.
	 */
	TranslationProblem::~TranslationProblem() {
		delete this->m_dfa;
		delete this->m_translation;
	}

	/**
	 * Restituisce l'automa DFA del problema di traduzione.
	 */
	DFA* TranslationProblem::getDFA() {
		return this->m_dfa;
	}

	/**
	 * Restituisce la traduzione del problema di traduzione.
	 */
	Translation* TranslationProblem::getTranslation() {
		return this->m_translation;
	}


	/**
	 * Costruttore della "struttura" DeterminizationProblem.
	 * Richiede un automa NFA.
	 */
	DeterminizationProblem::DeterminizationProblem(NFA* nfa)
	: Problem(DETERMINIZATION_PROBLEM) {

		DEBUG_ASSERT_NOT_NULL(nfa);
		this->m_nfa = nfa;
	}

	/**
	 * Distruttore.
	 * Un problema possiede i propri attributi; pertanto, quando viene eliminato,
	 * richiama il distruttore di tali attributi.
	 */
	DeterminizationProblem::~DeterminizationProblem() {
		delete this->m_nfa;
	}

	/**
	 * Restituisce l'automa NFA del problema di determinizzazione.
	 */
	NFA* DeterminizationProblem::getNFA() {
		return this->m_nfa;
	}

	/**
	 * Costruttore di un generatore di problemi.
	 * Si occupa di istanziare i generatori delegati.
	 * Inoltre, imposta alcuni parametri per la randomicità del programma.
	 */
	ProblemGenerator::ProblemGenerator() {
		// Istanzio un nuovo gestore di randomicità
		RandomnessManager* random = new RandomnessManager();
//		random->setSeed(1576624929); // NOT SOLVED (alphacard 5, size 6, trans 0.3, mix 0.9, epsilon 0.5)
		random->printSeed();
		// Una volta terminato il setup dei semi randomici, posso eliminarlo.
		delete random;

		// Impostazione dell'alfabeto comune
		AlphabetGenerator* alphabet_generator = new AlphabetGenerator();
		alphabet_generator->setCardinality(ALPHABET_CARDINALITY);
		this->m_alphabet = alphabet_generator->generate();
		delete alphabet_generator;

		// Istanzio i generatori delegati
		this->m_nfa_generator = new NFAGenerator(this->m_alphabet);
		this->m_nfa_generator->setSize(AUTOMATON_SIZE);
		this->m_nfa_generator->setFinalProbability(AUTOMATON_FINAL_PROBABILITY);
		this->m_nfa_generator->setTransitionPercentage(AUTOMATON_TRANSITION_PERCENTAGE);
		this->m_nfa_generator->setMaxDistance(AUTOMATON_MAX_DISTANCE);
		this->m_nfa_generator->setSafeZoneDistance(AUTOMATON_SAFE_ZONE_DISTANCE);

		this->m_dfa_generator = new DFAGenerator(this->m_alphabet);
		this->m_dfa_generator->setSize(AUTOMATON_SIZE);
		this->m_dfa_generator->setFinalProbability(AUTOMATON_FINAL_PROBABILITY);
		this->m_dfa_generator->setTransitionPercentage(AUTOMATON_TRANSITION_PERCENTAGE);
		this->m_dfa_generator->setMaxDistance(AUTOMATON_MAX_DISTANCE);
		this->m_dfa_generator->setSafeZoneDistance(AUTOMATON_SAFE_ZONE_DISTANCE);

		this->m_translation_generator = new TranslationGenerator();
		this->m_translation_generator->setMixingFactor(TRANSLATION_MIXING_FACTOR);
		this->m_translation_generator->setOffset(TRANSLATION_OFFSET);
		this->m_translation_generator->setEpsilonPercentage(TRANSLATION_EPSILON_PERCENTAGE);
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
	 * Genera un nuovo problema del tipo specifico richiesto, richiamando il metodo apposito.
	 */
	Problem* ProblemGenerator::generate(ProblemType type) {
		switch (type) {
		case TRANSLATION_PROBLEM :
			return this->generateTranslationProblem();
		case DETERMINIZATION_PROBLEM :
			return this->generateDeterminizationProblem();
		default :
			DEBUG_LOG_ERROR("Valore %d non riconosciuto all'interno dell'enumerazione ProblemType", type);
			return NULL;
		}
	}

	/**
	 * Genera un problema di traduzione, richiamando i generatori delegati.
	 */
	TranslationProblem* ProblemGenerator::generateTranslationProblem() {
		DEBUG_LOG("Generazione dell'automa DFA");
		DFA* automaton = this->m_dfa_generator->generateAutomaton(AUTOMATON_TYPE);

		DEBUG_LOG("Generazione della traduzione");
		Translation* translation = this->m_translation_generator->generateTranslation(this->m_alphabet);

		return new TranslationProblem(automaton, translation);
	}

	/**
	 * Genera un problema di determinizzazione, richiamando i generatori delegati.
	 */
	DeterminizationProblem* ProblemGenerator::generateDeterminizationProblem() {
		DEBUG_LOG("Generazione dell'automa NFA");
		NFA* automaton = this->m_nfa_generator->generateAutomaton(AUTOMATON_TYPE);

		return new DeterminizationProblem(automaton);
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
