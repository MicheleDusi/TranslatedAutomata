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
#include <cstring>
#include <vector>

#include "debug.hpp"
#include "automaton_nfa.hpp"

namespace translated_automata {

	template <typename Automaton>
	const char* AutomataGenerator<Automaton>::letters = "abcdefghijklmnopqrstuvwxyz";

	template <typename Automaton>
	const unsigned int AutomataGenerator<Automaton>::default_alphabet_cardinality = 10U;

	template <typename Automaton>
	const unsigned long int AutomataGenerator<Automaton>::default_size = 2UL;

	template <typename Automaton>
	const char* AutomataGenerator<Automaton>::default_name_prefix = "s";

	template <typename Automaton>
	const double AutomataGenerator<Automaton>::default_transition_percentage = 0.5;

	template <typename Automaton>
	const double AutomataGenerator<Automaton>::default_final_probability = 0.1;

	/**
	 * Costruttore.
	 */
	template <class Automaton>
	AutomataGenerator<Automaton>::AutomataGenerator() {
		this->m_alphabet = generateAlphabet(letters, default_alphabet_cardinality);
		this->m_size = default_size;
		this->m_name_prefix	= default_name_prefix;
		double m_transition_percentage = default_transition_percentage;
		double m_final_probability = default_final_probability;

		// Generazione del seme casuale
		srand(time(0));
	}

	/**
	 * Distruttore.
	 */
	template <class Automaton>
	AutomataGenerator<Automaton>::~AutomataGenerator() {
		this->m_alphabet.clear();
	}

	/**
	 * Metodo protetto.
	 * Permette di resettare lo spazio dei nomi utilizzati per gli stati di un automa.
	 * Se chiamato al termine della generazione di ogni automa, ha l'effetto di cominciare
	 * la generazione da zero durante la generazione successiva.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::resetNames() {
		this->m_namesCounter = 0;
	}

	/**
	 * Metodo protetto.
	 * Costruisce un nome unico dall'ultimo reset.
	 * Il nome viene costruito concatenando una stringa con un numero incrementale,
	 * ed è utilizzato per identificare univocamente gli stati di un automa.
	 */
	template <class Automaton>
	string AutomataGenerator<Automaton>::generateUniqueName() {
		// Inizializza la stringa con il prefisso e il numero incrementale
		string name = this->m_name_prefix + string(std::to_string(this->m_namesCounter++));
		// Restituisce il risultato
		return name;
	}

	/**
	 * Metodo protetto.
	 * Restituisce un numero in virgola mobile, a precisione doppia, compreso fra 0 e 1.
	 * Può essere utilizzato per operare delle scelte confrontandolo con una probabilità predefinita.
	 */
	template <class Automaton>
	double AutomataGenerator<Automaton>::generateNormalizedDouble() {
		return (static_cast <double> (rand()) / static_cast <double> (RAND_MAX));
	}

	/**
	 * Metodo protetto.
	 * Restituisce una label casuale scelta all'interno delle label
	 * dell'alfabeto impostato per la generazione di automi.
	 */
	template <class Automaton>
	string AutomataGenerator<Automaton>::getRandomLabelFromAlphabet() {
		return (m_alphabet[rand() % m_alphabet.size()]);
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
	 * Getter per la probabilità per uno stato di essere stato finale nell'automa da generare.
	 */
	template <class Automaton>
	double AutomataGenerator<Automaton>::getFinalProbability() {
		return this->m_final_probability;
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
	 * Setter per la probabilità di avere una transizione tra due stati nell'automa da generare.
	 * Questo metodo funziona solamente se la percentuale è effettivamente compresa nell'intervallo
	 * chiuso [0,1], altrimenti la sua esecuzione non ha alcun effetto.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::setTransitionPercentage(double percentage) {
		if (percentage >= 0 && percentage <= 1) {
			this->m_transition_percentage = percentage;
		}
	}

	/**
	 * Setter per la probabilità di uno stato di essere stato finale nell'automa da generare.
	 * Questo metodo funziona solamente se la probabilità è effettivamente compresa nell'intervallo
	 * chiuso [0,1], altrimenti la sua esecuzione non ha alcun effetto.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::setFinalProbability(double probability) {
		if (probability >= 0 && probability <= 1) {
			this->m_final_probability = probability;
		}
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
	template <class Automaton>
	Alphabet AutomataGenerator<Automaton>::generateAlphabet(const char *chars, unsigned int cardinality) {
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
		while (counter < cardinality) {

			// Predispongo il vector che accoglierà le stringhe di uguale dimensione,
			// generate concatenando un qualunque carattere con un qualunque simbolo dell'insieme
			// di stringhe precedente.
			std::vector<string> same_length_strings;

			for (string prefix : symbols[symbols.size() - 1]) {

				for (int l = 0; l < strlen(chars) && counter < cardinality; l++, counter++) {
					string newlabel = (prefix + chars[l]);
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

		DEBUG_ASSERT_TRUE(alpha.size() == cardinality);
		return alpha;
	}

    /*************
     * Nota: essendo la classe AutomataGenerator parametrizzata sul tipo "Automaton",
     * è necessario comunicare al compilatore quali implementazioni verranno utilizzate.
     */
	template class AutomataGenerator<NFA>;

} /* namespace translated_automata */
