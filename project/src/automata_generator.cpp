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
#include <vector>

#include "debug.hpp"
#include "automaton_nfa.hpp"

namespace translated_automata {

	template <typename Automaton>
	const char* AutomataGenerator<Automaton>::letters = "abcdefghijklmnopqrstuvwxyz";

	template <typename Automaton>
	const unsigned int AutomataGenerator<Automaton>::default_alphabet_cardinality = 1U;

	template <typename Automaton>
	const unsigned long int AutomataGenerator<Automaton>::default_size = 2UL;

	template <typename Automaton>
	const char* AutomataGenerator<Automaton>::default_name_prefix = "s";

	template <typename Automaton>
	const double AutomataGenerator<Automaton>::default_transition_percentage = 0.5;

	template <typename Automaton>
	const double AutomataGenerator<Automaton>::default_final_percentage = 0.1;

	/**
	 * Costruttore.
	 */
	template <class Automaton>
	AutomataGenerator<Automaton>::AutomataGenerator() {
		this->m_alphabet = generateAlphabet("abc", 3, 5);
		this->m_size = default_size;
		this->m_name_prefix	= default_name_prefix;
		double m_transition_percentage = default_transition_percentage;
		double m_final_percentage = default_final_percentage;
	}

	/**
	 * Distruttore.
	 */
	template <class Automaton>
	AutomataGenerator<Automaton>::~AutomataGenerator() {
		this->m_alphabet.clear();
	}

	/**
	 * Metodo privato.
	 * Permette di resettare lo spazio dei nomi utilizzati per gli stati di un automa.
	 * Se chiamato al termine della generazione di ogni automa, ha l'effetto di cominciare
	 * la generazione da zero durante la generazione successiva.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::resetNames() {
		this->m_namesCounter = 0;
	}

	/**
	 * Metodo privato.
	 * Costruisce un nome unico dall'ultimo reset.
	 * Il nome viene costruito concatenando una stringa con un numero incrementale,
	 * ed è utilizzato per identificare univocamente gli stati di un automa.
	 */
	template <class Automaton>
	string AutomataGenerator<Automaton>::generateUniqueName() {
		// Inizializza la stringa con il numero incrementale
		string name = string(std::to_string(this->m_namesCounter++));
		// Inserisce il prefisso
//		name.insert(this->m_name_prefix, 0);
		return name;
	}

	/**
	 * Getter per l'alfabeto dell'automa da generare.
	 */
	template <class Automaton>
	Alphabet AutomataGenerator<Automaton>::getAlphabet() {
		return this->m_alphabet;
	}

	/**
	 * Getter per la dimensione dell'automa da generare.
	 */
	template <class Automaton>
	unsigned long int AutomataGenerator<Automaton>::getSize() {
		return this->m_size;
	}

	/**
	 * Getter per la stringa utilizzata come prefisso per i nomi
	 * degli stati dell'automa da generare.
	 */
	template <class Automaton>
	string AutomataGenerator<Automaton>::getNamePrefix() {
		return this->m_name_prefix;
	}

	/**
	 * Getter per la percentuale di transizioni nell'automa da generare.
	 */
	template <class Automaton>
	double AutomataGenerator<Automaton>::getTransitionPercentage() {
		return this->m_transition_percentage;
	}

	/**
	 * Getter per la percentuale di stati finali nell'automa da generare.
	 */
	template <class Automaton>
	double AutomataGenerator<Automaton>::getFinalPercentage() {
		return this->m_final_percentage;
	}

	/**
	 * Setter per l'alfabeto dell'automa da generare.
	 * Si richiede che l'alfabeto contenga almeno un simbolo.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::setAlphabet(Alphabet alpha) {
		if (alpha.empty()) {
			DEBUG_LOG_ERROR("Impossibile impostare un alfabeto vuoto per un automa");
			return;
		}
		this->m_alphabet = alpha;
	}

	/**
	 * Setter per la dimensione dell'automa da generare.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::setSize(unsigned long int size) {
		this->m_size = size;
	}

	/**
	 * Getter per la stringa utilizzata come prefisso per i nomi
	 * degli stati dell'automa da generare.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::setNamePrefix(string prefix) {
		if (prefix.empty()) {
			DEBUG_LOG_ERROR("Impossibile impostare una stringa vuota o nulla per i nomi degli stati di un automa");
			return;
		}
		this->m_name_prefix = prefix;
	}

	/**
	 * Setter per la percentuale di transizioni dell'automa da generare.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::setTransitionPercentage(double percentage) {
		this->m_transition_percentage;
	}

	/**
	 * Setter per la percentuale di stati finali dell'automa da generare.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::setFinalPercentage(double percentage) {
		this->m_final_percentage;
	}

	/**
	 * Costruisce un alfabeto con la cardinalità (=numero di simboli dell'alfabeto) fissata,
	 * partendo da una base di n caratteri passati come parametro.
	 * I simboli sono costruiti come combinazione e concatenazione dei caratteri, cominciando con le
	 * combinazioni di lunghezza inferiore.
	 */
	template <class Automaton>
	Alphabet AutomataGenerator<Automaton>::generateAlphabet(const char *chars, unsigned int chars_size, unsigned int cardinality) {
		std::vector<string> alpha;

		// Tutti i caratteri ripetuti 0 volte
		alpha.push_back("");

		// Inserisco tutti (o meno) i caratteri ripetuti 1 volta
		for (int i = 0; i < chars_size && i < cardinality; i++) {
			string label = alpha.at(0);
			label.insert(chars[i], 0);
			alpha.push_back(label);
		}

		DEBUG_LOG("Dimensione attuale dell'alfabeto: %lu", alpha.size());

		int exp = 1;
		// Finché la dimensione dell'alfabeto è inferiore alla cardinalità
		while (alpha.size() < cardinality) {

			int actual_size = alpha.size();
			int new_insertion_size = chars_size ^ (exp + 1);

			for (int i = actual_size - chars_size ^ exp;
					i < actual_size;
					i++) {

				// Estraggo la lista dell'ultimo insieme inserito
				string suffix = alpha.at(i);

				for (int l = 0; l < chars_size; l++) {
					string newlabel = (chars[l] + suffix);
					std::cout << newlabel << std::endl;
					alpha.push_back(newlabel);
				}
			}

			exp++;
		}

		return alpha;

	}

    /*************
     * Nota: essendo la classe AutomataGenerator parametrizzata sul tipo "Automaton",
     * è necessario comunicare al compilatore quali implementazioni verranno utilizzate.
     */
	template class AutomataGenerator<NFA>;

} /* namespace translated_automata */
