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

	public:
		static const double default_mixing_factor;
		static const double default_offset;

		TranslationGenerator();
		virtual ~TranslationGenerator();

		double getMixingFactor();
		double getOffset();
		void setMixingFactor(double mixing_factor);
		void setOffset(double offset);

		Translation* generateRandomTranslation(Alphabet domain);

	};

} /* namespace translated_automata */

#endif /* INCLUDE_TRANSLATION_GENERATOR_HPP_ */
