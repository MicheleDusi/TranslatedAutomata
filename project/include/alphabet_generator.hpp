/*
 * alphabet_generator.hpp
 *
 * Offre la dichiarazione della classe AlphabetGenerator.
 */

#ifndef INCLUDE_ALPHABET_GENERATOR_HPP_
#define INCLUDE_ALPHABET_GENERATOR_HPP_

#include "alphabet.hpp"

namespace translated_automata {

	class AlphabetGenerator {

	private:
		const char *m_letters;
		unsigned int m_cardinality;

	public:
		static const char *default_letters;
		static const unsigned int default_cardinality;

		AlphabetGenerator();
		virtual ~AlphabetGenerator();

		void setLetters(char letters[]);
		void setCardinality(unsigned int cardinality);
		const char* getLetters();
		unsigned int getCardinality();

		Alphabet generate();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_ALPHABET_GENERATOR_HPP_ */
