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
	 * Costruttore.
	 */
	SettingValue::SettingValue(SettingType type) {
		DEBUG_LOG("Costruzione di un oggetto SettingValue con type %d", type);
		this->m_type = type;
	}

	/**
	 * Restituisce il tipo del valore.
	 */
	SettingType SettingValue::getType() {
		return m_type;
	}

// CLASSE "IntegerSettingValue"

	/**
	 * Costruttore.
	 */
	IntSettingValue::IntSettingValue(int value)
	: SettingValue(INT) {
		this->m_value = value;
	}

	/**
	 * Metodo overridden.
	 * Restituisce il valore contenuto all'interno, castato con il tipo previsto.
	 */
	int IntSettingValue::getIntValue() {
		return this->m_value;
	}

// CLASSE "DoubleSettingValue"

	/**
	 * Costruttore.
	 */
	DoubleSettingValue::DoubleSettingValue(double value)
	: SettingValue(DOUBLE) {
		this->m_value = value;
	}

	/**
	 * Metodo overridden.
	 * Restituisce il valore contenuto all'interno, castato con il tipo previsto.
	 */
	double DoubleSettingValue::getDoubleValue() {
		return this->m_value;
	}

// CLASSE "BoolSettingValue"

	/**
	 * Costruttore.
	 */
	BoolSettingValue::BoolSettingValue(bool value)
	: SettingValue(BOOLEAN) {
		this->m_value = value;
	}

	/**
	 * Metodo overridden.
	 * Restituisce il valore contenuto all'interno, castato con il tipo previsto.
	 */
	bool BoolSettingValue::getBoolValue() {
		return this->m_value;
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
		load(Testcases, new IntSettingValue(							10));

		// Proprietà del problema
//		load(ProblemType, new IntSettingValue(TRANSLATION_PROBLEM));
		load(ProblemType, new IntSettingValue(Problem::DETERMINIZATION_PROBLEM));

		load(AlphabetCardinality, new IntSettingValue(					5));
		load(TranslationMixingFactor, new DoubleSettingValue(			0.9));
		load(TranslationOffset, new DoubleSettingValue(					1));
		load(EpsilonPercentage, new DoubleSettingValue(					0.05));

//		load(AutomatonType, new IntSettingValue(AUTOMATON_RANDOM));
//		load(AutomatonType, new IntSettingValue(AUTOMATON_STRATIFIED));
		load(AutomatonStructure, new IntSettingValue(AUTOMATON_STRATIFIED_WITH_SAFE_ZONE));
		load(AutomatonSize, new IntSettingValue(						100));
		load(AutomatonFinalProbability, new DoubleSettingValue(			0.1));
		load(AutomatonTransitionsPercentage, new DoubleSettingValue(	0.5));
		load(AutomatonMaxDistance, new IntSettingValue(					90));
		load(AutomatonSafeZoneDistance, new IntSettingValue(			80));

		// Moduli e funzionalità opzionali
		load(ActiveAutomatonPruning, new BoolSettingValue(				true));		// In caso sia attivato, evita la formazione e la gestione dello stato con estensione vuota, tramite procedura Automaton Pruning
		load(ActiveRemovingLabel, new BoolSettingValue(					true));		// In caso sia attivato, utilizza una label apposita per segnalare le epsilon-transizione, che deve essere rimossa durante la determinizzazione
		load(ActiveDistanceCheckInTranslation, new BoolSettingValue(	false));	// In caso sia attivato, durante la traduzione genera dei Bud solamente se gli stati soddisfano una particolare condizione sulla distanza [FIXME è una condizione che genera bug]

		load(PrintStatistics, new BoolSettingValue(						true));
		load(PrintTranslation, new BoolSettingValue(					false));
		load(PrintOriginalAutomaton, new BoolSettingValue(				false));
		load(PrintSCSolution, new BoolSettingValue(						false));
		load(PrintESCSOlution, new BoolSettingValue(					false));

		load(DrawOriginalAutomaton, new BoolSettingValue(				false));
		load(DrawSCSolution, new BoolSettingValue(						false));
		load(DrawESCSOlution, new BoolSettingValue(						false));
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
			"Print ESC sOlution:                       ",
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
		string str = nameOf(name);
		switch (name) {

		case Testcases :
			return str + std::to_string(this->valueOf<int>(name));
		case ProblemType :
			return str + std::to_string(this->valueOf<int>(name));
		case AlphabetCardinality :
			return str + std::to_string(this->valueOf<int>(name));
		case TranslationMixingFactor :
			return str + std::to_string(this->valueOf<double>(name));
		case TranslationOffset :
			return str + std::to_string(this->valueOf<double>(name));
		case EpsilonPercentage :
			return str + std::to_string(this->valueOf<double>(name));
		case AutomatonStructure :
			return str + std::to_string(this->valueOf<int>(name));
		case AutomatonSize :
			return str + std::to_string(this->valueOf<int>(name));
		case AutomatonFinalProbability :
			return str + std::to_string(this->valueOf<double>(name));
		case AutomatonTransitionsPercentage :
			return str + std::to_string(this->valueOf<double>(name));
		case AutomatonMaxDistance :
			return str + std::to_string(this->valueOf<int>(name));
		case AutomatonSafeZoneDistance :
			return str + std::to_string(this->valueOf<int>(name));
		case ActiveAutomatonPruning :
			return str + std::to_string(this->valueOf<bool>(name));
		case ActiveRemovingLabel :
			return str + std::to_string(this->valueOf<bool>(name));
		case ActiveDistanceCheckInTranslation :
			return str + std::to_string(this->valueOf<bool>(name));
		case PrintStatistics :
			return str + std::to_string(this->valueOf<bool>(name));
		case PrintTranslation :
			return str + std::to_string(this->valueOf<bool>(name));
		case PrintOriginalAutomaton :
			return str + std::to_string(this->valueOf<bool>(name));
		case PrintSCSolution :
			return str + std::to_string(this->valueOf<bool>(name));
		case PrintESCSOlution :
			return str + std::to_string(this->valueOf<bool>(name));
		case DrawOriginalAutomaton :
			return str + std::to_string(this->valueOf<bool>(name));
		case DrawSCSolution :
			return str + std::to_string(this->valueOf<bool>(name));
		case DrawESCSOlution :
			return str + std::to_string(this->valueOf<bool>(name));
		default :
			DEBUG_LOG_ERROR("Impossibile identificare il nome passato come parametro");
			return "ERROR : INVALID NAME";
		}
	}


} /* namespace translated_automata */
