/*
 * alphabet_generator.cpp
 *
 * File che si occupa della generazione automatica di un alfabeto.
 * Per la generazione sono richiesti:
 * - L'insieme di caratteri da cui attingere per costruire le label.
 * - La cardinalità desiderata per l'alfabeto.
 */

#include "alphabet_generator.hpp"

#include <cstring>

#include "debug.hpp"

using std::vector;
using std::string;

namespace translated_automata {

	const char* AlphabetGenerator::default_letters = "abcdefghijklmnopqrstuvwxyz";
	const unsigned int AlphabetGenerator::default_cardinality = 10U;

	AlphabetGenerator::AlphabetGenerator() {
		this->m_letters = default_letters;
		this->m_cardinality = default_cardinality;
	}

	AlphabetGenerator::~AlphabetGenerator() {}

	void AlphabetGenerator::setLetters(char letters[]) {
		if (letters != NULL) {
			this->m_letters = letters;
		}
	}

	void AlphabetGenerator::setCardinality(unsigned int cardinality) {
		this->m_cardinality = cardinality;
	}

	const char* AlphabetGenerator::getLetters() {
		return this->m_letters;
	}

	unsigned int AlphabetGenerator::getCardinality() {
		return this->m_cardinality;
	}

	/**
	 * Costruisce un alfabeto con la cardinalità (=numero di simboli dell'alfabeto) fissata,
	 * partendo da una base di n caratteri passati come parametro.
	 * [!] Nota che questi caratteri vengono passati come array di char e necessitano del carattere terminale '\0'.
	 *
	 * I simboli dell'alfabeto sono costruiti come combinazione e concatenazione dei caratteri,
	 * cominciando con le combinazioni di lunghezza minore.
	 *
	 * Nota: come array di caratteri è possibile utilizzare l'array di caratteri "letters"
	 * contenente tutte e sole le 26 lettere minuscole dell'alfabeto inglese.
	 */
	Alphabet AlphabetGenerator::generate() {
		std::vector<std::vector<string>> symbols;

		// Inizializzo l'insieme delle stringhe di lunghezza zero
		// Questa procedura serve unicamente come passo base, ma la stringa nulla sarà rimossa
		// al termine della funzione poiché non è richiesto che nell'automa esistano epsilon transizioni.
		std::vector<string> zero_length_strings;
		zero_length_strings.push_back("");
		symbols.push_back(zero_length_strings);

		// Conteggia il numero di simboli inseriti nell'alfabeto (senza considerare la stringa vuota)
		int counter = 0;
		// Finché la dimensione dell'alfabeto è inferiore alla cardinalità
		while (counter < m_cardinality) {

			// Predispongo il vector che accoglierà le stringhe di uguale dimensione,
			// generate concatenando un qualunque carattere con un qualunque simbolo dell'insieme
			// di stringhe precedente.
			std::vector<string> same_length_strings;

			for (string prefix : symbols[symbols.size() - 1]) {

				for (int l = 0; l < strlen(m_letters) && counter < m_cardinality; l++, counter++) {
					string newlabel = (prefix + m_letters[l]);
					same_length_strings.push_back(newlabel);
				}
			}

			symbols.push_back(same_length_strings);
		}

		// Rimozione dell'insieme contenente la stringa vuota
		symbols.erase(symbols.begin());

		// Flattening del vettore di vettori di stringhe
		Alphabet alpha;
		for (std::vector<string> string_set : symbols) {
			for (string s : string_set)
			alpha.push_back(s);
		}

		DEBUG_ASSERT_TRUE(alpha.size() == m_cardinality);
		return alpha;
	}


} /* namespace translated_automata */
