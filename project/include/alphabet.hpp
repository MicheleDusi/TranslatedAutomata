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
 * Questo header contiene anche la definizione della label epsilon, sebbene una label
 * epsilon non sia mai contenuta all'interno dell'alfabeto.
 *
 ******************************************************************************/

#ifndef INCLUDE_ALPHABET_HPP_
#define INCLUDE_ALPHABET_HPP_

#include <string>
#include <vector>

// Definizione della label espilon
#define EPSILON "ε"
#define EPSILON_PRINT "\033[96mε\033[0m"
#define SHOW( label ) ((label == EPSILON) ? (EPSILON_PRINT) : (label))

namespace translated_automata {

	/**
	 * Definizione dell'alfabeto come vettore di stringhe.
	 */
	using Alphabet = std::vector<std::string>;

}

#endif /* INCLUDE_ALPHABET_HPP_ */
