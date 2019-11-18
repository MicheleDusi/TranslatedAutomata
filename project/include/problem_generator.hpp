/******************************************************************************
 * problem_generator.hpp
 *
 * Project: TranslatedAutomata
 *
 * Firme e dichiarazioni della classe ProblemGenerator, avente la responsabilità
 * di generare istanze di problemi secondo differenti caratteristiche.
 * Questa classe funge da controller per le differenti classi generatrici delle
 * varie componenti di un'istanza di un problema.
 *
 ******************************************************************************/

#ifndef INCLUDE_PROBLEM_GENERATOR_HPP_
#define INCLUDE_PROBLEM_GENERATOR_HPP_

namespace translated_automata {

	class ProblemGenerator {

	public:
		ProblemGenerator();
		virtual ~ProblemGenerator();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_PROBLEM_GENERATOR_HPP_ */
