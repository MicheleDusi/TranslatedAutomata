/******************************************************************************
 * embedded_subset_construction.hpp
 *
 * Project: TranslatedAutomata
 *
 * File sorgente della classe "EmbeddedSubsetConstruction" che implementa l'algoritmo
 * "Embedded Subset Construction" per la traduzione di un automa DFA.
 * Per maggiori informazioni sul funzionamento dell'algoritmo, si veda la documentazione
 * del metodo "run".
 *
 ******************************************************************************/

#include "embedded_subset_construction.hpp"

namespace translated_automata {

	/** Costruttore vuoto */
	EmbeddedSubsetConstruction::EmbeddedSubsetConstruction() {}

	/** Distruttore vuoto */
	EmbeddedSubsetConstruction::~EmbeddedSubsetConstruction() {}

	/**
	 * Metodo principale della classe, fornisce lo scheletro dell'algoritmo "Embedded Subset Construction".
	 *
	 * L'algoritmo può essere suddiviso in due fasi:
	 * 1) Nella prima viene applicata la traduzione all'automa di partenza, generando un NFA e un DFA
	 * 	  isomorfi le cui etichette sono state tradotte. Inoltre, viene generata la lista di Bud iniziali.
	 * 2) Nella seconda vengono processati i Buds della lista, con l'obiettivo principale di eliminare i
	 * 	  punti di non determinismo all'interno del DFA.
	 */
	DFA* EmbeddedSubsetConstruction::run(DFA* dfa, Translation* tau) {

		// Fase 1: Automaton Translation
		tuple<NFA*, DFA*, vector<Bud>> automaton_translation_result = tau->translate(dfa);
		NFA* translated_nfa = std::get<0>(automaton_translation_result);
		DFA* translated_dfa = std::get<1>(automaton_translation_result);
		vector<Bud> buds    = std::get<2>(automaton_translation_result);

		// Fase 2: Bud Processing
		this->runBudProcessing(translated_nfa, translated_dfa, buds);

		// Restituzione del DFA tradotto
		return translated_dfa;
	}

	/**
	 * Metodo privato.
	 * Fornisce un'implementazione della seconda fase dell'algoritmo più generale "Embedded Subset Construction"
	 * per la traduzione di automi (più specificamente, DFA).
	 */
	void EmbeddedSubsetConstruction::runBudProcessing(NFA* translated_nfa, DFA* translated_dfa, std::vector<Bud> buds) {

	}

} /* namespace translated_automata */
