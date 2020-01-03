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
		BOOLEAN,
	} SettingType;

	/**
	 * Rappresenta un valore di tipo generico associato ad una specifica configurazione.
	 */
	class SettingValue {

	private:
		SettingType m_type;

	public:
		SettingValue(SettingType type);
		virtual ~SettingValue() {};

		SettingType getType();

	};

	class IntSettingValue : public SettingValue {

	private:
		int m_value;

	public:
		IntSettingValue(int value);
		~IntSettingValue() {};
		int getIntValue();
	};

	class DoubleSettingValue : public SettingValue {

	private:
		double m_value;

	public:
		DoubleSettingValue(double value);
		~DoubleSettingValue() {};
		double getDoubleValue();
	};

	class BoolSettingValue : public SettingValue {

	private:
		bool m_value;

	public:
		BoolSettingValue(bool value);
		~BoolSettingValue() {};
		bool getBoolValue();
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
			SettingValue* setting_value = this->m_settings.at(name);

			switch (setting_value->getType()) {

			case INT : {
				IntSettingValue* sv = (IntSettingValue*)(setting_value);
				return (T) (sv->getIntValue());
			}
			case DOUBLE : {
				DoubleSettingValue* sv = (DoubleSettingValue*)(setting_value);
				return (T) (sv->getDoubleValue());
			}
			case BOOLEAN : {
				BoolSettingValue* sv = (BoolSettingValue*)(setting_value);
				return (T) (sv->getBoolValue());
			}
			default :
				DEBUG_LOG_ERROR("Impossibile interpretare %d come valore dell'enum SettingType", setting_value->getType());
				throw "Impossibile interpretare il valore";
			}
		}

//		template <> int valueOf(const SettingName& name);

	};

} /* namespace translated_automata */

#endif /* INCLUDE_CONFIGURATIONS_HPP_ */
