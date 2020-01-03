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
		Value getValue();
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
		map<SettingID, SettingValue*> m_settings;

		void load(SettingID, SettingValue*);

	public:
		Configurations();
		~Configurations();

		void load();
		static string nameOf(const SettingID& name);
		string toString(const SettingID& name);

		template <class T> T valueOf(const SettingID& name) {
			DEBUG_ASSERT_TRUE(this->m_settings.count(name));
			return this->m_settings.at(name)->getTemplateValue<T>();
		}

	};

} /* namespace translated_automata */

#endif /* INCLUDE_CONFIGURATIONS_HPP_ */
