/*
 * AutomataGenerator.cpp
 *
 * Project: TranslatedAutomata
 *
 * File header della classe AutomataGenerator, avente la responsabilità
 * di generare automi di differenti tipologie e caratteristiche, a seconda dei
 * parametri richiesti nella chiamata dei metodi.
 * La classe AutomataGenerator è parametrizzata sul tipo "Automaton"; il suo metodo
 * "generateRandomAutomaton" restituisce un automa generico, che deve essere specificato
 * nelle implementazioni concrete della classe.
 */

#ifndef INCLUDE_AUTOMATAGENERATOR_HPP_
#define INCLUDE_AUTOMATAGENERATOR_HPP_

#include <vector>
#include <string>

#include "Alphabet.hpp"
#include "Automaton.hpp"

#define UNDEFINED_VALUE -1

namespace translated_automata {

	enum AutomatonType {
		AUTOMATON_RANDOM,
		AUTOMATON_STRATIFIED,
		AUTOMATON_STRATIFIED_WITH_SAFE_ZONE
	};

	template <class Automaton>
	class AutomataGenerator {

	private:
		Alphabet m_alphabet;
		unsigned long int m_size;
		string m_name_prefix;
		double m_transition_percentage;
		double m_final_probability;
		double m_max_distance = UNDEFINED_VALUE;
		double m_safe_zone_distance = UNDEFINED_VALUE;

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
		unsigned long int computeDeterministicTransitionsNumber();

	public:
		AutomataGenerator(Alphabet alphabet);
		virtual ~AutomataGenerator();

		Alphabet getAlphabet();
		unsigned long int getSize();
		string getNamePrefix();
		double getTransitionPercentage();
		double getFinalProbability();
		unsigned int getMaxDistance();
		unsigned int getSafeZoneDistance();

		void setAlphabet(Alphabet alpha);
		void setSize(unsigned long int size);
		void setNamePrefix(string prefix);
		void setTransitionPercentage(double percentage);
		void setFinalProbability(double probability);
		void setMaxDistance(unsigned int max_distance);
		void setSafeZoneDistance(unsigned int safe_zone_distance);

		Automaton* generateAutomaton(AutomatonType type);
		virtual Automaton* generateRandomAutomaton();
		virtual Automaton* generateStratifiedAutomaton();
		virtual Automaton* generateStratifiedWithSafeZoneAutomaton();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATAGENERATOR_HPP_ */
