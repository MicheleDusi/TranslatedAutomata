/*
 * AutomataGenerator.cpp
 *
 * Project: TranslatedAutomata
 *
 * File header della classe AutomataGenerator, avente la responsabilità
 * di generare automi di differenti tipologie e caratteristiche, a seconda dei
 * parametri richiesti nella chiamata dei metodi.
 * La classe AutomataGenerator è parametrizzata sul tipo "State"; il suo metodo
 * "generateRandomAutomaton" restituisce un automa parametrizzato sullo stesso tipo.
 * Il metodo, così come il costruttore, sono virtuali; questo fa della classe una
 * classe astratta che necessita di un'implementazione concreta per essere usata.
 *
 */

#ifndef INCLUDE_AUTOMATAGENERATOR_HPP_
#define INCLUDE_AUTOMATAGENERATOR_HPP_

#include <vector>
#include <string>

#include "Alphabet.hpp"
#include "Automaton.hpp"

namespace translated_automata {

	template <class Automaton>
	class AutomataGenerator {

	private:
		Alphabet m_alphabet;
		unsigned long int m_size;
		string m_name_prefix;
		double m_transition_percentage;
		double m_final_probability;
		double m_max_distance = 0;

		unsigned int m_namesCounter = 0;

	protected:
		static const unsigned long int default_size;
		static const char *default_name_prefix;
		static const double default_transition_percentage;
		static const double default_final_probability;

		void resetNames();
		string generateUniqueName();
		double generateNormalizedDouble();
		string getRandomLabelFromAlphabet();

	public:

		AutomataGenerator(Alphabet alphabet);
		virtual ~AutomataGenerator();

		Alphabet getAlphabet();
		unsigned long int getSize();
		string getNamePrefix();
		double getTransitionPercentage();
		double getFinalProbability();
		unsigned int getMaxDistance();

		void setAlphabet(Alphabet alpha);
		void setSize(unsigned long int size);
		void setNamePrefix(string prefix);
		void setTransitionPercentage(double percentage);
		void setFinalProbability(double probability);
		void setMaxDistance(unsigned int max_distance);

		virtual Automaton* generateRandomAutomaton() = 0;

	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATAGENERATOR_HPP_ */
