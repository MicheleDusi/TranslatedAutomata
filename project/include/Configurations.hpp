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
#include <vector>

#include "Debug.hpp"

using std::string;
using std::map;
using std::vector;

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
		LogStatistics,
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

		// La classe SettingMultiValue può accedere ai membri della classe SettingValue
		friend class SettingMultiValue;

	private:
		SettingType m_type;
		Value m_value;

	public:
		SettingValue(int value);
		SettingValue(double value);
		SettingValue(bool value);
		virtual ~SettingValue() {};

		SettingType getType();
		virtual Value getValue();
		virtual string getValueString(); // Restituisce il valore come stringa
		virtual string toString();		 // Restituisce l'oggetto SettingValue come stringa
		virtual bool nextCase();

	};

	/**
	 * Classe che accorpa più valori.
	 * Secondo il pattern Composite, è figlia della classe SettingValue e aggrega
	 * multipli valori SettingValue.
	 * Inoltre, utilizza l'attributo "value" della classe padre come indice numerico
	 * del valore corrente.
	 */
	class SettingMultiValue : public SettingValue {

	private:
		vector<SettingValue> m_multivalue;

	public:
		SettingMultiValue(vector<int> values);
		SettingMultiValue(vector<double> values);
		virtual ~SettingMultiValue() {};

		virtual Value getValue();
		virtual string getValueString(); // Restituisce il valore come stringa
		virtual string toString();		 // Restituisce l'oggetto SettingValue come stringa
		virtual bool nextCase();

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

		/**
		 * Carica un singolo parametro di configurazione multi-valore all'interno della mappa.
		 */
		template <typename T> void load(const SettingID& id, vector<T> values) {
			this->m_settings_instances.insert(std::make_pair(id, new SettingMultiValue(values)));
		}

	public:
		Configurations();
		~Configurations();

		void load();
		static string nameOf(const SettingID& id);
		static string abbreviationOf(const SettingID& id);
		static bool isTestParam(const SettingID& id);
		string getValueString();
		string toString();
		string toString(const SettingID& id);
		bool nextTestCase();

		template <class T> T valueOf(const SettingID& id) {
			DEBUG_ASSERT_TRUE(this->m_settings_instances.count(id));
			DEBUG_LOG("Richiesta del valore di: %s", settings_list[int(id)].m_name.c_str());
			SettingValue* associated_value_container = this->m_settings_instances[id];
			Value value = associated_value_container->getValue();
			switch (associated_value_container->getType()) {
			case INT :
				DEBUG_LOG("Valore restituito: %s", std::to_string(value.integer).c_str());
				return static_cast<int>(value.integer);
			case DOUBLE :
				DEBUG_LOG("Valore restituito: %s", std::to_string(value.real).c_str());
				return static_cast<double>(value.real);
			case BOOL :
				DEBUG_LOG("Valore restituito: %s", std::to_string(value.flag).c_str());
				return static_cast<bool>(value.flag);
			default :
				DEBUG_LOG_ERROR("Impossibile interpretare il valore %d", value.integer);
				return static_cast<int>(value.integer);
			}
		}

	};

} /* namespace translated_automata */

#endif /* INCLUDE_CONFIGURATIONS_HPP_ */
