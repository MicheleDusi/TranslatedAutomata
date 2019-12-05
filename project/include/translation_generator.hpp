/******************************************************************************
 * translation_generator.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header della classe TranslationGenerator, avente la responsabilità
 * di generare traduzioni di alfabeti secondo i parametri preimpostati.
 *
 ******************************************************************************/

#ifndef INCLUDE_TRANSLATION_GENERATOR_HPP_
#define INCLUDE_TRANSLATION_GENERATOR_HPP_

#include "translation.hpp"

namespace translated_automata {

	class TranslationGenerator {

	private:
		double m_mixing_factor;
		double m_offset;
		double m_epsilon_percentage;

	public:
		static const double default_mixing_factor;
		static const double default_offset;
		static const double default_epsilon_percentage;

		TranslationGenerator();
		virtual ~TranslationGenerator();

		double getMixingFactor();
		double getOffset();
		double getEpsilonPercentage();
		void setMixingFactor(double mixing_factor);
		void setOffset(double offset);
		void setEpsilonPercentage(double epsilon_percentage);

		Translation* generateTranslation(Alphabet& domain);

	};

} /* namespace translated_automata */

#endif /* INCLUDE_TRANSLATION_GENERATOR_HPP_ */
