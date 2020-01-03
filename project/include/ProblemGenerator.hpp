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
#include "AutomataGeneratorNFA.hpp"
#include "TranslationGenerator.hpp"

namespace translated_automata {

	/**
	 * Struttura generica che definisce un problema all'interno del programma.
	 * Un problema può essere di due tipi, ciascuno rappresentato da una sottoclasse:
	 * - Un problema di determinizzazione.
	 * - Un problema di traduzione (che racchiude anche un aspetto di determinizzazione).
	 */
	class Problem {

	public:
		typedef enum {
			TRANSLATION_PROBLEM,
			DETERMINIZATION_PROBLEM
		} ProblemType;

	private:
		const ProblemType m_type;

	public:
		Problem(ProblemType type) : m_type(type) {};
		~Problem() {};

		ProblemType getType() { return this->m_type; };

	};

	/**
	 * Struttura che definisce un problema di traduzione per questo programma.
	 * Un problema di traduzione prevede in input:
	 * - un automa deterministico.
	 * - una traduzione sull'alfabeto di tale automa.
	 * La soluzione di un problema di traduzione che ci si aspetta di ottenere in ouput comprende:
	 * - un automa deterministico.
	 */
	class TranslationProblem : public Problem {

	private:
		DFA* m_dfa;
		Translation* m_translation;

	public:
		TranslationProblem(DFA* dfa, Translation* translation);
		~TranslationProblem();

		DFA* getDFA();
		Translation* getTranslation();
	};

	/**
	 * Struttura che definisce un problema di determinizzazione per questo programma.
	 * Un problema di traduzione prevede in input:
	 * - un automa non deterministico.
	 * La soluzione di un problema di traduzione che ci si aspetta di ottenere in ouput comprende:
	 * - un automa deterministico.
	 */
	class DeterminizationProblem : public Problem {

	private:
		NFA* m_nfa;

	public:
		DeterminizationProblem(NFA* nfa);
		~DeterminizationProblem();

		NFA* getNFA();
	};

	/**
	 * Classe che genera il problema, richiamando i seguenti generatori:
	 * - AlphabetGenerator (una sola volta, l'alfabeto è fisso).
	 * - DFAGenerator
	 * - TranslationGenerator
	 */
	class ProblemGenerator {

	private:
		Problem::ProblemType m_problem_type;
		Alphabet m_alphabet;
		DFAGenerator* m_dfa_generator;
		NFAGenerator* m_nfa_generator;
		TranslationGenerator* m_translation_generator;

	public:
		ProblemGenerator(Configurations* configurations);
		~ProblemGenerator();

		Problem* generate();
		TranslationProblem* generateTranslationProblem();
		DeterminizationProblem* generateDeterminizationProblem();

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
