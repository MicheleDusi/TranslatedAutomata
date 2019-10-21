/******************************************************************************
 * alphabet.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header che contiene la definizione del concetto di Alfebeto.
 * Tale definizion è usata durante la generazione di un automa e nella classe "Translation",
 * più in tutte le classi che utilizzano quest'ultima.
 *
 * Un alfabeto è una collezione di "simboli". Ogni simbolo viene implementato,
 * in questo progetto, come stringa.
 * Questa soluzione permette una maggiore versatilità e non limita la cardinalità
 * dell'alfabeto al numero di lettere dell'alfabeto latino.
 *
 ******************************************************************************/

#ifndef INCLUDE_ALPHABET_HPP_
#define INCLUDE_ALPHABET_HPP_

#include <string>
#include <vector>

namespace translated_automata {

	/**
	 * Definizione dell'alfabeto come vettore di stringhe.
	 */
	using Alphabet = std::vector<std::string>;

}

#endif /* INCLUDE_ALPHABET_HPP_ */
