/*
 * AutomataGenerator.cpp
 *
 * Project: TranslatedAutomata
 *
 * Implementazione della classe AutomataGenerator, avente la responsabilità
 * di generare automi di differenti tipologie e caratteristiche, a seconda dei
 * parametri richiesti nella chiamata dei metodi.
 *
 */

#include "AutomataGenerator.hpp"

#include <string>
#include <cstring>
#include <vector>

#include "Debug.hpp"

namespace translated_automata {

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
	 * Poiché l'alfabeto viene generato di default da un'altra classe, è opportuno che la classe
	 * che si occupa della generazione dell'automa venga resa indipendente da essa.
	 * Per questo il costruttore accetta in ingresso un alfabeto già pronto.
	 * Questo può tuttavia cambiare con i metodi setter.
	 */
	template <class Automaton>
	AutomataGenerator<Automaton>::AutomataGenerator(Alphabet alphabet) {
		this->m_alphabet = alphabet;
		this->m_size = default_size;
		this->m_name_prefix	= default_name_prefix;
		this->m_transition_percentage = default_transition_percentage;
		this->m_final_probability = default_final_probability;
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
	 * Metodo protetto.
	 * Calcola il numero di transizioni *deterministiche* da creare all'interno dell'automa; si parla di transizioni deterministiche
	 * poiché si esclude la possibilità che uno stato abbia più di una transizione uscente con la stessa label. Per questo motivo, il
	 * numero massimo di transizioni deterministiche (su cui poi è calcolata la percentuale prevista) è dato dal numero di stati per il
	 * numero di possibili label previste dall'alfabeto.
	 *
	 * Più nello specifico, il calcolo è effettuato secondo il seguente algoritmo:
	 * - Viene calcolato il numero massimo possibile di transizioni. Tale numero corrisponde al numero massimo di transizioni
	 * 	 per ogni stato moltiplicate per il numero N degli stati. Il numero massimo di transizioni di un singolo stato è dato
	 * 	 dal numero L di label disponibili (ricordiamo che trattiamo solo le transizioni deterministiche).
	 * - Viene calcolata la percentuale richiesta su tale numero, arrotondata per difetto.
	 * - Se il numero risultante è inferiore al numero necessario per connettere tutti gli stati, viene alzato
	 *   fino al numero di stati - 1.
	 *
	 * Nota: nel caso di automi NON determinstici (NFA) è possibile inserire una percentuale superiore ad 1 per generare
	 * più di una transizione con la stessa label (si veda la documentazione della classe NFAGenerator).
	 */
	template <class Automaton>
	unsigned long int AutomataGenerator<Automaton>::computeDeterministicTransitionsNumber() {
		unsigned long int max_n_trans = (this->getSize()) * (this->getAlphabet().size());
		unsigned long int n = (unsigned long int) (max_n_trans * this->getTransitionPercentage());
		return (n < this->getSize() - 1) ? (this->getSize() - 1) : (n);
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
	 * Restituisce la massima distanza di uno stato all'interno dell'automa.
	 * Questo parametro è utilizzato solo per la generazione di automi stratificati.
	 */
	template <class Automaton>
	unsigned int AutomataGenerator<Automaton>::getMaxDistance() {
		return this->m_max_distance;
	}

	/**
	 * Restituisce la distanza entro cui non sono sicuramente presenti punti di non
	 * determinismo, nel caso di automi NFA a strati che prevedano tale possibilità.
	 */
	template <class Automaton>
	unsigned int AutomataGenerator<Automaton>::getSafeZoneDistance() {
		return this->m_safe_zone_distance;
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
	 * Precondizione: la probabilità deve essere superiore a 0.
	 * Può essere superiore ad 1 se siamo nel caso NFA.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::setTransitionPercentage(double percentage) {
		if (percentage >= 0) {
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
	 * Imposta la massima distanza di uno stato all'interno dell'automa da costruire.
	 * Questo parametro è utilizzato solamente nella generazione di automi stratificati,
	 * non di automi generici.
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::setMaxDistance(unsigned int max_distance) {
		this->m_max_distance = max_distance;
	}

	/**
	 * Imposta la distanza entro la quale viene garantito il determinismo dell'automa.
	 * Questo parametro è utilizzato solamente negli automi stratificati con SafeZone,
	 * che per natura sono NON deterministici (fuori dalla SZ).
	 */
	template <class Automaton>
	void AutomataGenerator<Automaton>::setSafeZoneDistance(unsigned int safe_zone_distance) {
		this->m_safe_zone_distance = safe_zone_distance;
	}

	/**
	 * Restituisce un automa della tipologia desiderata.
	 * In breve, questo metodo si occupa di delegare la creazione dell'automa
	 * agli altri metodi della classe, a seconda del valore del parametro richiesto.
	 * Gli altri metodi, poi, verranno implementati effettivamente nelle classi figlie.
	 *
	 * AutomatonGenerator fornisce un'implementazione di base dei metodi per la costruzione
	 * di automi di vario tipo, ma sono implementazioni fittizie che generano un'eccezione.
	 * Questo permette di capire immediatamente se una certa struttura di automa è disponibile
	 * per gli automi DFA o per gli automi NFA.
	 */
	template <class Automaton>
	Automaton* AutomataGenerator<Automaton>::generateAutomaton(AutomatonType type) {
		switch(type) {

		case AUTOMATON_RANDOM :
			return this->generateRandomAutomaton();

		case AUTOMATON_STRATIFIED :
			return this->generateStratifiedAutomaton();

		case AUTOMATON_STRATIFIED_WITH_SAFE_ZONE :
			return this->generateStratifiedWithSafeZoneAutomaton();

		default :
			DEBUG_LOG_ERROR("Valore %d non riconosciuto all'interno dell'enumerazione AutomatonType", type);
			return NULL;
		}
	}


	template <class Automaton>
	Automaton* AutomataGenerator<Automaton>::generateRandomAutomaton() {
		DEBUG_LOG_ERROR("Impossibile generare un automa di tipo \"Random\" per l'attuale tipologia di problema");
		throw "Impossibile generare un automa di tipo \"Random\" per l'attuale tipologia di problema";
	}

	template <class Automaton>
	Automaton* AutomataGenerator<Automaton>::generateStratifiedAutomaton() {
		DEBUG_LOG_ERROR("Impossibile generare un automa di tipo \"Stratified\" per l'attuale tipologia di problema");
		throw "Impossibile generare un automa di tipo \"Stratified\" per l'attuale tipologia di problema";
	}

	template <class Automaton>
	Automaton* AutomataGenerator<Automaton>::generateStratifiedWithSafeZoneAutomaton() {
		DEBUG_LOG_ERROR("Impossibile generare un automa di tipo \"StratifiedWithSafeZone\" per l'attuale tipologia di problema");
		throw "Impossibile generare un automa di tipo \"StratifiedWithSafeZone\" per l'attuale tipologia di problema";
	}

    /*************
     * Nota: essendo la classe AutomataGenerator parametrizzata sul tipo "Automaton",
     * è necessario comunicare al compilatore quali implementazioni verranno utilizzate.
     */
	template class AutomataGenerator<NFA>;
	template class AutomataGenerator<DFA>;

} /* namespace translated_automata */
