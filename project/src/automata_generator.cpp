/******************************************************************************
 * automata_generator.hpp
 *
 * Project: TranslatedAutomata
 *
 * Implementazione della classe AutomataGenerator, avente la responsabilità
 * di generare automi di differenti tipologie e caratteristiche, a seconda dei
 * parametri richiesti nella chiamata dei metodi.
 *
 ******************************************************************************/

#include "automata_generator.hpp"

#include <string>

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	template <class State>
	AutomataGenerator<State>::AutomataGenerator() {
		this->m_alphabet = generateAlphabet(letters, 26, default_alphabet_cardinality);
	}

	/**
	 * Costruisce un alfabeto con la cardinalità (=numero di simboli dell'alfabeto) fissata,
	 * partendo da una base di n caratteri passati come parametro.
	 * I simboli sono costruiti come combinazione e concatenazione dei caratteri, cominciando con le
	 * combinazioni di lunghezza inferiore.
	 */
	template <class State>
	Alphabet AutomataGenerator<State>::generateAlphabet(const char *chars, int chars_size, int cardinality) {
		// TODO
	}

	/**
	 * Permette di resettare lo spazio dei nomi utilizzati per gli stati di un automa.
	 * Se chiamato al termine della generazione di ogni automa, ha l'effetto di cominciare
	 * la generazione da zero durante la generazione successiva.
	 */
	template <class State>
	void AutomataGenerator<State>::resetNames() {
		// TODO
	}


	/**
	 * Costruisce un nome unico dall'ultimo reset.
	 * Il nome viene costruito concatenando una stringa con un numero incrementale,
	 * ed è utilizzato per identificare univocamente gli stati di un automa.
	 */
	template <class State>
	string AutomataGenerator<State>::generateUniqueName() {
		// TODO
	}

} /* namespace translated_automata */
