/******************************************************************************
 * constructed_state_dfa.cpp
 *
 * Project: TranslatedAutomata
 *
 * Sorgente contenente l'implementazione della classe concreta "ConstructedStateDFA", figlia
 * della classe "StateDFA".
 * Rappresenta uno stato di un automa DFA che è stato costruito sulla base di un automa NFA,
 * secondo un algoritmo di costruzione come SubsetConstruction o EmbeddedSubsetConstruction.
 * Ha la caratteristica principale di mantenere il riferimento agli stati dell'NFA da cui è
 * stato originato, riferimento che facilita le operazioni durante l'esecuzione degli algoritmi
 * sopra citati.
 *
 ******************************************************************************/

#include "constructed_state_dfa.hpp"

namespace translated_automata {

	/**
	 * Metodo statico.
	 * Crea il nome dello stato concatenando i nomi degli stati dell'estensione.
	 * Questo metodo è utilizzato in automatico per stabilire il nome di un oggetto
	 * ConstructedStateDFA ogni volta che viene assegnata o modificata la sua
	 * estensione.
	 */
	string ConstructedStateDFA::createNameFromExtension(const ExtensionDFA &ext) {
		// Inizializzo la stringa
		string name = "{";

		// Per ciascuno stato dell'estensione, aggiungo il nome alla lista
		for (StateNFA* s: ext) {
			name += s->getName() + ',';
		}
		// Rimuovo la virgola in coda
		name.pop_back();
		name += "}";

		return name;
	}

	/**
	 * Metodo statico.
	 * Sottrae dalla prima estensione gli stati della seconda estensione.
	 * Considerando le estensioni come insiemi, opera una differenza insiemistica e restituisce il risultato.
	 */
	ExtensionDFA ConstructedStateDFA::subtractExtensions(const ExtensionDFA &ext1, const ExtensionDFA &ext2) {
		ExtensionDFA result;

		for (StateNFA* s : ext1) {
			if (ext2.count(s) == 0) {
				result.insert(s);
			}
		}

		return result;
	}

	/**
	 * Metodo statico.
	 * Restituisce "true" se e solo se esiste almeno uno stato NFA all'interno
	 * dell'estensione marcato come stato finale.
	 */
	bool ConstructedStateDFA::hasFinalStates(const ExtensionDFA &ext) {
		for (StateNFA* state_nfa : ext) {
			if (state_nfa->isFinal()) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Costruttore.
	 * Assegna allo stato l'estensione passata come parametro e imposta il nome dello stato.
	 * Prima della costruzione viene chiamato il costruttore della classe padre "StateDFA" mediante
	 * l'utilizzo di due metodi statici che operano sull'estensione per ottenere il nome dello stato
	 * e il valore booleano rappresentante se lo stato è final o no.
	 */
	ConstructedStateDFA::ConstructedStateDFA(ExtensionDFA &extension)
		: StateDFA(ConstructedStateDFA::createNameFromExtension(extension), ConstructedStateDFA::hasFinalStates(extension)) {

		this->m_extension = extension;
	}

	/**
	 * Distruttore.
	 */
	ConstructedStateDFA::~ConstructedStateDFA() {
		this->m_extension.clear();
	}

	/**
	 * Verifica se lo stato ha una specifica estensione passata come parametro.
	 * Il confronto viene effettuato mediante la stringa contenente tutti i nomi dell'estensione.
	 */
	bool ConstructedStateDFA::hasExtension(const ExtensionDFA &ext) {
		return (getName() == createNameFromExtension(ext));
	}

	/**
	 * Restituisce l'estensione dello stato, ossia l'insieme di StateNFA
	 * da cui questo stato è stato creato.
	 */
	const ExtensionDFA& ConstructedStateDFA::getExtension() {
		return m_extension;
	}

	/**
	 * Restituisce tutte le etichette delle transizioni uscenti dagli stati
	 * dell'estensione.
	 */
	set<string>& ConstructedStateDFA::getLabelsExitingFromExtension() {
		static set<string> labels;

		// Per ciascuno stato dell'estensione
		for (StateNFA* member : m_extension) {
			// Inserisco le label delle transizioni uscenti
			for (auto &pair: member->getExitingTransitionsRef()) {
				labels.insert(pair.first);
			}
		}
		return labels;
	}

	/**
	 * Restituisce la l-closure data la stringa etichetta:
	 * per tutti gli stati della propria estensione, calcola la
	 * l-closure, e dopo unisce tutti gli insiemi.
	 *
	 * Nota: Non è necessario estendere il tutto con epsilon transizioni,
	 * poiché esse non vengono considerate.
	 */
	ExtensionDFA ConstructedStateDFA::computeLClosureOfExtension(string label) {
		ExtensionDFA closure;

		for (StateNFA* member : m_extension) {
			for (StateNFA* child : member->getChildren(label)) {
				closure.insert(child);
			}
		}

		return closure;
	}

	/**
	 * Sostituisce interamente l'estensione di questo stato con un'altra.
	 *
	 * Nota: questo metodo causa anche il cambio del nome dello stato, basato
	 * sugli stati dell'NFA che sono contenuti nella nuova estensione.
	 */
	void ConstructedStateDFA::replaceExtensionWith(ExtensionDFA &new_ext) {
		m_extension = new_ext;
		m_name = createNameFromExtension(m_extension);
	}

	/**
	 * Verifica se lo stato è "vuoto", ossia se la sua estensione è vuota.
	 */
	bool ConstructedStateDFA::isExtensionEmpty() {
		return m_extension.empty();
	}

} /* namespace translated_automata */
