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

	/**
	 * Restituisce il valore come stringa.
	 * Poiché il casting avviene internamente non è necessario usare template o overloading.
	 */
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
		this->m_settings_instances = map<SettingID, SettingValue*>();
	}

	/**
	 * Distruttore.
	 */
	Configurations::~Configurations() {}

	/**
	 * Metodo static.
	 * Restituisce il setting associato all'ID.
	 *
	 * NOTA: Il metodo fa affidamento sul fatto che le strutture sono state inizializzate nell'array statico
	 * in ordine, e quindi è possibile avervi accesso tramite l'indice dell'array.
	 * Per sicurezza è stato aggiunto un controllo all'interno di questo metodo, che -in caso l'ID non corrisponda-
	 * segnala un errore di debug.
	 */
	const Configurations::Setting& Configurations::getSetting(const SettingID& id) {
		const Configurations::Setting* setting;
		setting = &(Configurations::settings_list[static_cast<int>(id)]);
		if (setting->m_id != id) {
			DEBUG_LOG_ERROR("Il parametro richiesto con id=%d NON corrisponde al parametro di configurazione nella posizione attesa, che invece ha id=%d e nome \"%s\"",
					id, setting.m_id, setting.m_name.c_str());
		}
		return *setting;
	}

	/**
	 * Carica le configurazioni con cui eseguire il programma.
	 */
	void Configurations::load() {
		// Numero di Testcase
		load(Testcases, 							10);

		// Proprietà del problema
//		load(ProblemType, TRANSLATION_PROBLEM);
		load(ProblemType, Problem::DETERMINIZATION_PROBLEM);

		load(AlphabetCardinality, 					5);
		load(TranslationMixingFactor, 				0.9);
		load(TranslationOffset, 					1);
		load(EpsilonPercentage, 					0.05);

//		load(AutomatonType, AUTOMATON_RANDOM);
//		load(AutomatonType, AUTOMATON_STRATIFIED);
		load(AutomatonStructure, AUTOMATON_STRATIFIED_WITH_SAFE_ZONE);
		load(AutomatonSize, 						500);
		load(AutomatonFinalProbability, 			0.1);
		load(AutomatonTransitionsPercentage, 		0.5);
		load(AutomatonMaxDistance, 					90);
		load(AutomatonSafeZoneDistance, 			80);

		// Moduli e funzionalità opzionali
		load(ActiveAutomatonPruning, 				true);		// In caso sia attivato, evita la formazione e la gestione dello stato con estensione vuota, tramite procedura Automaton Pruning
		load(ActiveRemovingLabel, 					true);		// In caso sia attivato, utilizza una label apposita per segnalare le epsilon-transizione, che deve essere rimossa durante la determinizzazione
		load(ActiveDistanceCheckInTranslation,	 	false);	// In caso sia attivato, durante la traduzione genera dei Bud solamente se gli stati soddisfano una particolare condizione sulla distanza [FIXME è una condizione che genera bug]

		load(PrintStatistics, 						true);
		load(PrintTranslation, 						false);
		load(PrintOriginalAutomaton, 				false);
		load(PrintSCSolution, 						false);
		load(PrintESCSOlution, 						false);

		load(DrawOriginalAutomaton, 				false);
		load(DrawSCSolution, 						false);
		load(DrawESCSOlution, 						false);
	}

	/////

	/** Inizializzazione della lista di configurazioni */
	const Configurations::Setting Configurations::settings_list[] = {
			{ Testcases,					"Testcases:                                ", true },
			{ ProblemType,					"Problem type:                             ", false	},
			{ AlphabetCardinality,			"Alphabet cardinality:                     ", true },
			{ TranslationMixingFactor , 	"Translation mixing factor:                ", false },
			{ TranslationOffset , 			"Translation offset:                       ", false },
			{ EpsilonPercentage , 			"Epsilong percentage:                      ", true },
			{ AutomatonStructure , 			"Automaton's structure type:               ", false },
			{ AutomatonSize , 				"Automaton's size (#states):               ", true },
			{ AutomatonFinalProbability , 	"Automaton's final states probability:     ", false },
			{ AutomatonTransitionsPercentage , "Automaton's transitions percentage:       ",         true },
			{ AutomatonMaxDistance , 		"Automaton's max distance:                 ", true },
			{ AutomatonSafeZoneDistance , 	"Automaton's safe-zone distance:           ", true },
			{ ActiveAutomatonPruning , 		"Active \"automaton pruning\"                ", false },
			{ ActiveRemovingLabel , 		"Active \"removing label\":                  ", false },
			{ ActiveDistanceCheckInTranslation , "Active \"distance check in translation\":   ",       false },
			{ PrintStatistics , 			"Print statistics:                         ", false },
			{ PrintTranslation , 			"Print translation:                        ", false },
			{ PrintOriginalAutomaton , 		"Print original automaton:                 ", false },
			{ PrintSCSolution , 			"Print SC solution:                        ", false },
			{ PrintESCSOlution , 			"Print ESC solution:                       ", false },
			{ DrawOriginalAutomaton , 		"Draw original automaton:                  ", false },
			{ DrawSCSolution , 				"Draw SC solution:                         ", false },
			{ DrawESCSOlution , 			"Draw ESC solution:                        ", false }
	};

	/**
	 * Metodo statico.
	 * Restituisce il nome del parametro di configurazione.
	 */
	string Configurations::nameOf(const SettingID& id) {
		return Configurations::getSetting(id).m_name;
	}

	/**
	 * Metodo statico.
	 * Restituisce un flag booleano indicante se il valore del parametro deve
	 * essere stampato in coda ai test.
	 */
	bool Configurations::isTestParam(const SettingID& id) {
		return Configurations::getSetting(id).m_test_param;
	}

	/**
	 * Restituisce una stringa contenente NOME e VALORE associati al parametro
	 * passato in ingresso.
	 */
	string Configurations::toString(const SettingID& id) {
		return (Configurations::nameOf(id) + this->m_settings_instances.at(id)->getValueString());
	}


} /* namespace translated_automata */
