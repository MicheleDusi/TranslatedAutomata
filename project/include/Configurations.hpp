/*
 * Configurations.hpp
 *
 * Dichiarazione della classe "Configurations", che racchiude le configurazioni
 * previste per l'esecuzione del programma.
 */

#ifndef INCLUDE_CONFIGURATIONS_HPP_
#define INCLUDE_CONFIGURATIONS_HPP_

#include <iostream>
#include <map>

#include "Debug.hpp"

using std::string;
using std::map;

namespace translated_automata {

	typedef enum {
		Testcases,

		ProblemType,

		AlphabetCardinality,
		TranslationMixingFactor,
		TranslationOffset,
		EpsilonPercentage,

		AutomatonStructure,
		AutomatonSize,
		AutomatonFinalProbability,
		AutomatonTransitionsPercentage,
		AutomatonMaxDistance,
		AutomatonSafeZoneDistance,

		ActiveAutomatonPruning,
		ActiveRemovingLabel,
		ActiveDistanceCheckInTranslation,

		PrintStatistics,
		PrintTranslation,
		PrintOriginalAutomaton,
		PrintSCSolution,
		PrintESCSOlution,

		DrawOriginalAutomaton,
		DrawSCSolution,
		DrawESCSOlution

	} SettingID;

	typedef enum {
		INT,
		DOUBLE,
		BOOL,
	} SettingType;

	typedef union {
		int integer;
		double real;
		bool flag;
	} Value;

	/**
	 * Rappresenta un valore di tipo generico associato ad una specifica configurazione.
	 */
	class SettingValue {

	private:
		SettingType m_type;
		Value m_value;

	public:
		SettingValue(int value);
		SettingValue(double value);
		SettingValue(bool value);
		virtual ~SettingValue() {};

		SettingType getType();
		string getValueString();

		template <typename T> T getTemplateValue() {
			switch (this->m_type) {
			case INT :
				return static_cast<int>(this->m_value.integer);
			case DOUBLE :
				return static_cast<double>(this->m_value.real);
			case BOOL :
				return static_cast<bool>(this->m_value.flag);
			default :
				DEBUG_LOG_ERROR("Impossibile interpretare il valore %b", this->m_value.integer);
				return static_cast<int>(this->m_value.integer);
			}
		};

	};

	/**
	 * Lista di configurazioni, attualmente inizializzate runtime sulla base di quello che viene definito nel metodo "load".
	 */
	class Configurations {

	private:

		/**
		 * Singola configurazione, rappresenta concettualmente il setting a prescidere
		 * dal valore assunto in una specifica istanza del programma.
		 */
		struct Setting {
			SettingID m_id;		// L'identificatore (enum) che rappresenta
			string m_name;		// Il nome completo che ne spiega brevemente lo scopo
			string m_abbr;		// Il nome abbreviato o la sigla che lo identifica
			bool m_test_param;	// Indica se il setting è utilizzato per il testing, e quindi deve essere stampato nei risultati
		};

		static const Setting settings_list[];					// Lista statica di tutte le configurazioni, inizializzata compile-time o all'avvio
		map<SettingID, SettingValue*> m_settings_instances;		// Valori delle configurazioni in una specifica esecuzione del programma

		static const Setting& getSetting(const SettingID& id);

		/**
		 * Carica un singolo parametro di configurazione all'interno della mappa.
		 */
		template <typename T> void load(const SettingID& id, T value) {
			this->m_settings_instances.insert(std::make_pair(id, new SettingValue(value)));
		};

	public:
		Configurations();
		~Configurations();

		void load();
		static string nameOf(const SettingID& id);
		static string abbreviationOf(const SettingID& id);
		static bool isTestParam(const SettingID& id);
		string toString(const SettingID& id);

		template <class T> T valueOf(const SettingID& id) {
			DEBUG_ASSERT_TRUE(this->m_settings_instances.count(id));
			return this->m_settings_instances.at(id)->getTemplateValue<T>();
		}

	};

} /* namespace translated_automata */

#endif /* INCLUDE_CONFIGURATIONS_HPP_ */
