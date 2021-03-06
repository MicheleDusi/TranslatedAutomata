/*
 * TranslationGenerator.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header della classe TranslationGenerator, avente la responsabilità
 * di generare traduzioni di alfabeti secondo i parametri preimpostati.
 *
 */

#ifndef INCLUDE_TRANSLATIONGENERATOR_HPP_
#define INCLUDE_TRANSLATIONGENERATOR_HPP_

#include "Configurations.hpp"
#include "Translation.hpp"

namespace translated_automata {

	class TranslationGenerator {

	private:
		double m_mixing_factor = default_mixing_factor;
		double m_offset = default_offset;
		double m_epsilon_percentage = default_epsilon_percentage;

	public:
		static const double default_mixing_factor;
		static const double default_offset;
		static const double default_epsilon_percentage;

		TranslationGenerator(Configurations* configurations);
		virtual ~TranslationGenerator();

		double getMixingFactor();
		double getOffset();
		double getEpsilonPercentage();

		Translation* generateTranslation(Alphabet& domain);

	};

} /* namespace translated_automata */

#endif /* INCLUDE_TRANSLATIONGENERATOR_HPP_ */
