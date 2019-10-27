/******************************************************************************
 * constructed_state_dfa.hpp
 *
 * Project: TranslatedAutomata
 *
 * Header della classe concreta "ConstructedStateDFA".
 * Rappresenta uno stato di un automa DFA che è stato costruito sulla base di un automa NFA,
 * secondo un algoritmo di costruzione come SubsetConstruction o EmbeddedSubsetConstruction.
 * Ha la caratteristica principale di mantenere il riferimento agli stati dell'NFA da cui è
 * stato originato, riferimento che facilita le operazioni durante l'esecuzione degli algoritmi
 * sopra citati.
 * Inoltre, contiene la definizione dell'estensione di uno stato DFA (denominata "ExtensionDFA"),
 * ossia dell'insieme di stati NFA originali. Tale estensione è implementata come std::set di StateNFA
 * ordinati mediante un Comparator operante sui nomi degli stati.
 *
 ******************************************************************************/

#ifndef INCLUDE_CONSTRUCTED_STATE_DFA_HPP_
#define INCLUDE_CONSTRUCTED_STATE_DFA_HPP_

#include "state_dfa.hpp"

namespace translated_automata {

	/**
	 * Definisco l'estensione di uno stato StateDFA
	 * come un'insieme di stati StateNFA, analogamente
	 * a quanto succede nell'algoritmo Subset Construction.
	 */
	using ExtensionDFA = set<StateNFA*, StateNFA::Comparator>;

	/**
	 * Classe concreta "ConstructedStateDFA". Rappresenta uno StateDFA ottenuto
	 * mediante un generico algoritmo di costruzione di un DFA partendo da un NFA.
	 * Per questo motivo, mantiene al suo interno un riferimento alla lista di stati
	 * dell'NFA da cui è stato originato.
	 */
	class ConstructedStateDFA : public StateDFA {

	private:
		ExtensionDFA m_extension;			// Stati dell'NFA corrispondente

	public:
		static string createNameFromExtension(const ExtensionDFA &ext);
		static ExtensionDFA subtractExtensions(const ExtensionDFA &ext1, const ExtensionDFA &ext2);
		static bool hasFinalStates(const ExtensionDFA &ext);

		ConstructedStateDFA(ExtensionDFA &extension);
		virtual ~ConstructedStateDFA();

		bool hasExtension(const ExtensionDFA &ext);
		const ExtensionDFA& getExtension();
		set<string>& getLabelsExitingFromExtension();
		ExtensionDFA computeLClosureOfExtension(string l);
		void replaceExtensionWith(ExtensionDFA &new_ext);
		bool isExtensionEmpty();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_CONSTRUCTED_STATE_DFA_HPP_ */
