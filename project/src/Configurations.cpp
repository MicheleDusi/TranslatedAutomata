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
	AtomicSettingValue::AtomicSettingValue(int value) {
		DEBUG_LOG("Costruzione di un oggetto SettingValue con valore INT = %d", value);
		this->m_type = INT;
		this->m_value.integer = value;
	}

	/**
	 * Costruttore con valore reale.
	 */
	AtomicSettingValue::AtomicSettingValue(double value) {
		DEBUG_LOG("Costruzione di un oggetto SettingValue con valore DOUBLE = %f", value);
		this->m_type = DOUBLE;
		this->m_value.real = value;
	}

	/**
	 * Costruttore con valore booleano.
	 */
	AtomicSettingValue::AtomicSettingValue(bool value) {
		DEBUG_LOG("Costruzione di un oggetto SettingValue con valore BOOL = %d", value);
		this->m_type = BOOL;
		this->m_value.flag = value;
	}

	/**
	 * Restituisce il tipo del valore.
	 */
	SettingType AtomicSettingValue::getType() {
		return this->m_type;
	}

	/**
	 * Restituisce il valore.
	 * Sarà compito del metodo di Configurations castarlo al tipo corretto.
	 */
	Value AtomicSettingValue::getValue() {
		return this->m_value;
	}

	/**
	 * Restituisce il valore come stringa.
	 * Poiché il casting avviene internamente non è necessario usare template o overloading.
	 */
	string AtomicSettingValue::getValueString() {
		switch (this->m_type) {
		case INT :
			return std::to_string(this->m_value.integer);
		case DOUBLE :
			return std::to_string(this->m_value.real);
		case BOOL :
			return std::to_string(this->m_value.flag);
		default :
			DEBUG_LOG_ERROR("Impossibile interpretare il valore %d", this->m_value.integer);
			return "null";
		}
	}

	/**
	 * Restituisce una rappresentazione dell'oggetto come stringa.
	 * In caso di valori multipli, restituisce TUTTI i valori.
	 * Nella stringa è presente anche un'informazione sul tipo di valore.
	 */
	string AtomicSettingValue::toString() {
		switch (this->m_type) {
		case INT :
			return "int:" + std::to_string(this->m_value.integer);
		case DOUBLE :
			return "double:" + std::to_string(this->m_value.real);
		case BOOL :
			return "bool:" + std::to_string(this->m_value.flag);
		default :
			DEBUG_LOG_ERROR("Impossibile interpretare il valore %d", this->m_value.integer);
			return "null";
		}
	}

	/**
	 * Poiché il valore è "singolo" ( o atomico), non esiste un valore successivo e il metodo restituisce sempre FALSE.
	 */
	bool AtomicSettingValue::nextCase() {
		return false;
	}

