/******************************************************************************
 * translation_generator.cpp
 *
 * Project: TranslatedAutomata
 *
 * Implementazione della classe TranslationGenerator, avente la responsabilità
 * di generare traduzioni di alfabeti secondo i parametri preimpostati.
 * E' possibile personalizzare la generazione mediante due parametri:
 * · Mixing Factor (coefficiente per il quale viene moltiplicato l'indice).
 * · Offset (valore per il quale viene shiftato l'indice).
 *
 ******************************************************************************/

#include "translation_generator.hpp"

#include <cmath>

namespace translated_automata {

	const double TranslationGenerator::default_mixing_factor = 1.;
	const double TranslationGenerator::default_offset = 0.;

	/** Costruttore */
	TranslationGenerator::TranslationGenerator() {
		this->m_mixing_factor = default_mixing_factor;
		this->m_offset = default_offset;
	}

	/** Distruttore */
	TranslationGenerator::~TranslationGenerator() {}

	/**
	 * Getter per il fattore di rimescolamento della traduzione.
	 */
	double TranslationGenerator::getMixingFactor() {
		return this->m_mixing_factor;
	}

	/**
	 * Getter per l'offset della traduzione.
	 */
	double TranslationGenerator::getOffset() {
		return this->m_offset;
	}

	/**
	 * Setter per il fattore di rimescolamento della traduzione.
	 *
	 * Precondizione: il valore del parametro deve essere non negativo.
	 * In caso dovesse essere inserito un valore negativo, il fattore di
	 * rimescolamento sarà impostato al valore di default.
	 *
	 * Il Mixing Factor fornisce un'indicazione sul fattore di rimescolamento della traduzione.
	 * L'oggetto Translation verrà costruito associando ad ogni simbolo dell'alfabeto il simbolo
	 * con indice pari al prodotto fra l'indice originale e il fattore di rimescolamento.
	 * Questo significa che:
	 * 	· mf = 0 --> La traduzione tradurrà tutti i simboli dell'alfabeto nel simbolo di indice 0 (traduzione nulla).
	 * 	· mf = 1 --> La traduzione tradurrà ogni simbolo dell'alfabeto in sé stesso (identità o traduzione identica).
	 * 	· mf = 1/N , con N > 1 intero --> L'alfabeto sarà diviso in gruppi di cardinalità pari a N, i cui elementi verranno
	 * 				mandati nello stesso elemento dalla traduzione.
	 * 	· mf = N , con N > 1 intero --> In generale, l'alfabeto subirà un rimescolamento. Più nello specifico, la traduzione
	 * 				corrisponderà ad una permutazione per valori di N coprimi con la cardinalità dell'alfabeto.
	 */
	void TranslationGenerator::setMixingFactor(double mixing_factor) {
		if (mixing_factor < 0) {
			this->m_mixing_factor = TranslationGenerator::default_mixing_factor;
		} else {
			this->m_mixing_factor = mixing_factor;
		}
	}

	/**
	 * Setter per l'offset della traduzione.
	 *
	 * L'offset esprime lo scostamento che la traduzione produce in assenza di rimescolamento.
	 * Questo significa che (in assenza di rimescolamento):
	 * 	· os = 0 --> La traduzione tradurrà ogni simbolo dell'alfabeto in sé stesso (identità o traduzione identica).
	 * 	· os = N --> La traduzione tradurrà ogni simbolo (i) dell'alfabeto nel simbolo di indice (i+N).
	 *
	 * Nota: in caso di valori di offset superiori alla cardinalità dell'alfabeto, verrà considerata la classe di resto
	 * in modulo (cardinalità dell'alfabeto).
	 */
	void TranslationGenerator::setOffset(double offset) {
		this->m_offset = offset;
	}

	/**
	 * Genera casualmente e restituisce una traduzione sull'alfabeto passato come parametro.
	 * La generazione viene effettuata secondo i parametri pre-impostati con gli altri metodi
	 * della classe (come il Mixing Factor, l'Offset, etc..).
	 */
	Translation TranslationGenerator::generateTranslation(Alphabet& domain) {
		map<string, string> translation_map;

		// Parametri adattati all'alfabeto
		int cardinality = domain.size();
		double reduced_mixing_factor = std::fmod(this->m_mixing_factor, cardinality);
		double reduced_offset = std::fmod(std::fmod(this->m_offset, cardinality) + cardinality, cardinality);	// Nota: il duplice modulo è necessario per la gestione dei valori negativi

		// Associazione delle stringhe tradotte
		for (int i = 0; i < domain.size(); i++) {
			translation_map[domain[i]] = domain[(int) (std::fmod(i * reduced_mixing_factor + reduced_offset, cardinality))];
		}

		// Istanziazione dell'oggetto Translation
		Translation translation = Translation(domain, translation_map);
		return translation;
	}

} /* namespace translated_automata */
