/*
 * ProblemGenerator.hpp
 *
 * Project: TranslatedAutomata
 *
 * Firme e dichiarazioni della classe ProblemGenerator, avente la responsabilità
 * di generare istanze di problemi secondo differenti caratteristiche.
 * Questa classe funge da controller per le differenti classi generatrici delle
 * varie componenti di un'istanza di un problema.
 *
 */

#ifndef INCLUDE_PROBLEMGENERATOR_HPP_
#define INCLUDE_PROBLEMGENERATOR_HPP_

#include "AutomataGeneratorDFA.hpp"
#include "Automaton.hpp"
#include "Translation.hpp"
#include "TranslationGenerator.hpp"

namespace translated_automata {

	/**
	 * Struttura che definisce un problema per questo programma.
	 * Un problema prevede inizialmente un automa e una traduzione sull'alfabeto
	 * di tale automa.
	 */
	class Problem {

	private:
		DFA* m_dfa;
		Translation* m_translation;

	public:
		Problem(DFA* dfa, Translation* translation);
		~Problem();

		DFA* getDFA();
		Translation* getTranslation();
	};

	/**
	 * Classe che genera il problema, richiamando i seguenti generatori:
	 * - AlphabetGenerator (una sola volta, l'alfabeto è fisso).
	 * - DFAGenerator
	 * - TranslationGenerator
	 */
	class ProblemGenerator {

	private:
		Alphabet m_alphabet;
		DFAGenerator* m_dfa_generator;
		TranslationGenerator* m_translation_generator;

	public:
		ProblemGenerator();
		~ProblemGenerator();

		DFAGenerator* getDFAGenerator();
		TranslationGenerator* getTranslationGenerator();

		Problem* generate();

	};

	/**
	 * Classe che si offre alcuni semplici metodi per la gestione delle funzioni casuali
	 * all'interno del programma.
	 */
	class RandomnessManager {

	private:
		unsigned long int m_seed;

	public:
		RandomnessManager();
		~RandomnessManager();

		void newSeed();
		unsigned long int getSeed();
		void setSeed(unsigned long int new_seed);
		void printSeed();
	};

} /* namespace translated_automata */

#endif /* INCLUDE_PROBLEMGENERATOR_HPP_ */
