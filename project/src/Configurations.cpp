/*
 * Configurations.cpp
 *
 * Implementazione della classe "Configurations", che memorizza e offre un'interfaccia
 * standard per le impostazioni con cui eseguire il programma.
 */

#include "Configurations.hpp"

#include "AutomataGenerator.hpp"
#include "Debug.hpp"
#include "ProblemGenerator.hpp"

namespace translated_automata {

// CLASSE "SettingValue"

	/**
	 * Costruttore con valore intero.
	 */
	SettingValue::SettingValue(int value) {
		DEBUG_LOG("Costruzione di un oggetto SettingValue con valore INT = %d", value);
		this->m_type = INT;
		this->m_value.integer = value;
	}

	/**
	 * Costruttore con valore reale.
	 */
	SettingValue::SettingValue(double value) {
		DEBUG_LOG("Costruzione di un oggetto SettingValue con valore DOUBLE = %f", value);
		this->m_type = DOUBLE;
		this->m_value.real = value;
	}

	/**
	 * Costruttore con valore booleano.
	 */
	SettingValue::SettingValue(bool value) {
		DEBUG_LOG("Costruzione di un oggetto SettingValue con valore BOOL = %d", value);
		this->m_type = BOOL;
		this->m_value.flag = value;
	}

	/**
	 * Restituisce il tipo del valore.
	 */
	SettingType SettingValue::getType() {
		return m_type;
	}

	Value SettingValue::getValue() {
		return m_value;
	}

	string SettingValue::getValueString() {
		switch (this->m_type) {
		case INT :
			return std::to_string(this->m_value.integer);
		case DOUBLE :
			return std::to_string(this->m_value.real);
		case BOOL :
			return std::to_string(this->m_value.flag);
		default :
			DEBUG_LOG_ERROR("Impossibile interpretare il valore %b", this->m_value.integer);
			return "null";
		}
	}

// CLASSE "Configurations"

	/**
	 * Costruttore.
	 */
	Configurations::Configurations() {
		this->m_settings = map<SettingID, SettingValue*>();
	}

	/**
	 * Distruttore.
	 */
	Configurations::~Configurations() {}

	/**
	 * Metodo privato che carica nelle impostazioni un singolo parametro.
	 */
	void Configurations::load(SettingID name, SettingValue* value) {
		this->m_settings.insert(std::make_pair(name, value));
	}

	/**
	 * Carica le configurazioni con cui eseguire il programma.
	 */
	void Configurations::load() {
		// Numero di Testcase
		load(Testcases, new SettingValue(							10));

		// Proprietà del problema
//		load(ProblemType, new SettingValue(TRANSLATION_PROBLEM));
		load(ProblemType, new SettingValue(Problem::DETERMINIZATION_PROBLEM));

		load(AlphabetCardinality, new SettingValue(					5));
		load(TranslationMixingFactor, new SettingValue(				0.9));
		load(TranslationOffset, new SettingValue(					1));
		load(EpsilonPercentage, new SettingValue(					0.05));

//		load(AutomatonType, new SettingValue(AUTOMATON_RANDOM));
//		load(AutomatonType, new SettingValue(AUTOMATON_STRATIFIED));
		load(AutomatonStructure, new SettingValue(AUTOMATON_STRATIFIED_WITH_SAFE_ZONE));
		load(AutomatonSize, new SettingValue(						100));
		load(AutomatonFinalProbability, new SettingValue(			0.1));
		load(AutomatonTransitionsPercentage, new SettingValue(		0.5));
		load(AutomatonMaxDistance, new SettingValue(				90));
		load(AutomatonSafeZoneDistance, new SettingValue(			80));

		// Moduli e funzionalità opzionali
		load(ActiveAutomatonPruning, new SettingValue(				true));		// In caso sia attivato, evita la formazione e la gestione dello stato con estensione vuota, tramite procedura Automaton Pruning
		load(ActiveRemovingLabel, new SettingValue(					true));		// In caso sia attivato, utilizza una label apposita per segnalare le epsilon-transizione, che deve essere rimossa durante la determinizzazione
		load(ActiveDistanceCheckInTranslation, new SettingValue(	false));	// In caso sia attivato, durante la traduzione genera dei Bud solamente se gli stati soddisfano una particolare condizione sulla distanza [FIXME è una condizione che genera bug]

		load(PrintStatistics, new SettingValue(						true));
		load(PrintTranslation, new SettingValue(					false));
		load(PrintOriginalAutomaton, new SettingValue(				false));
		load(PrintSCSolution, new SettingValue(						false));
		load(PrintESCSOlution, new SettingValue(					false));

		load(DrawOriginalAutomaton, new SettingValue(				false));
		load(DrawSCSolution, new SettingValue(						false));
		load(DrawESCSOlution, new SettingValue(						false));
	}

	string names[] = {
			"Number of testcases:                      ",
			"Problem type:                             ",
			"Alphabet cardinality:                     ",
			"Translation mixing factor:                ",
			"Translation offset:                       ",
			"Epsilong percentage:                      ",
			"Automaton's structure type:               ",
			"Automaton's size (#states):               ",
			"Automaton's final states probability:     ",
			"Automaton's transitions percentage:       ",
			"Automaton's max distance:                 ",
			"Automaton's safe-zone distance:           ",
			"Active \"automaton pruning\"                ",
			"Active \"removing label\":                  ",
			"Active \"distance check in translation\":   ",
			"Print statistics:                         ",
			"Print translation:                        ",
			"Print original automaton:                 ",
			"Print SC solution:                        ",
			"Print ESC solution:                       ",
			"Draw original automaton:                  ",
			"Draw SC solution:                         ",
			"Draw ESC solution:                        ",
	};

	/**
	 * Restituisce il nome del parametro di configurazione.
	 * Precondizione: il nome deve essere esistente, non deve eccedere l'indice
	 * dell'array in cui sono contenuti i nomi.
	 */
	string Configurations::nameOf(const SettingID& name) {
		return names[name];
	}

	/**
	 * Restituisce una stringa contenente NOME e VALORE associati al parametro
	 * passato in ingresso.
	 */
	string Configurations::toString(const SettingID& name) {
		return (nameOf(name) + this->m_settings.at(name)->getValueString());
	}


} /* namespace translated_automata */