// CLASSE "SettingMultiValue"

	/**
	 * Costruttore con un array di interi.
	 */
	CompositeSettingValue::CompositeSettingValue(vector<int> values) : SettingValue() {
		// Inserisco i valori
		this->m_multivalue = vector<SettingValue*>();
		for (int n : values) {
			this->m_multivalue.push_back(new AtomicSettingValue(n));
		}
		this->m_current_value_index = 0;
	}

	/**
	 * Costruttore con un array di double.
	 */
	CompositeSettingValue::CompositeSettingValue(vector<double> values) : SettingValue() {
		// Inserisco i valori
		this->m_multivalue = vector<SettingValue*>();
		for (double d : values) {
			this->m_multivalue.push_back(new AtomicSettingValue(d));
		}
		this->m_current_value_index = 0;
	}

	SettingType CompositeSettingValue::getType() {
		return this->m_multivalue[this->m_current_value_index]->getType();
	}

	/**
	 * Restituisce il valore corrente come stringa.
	 * In caso di valori multipli, solo il valore corrente è restituito come stringa.
	 */
	string CompositeSettingValue::getValueString() {
		return this->m_multivalue[this->m_current_value_index]->getValueString();
	}

	/**
	 * Restituisce una rappresentazione dell'oggetto come stringa.
	 * In caso di valori multipli, restituisce TUTTI i valori.
	 * Nella stringa è presente anche un'informazione sul tipo di valore.
	 */
	string CompositeSettingValue::toString() {
		// Concateno i valori
		string result = "{";
		for (SettingValue* sv : this->m_multivalue) {
			result += sv->getValueString() + ", ";
		}
		result.pop_back(); result.pop_back();
		result += "}";
		return result;
	}

	/**
	 * Restituisce il valore all'indice corrente.
	 * Sarà responsabilità della classe Configuration castare il tutto.
	 */
	Value CompositeSettingValue::getValue() {
		return this->m_multivalue[this->m_current_value_index]->getValue();
	}

	/**
	 * Incrementa il contatore e imposta l'oggetto al valore successivo del vettore.
	 * Se tale valore esiste, e quindi se tale valore cambia, viene restituito TRUE.
	 * Altrimenti viene restituito FALSE, e il contatore viene resettato a 0 per un nuovo ciclo.
	 */
	bool CompositeSettingValue::nextCase() {
		// Verifico se il valore corrente è atomico o contiene valori multipli a sua volta
		if ((this->m_multivalue[this->m_current_value_index])->nextCase()) {
			// In tal caso, itero su di esso e restituisco TRUE
			return true;
		}
		// Se invece il valore corrente ha già "esaurito" i suoi valori, o ha un valore atomico, il suo metodo restituirà false.
		else {
			// Viene quindi verificato se esistono altri valori nel vettore di questo oggetto
			// Incemento il contatore
			this->m_current_value_index++;
			// Verifico se corrisponde ad un nuovo valore
			if (this->m_current_value_index < this->m_multivalue.size()) {
				return true;
			} else {
				// Altrimenti sono arrivato alla fine, azzero il contatore e restituisco FALSE
				this->m_current_value_index = 0;
				return false;
			}
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
					id, setting->m_id, setting->m_name.c_str());
		}
		return *setting;
	}

	/////

	/**
	 * Carica le configurazioni con cui eseguire il programma.
	 */
	void Configurations::load() {
		// Numero di Testcase
		load(Testcases, 1);
		// Proprietà del problema
		//		load(ProblemType, TRANSLATION_PROBLEM);
		load(ProblemType, Problem::DETERMINIZATION_PROBLEM);
		load(AlphabetCardinality, 10);
		load(TranslationMixingFactor, 0.9);
		load(TranslationOffset, 1);
		load(EpsilonPercentage, 0);
//		load(AutomatonType, AUTOMATON_RANDOM);
//		load(AutomatonType, AUTOMATON_STRATIFIED);
		load(AutomatonStructure, AUTOMATON_STRATIFIED_WITH_SAFE_ZONE);
//		load(AutomatonStructure, AUTOMATON_ACYCLIC);
		load(AutomatonSize, vector<int>{10, 11, 12});
		load(AutomatonFinalProbability, 0.1);
		load(AutomatonTransitionsPercentage, 0.1);
		load(AutomatonMaxDistance, vector<int>{4,5});
		load(AutomatonSafeZoneDistance, 3);
		// Moduli e funzionalità opzionali
		load(ActiveAutomatonPruning, true); // In caso sia attivato, evita la formazione e la gestione dello stato con estensione vuota, tramite procedura Automaton Pruning
		load(ActiveRemovingLabel, true); // In caso sia attivato, utilizza una label apposita per segnalare le epsilon-transizione, che deve essere rimossa durante la determinizzazione
		load(ActiveDistanceCheckInTranslation, false); // In caso sia attivato, durante la traduzione genera dei Bud solamente se gli stati soddisfano una particolare condizione sulla distanza [FIXME è una condizione che genera bug]
		load(PrintStatistics, true);
		load(LogStatistics, true);
		load(PrintTranslation, false);
		load(PrintOriginalAutomaton, false);
		load(PrintSCSolution, false);
		load(PrintESCSOlution, false);
		load(DrawOriginalAutomaton, false);
		load(DrawSCSolution, false);
		load(DrawESCSOlution, false);
	}

	/** Inizializzazione della lista di configurazioni */
	const Configurations::Setting Configurations::settings_list[] = {
			{ Testcases,					"Testcases", 								"#test", false },
			{ ProblemType,					"Problem type", 							"problem", false },
			{ AlphabetCardinality,			"Alphabet cardinality", 					"#alpha", true },
			{ TranslationMixingFactor , 	"Translation mixing factor", 				"mixing", false },
			{ TranslationOffset , 			"Translation offset", 						"offset", false },
			{ EpsilonPercentage , 			"Epsilon percentage", 						"%epsilon", true },
			{ AutomatonStructure , 			"Automaton's structure type", 				"structure", false },
			{ AutomatonSize , 				"Automaton's size (#states)",	 			"#size", true },
			{ AutomatonFinalProbability , 	"Automaton's final states probability", 	"%finals", false },
			{ AutomatonTransitionsPercentage , "Automaton's transitions percentage", 	"%transitions", true },
			{ AutomatonMaxDistance , 		"Automaton's max distance", 				"maxdist", true },
			{ AutomatonSafeZoneDistance , 	"Automaton's safe-zone distance", 			"safezonedist", true },
			{ ActiveAutomatonPruning , 		"Active \"automaton pruning\"", 			"?autompruning", false },
			{ ActiveRemovingLabel , 		"Active \"removing label\"", 				"?removlabel", false },
			{ ActiveDistanceCheckInTranslation , "Active \"distance check in translation\"", "?distcheck",  false },
			{ PrintStatistics , 			"Print statistics", 						"?pstats", false },
			{ LogStatistics , 				"Log statistics in file", 					"?lstats", false },
			{ PrintTranslation , 			"Print translation", 						"?ptrad", false },
			{ PrintOriginalAutomaton , 		"Print original automaton", 				"?porig", false },
			{ PrintSCSolution , 			"Print SC solution", 						"?psc", false },
			{ PrintESCSOlution , 			"Print ESC solution", 						"?pesc", false },
			{ DrawOriginalAutomaton , 		"Draw original automaton", 					"?dorig", false },
			{ DrawSCSolution , 				"Draw SC solution", 						"?dsc", false },
			{ DrawESCSOlution , 			"Draw ESC solution", 						"?desc", false }
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
	 * Restituisce la sigla identificativa del parametro di configurazione.
	 */
	string Configurations::abbreviationOf(const SettingID& id) {
		return Configurations::getSetting(id).m_abbr;
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
	 * Stampa i valori CORRENTI delle configurazioni attuali.
	 */
	string Configurations::getValueString() {
		string result = "";
		for (int param = Testcases; param <= DrawESCSOlution; param++) {
			if (isTestParam((SettingID) param)) {
				result += this->m_settings_instances.at((SettingID)param)->getValueString() + ", ";
			}
		}
		return result;
	}

	/**
	 * Stampa i valori (completi!) di tutte configurazioni.
	 */
	string Configurations::toString() {
		string result = "Configurations:\n";
		for (int param = Testcases; param <= DrawESCSOlution; param++) {
			// TODO Ricordati di aggiornare l'ultimo valore
			result += Configurations::nameOf((SettingID)param) + " = ";
			result += this->m_settings_instances.at((SettingID)param)->toString() + "\n";
		}
		return result;
	}

	/**
	 * Restituisce una stringa contenente NOME e VALORE associati al parametro
	 * passato in ingresso.
	 */
	string Configurations::toString(const SettingID& id) {
		return (Configurations::abbreviationOf(id) + ":" + this->m_settings_instances.at(id)->toString());
	}

	/**
	 * Imposta i parametri salvati all'interno delle configurazioni con la combinazione successiva
	 */
	bool Configurations::nextTestCase() {
		for (auto &pair : this->m_settings_instances) {
			// Se viene trovato un caso successivo, si restituisce TRUE
			if (pair.second->nextCase()) {
				return true;
			}
		}
		return false;
	}


} /* namespace translated_automata */
