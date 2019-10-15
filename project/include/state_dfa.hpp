/******************************************************************************
 * state_nfa.hpp
 *
 * Project: TranslatedAutomata
 *
 * Header contenente la definizione e le firme dei metodi della classe
 * concreta "StateDFA", figlia della classe astratta "State".
 * Concettualmente, un oggetto "StateDFA" rappresenta uno stato di un DFA, ossia
 * di un automa a stati finiti deterministico.
 * Un oggetto StateDFA ha la caratteristica di avere un solo nodo figlio per
 * ciascuna (eventuale) label. Questo implica che non avrà mai più di N transizioni
 * uscenti, dove N è la cardinalità dell'alfabeto.
 * Nel nostro problema, uno StateDFA è fortemente legato al concetto di "Estensione":
 * l'estensione di uno stato DFA è un'insieme di stati NFA a cui lo stato corrisponde,
 * secondo un processo di Subset Construction che traduce l'NFA in DFA.
 *
 ******************************************************************************/

#include "state.hpp"
#include "state_nfa.hpp"
#include "automaton.hpp"

#define DEFAULT_VOID_DISTANCE 1U<<30

namespace translated_automata {

	/**
	 * Definisco l'estensione di uno stato StateDFA
	 * come un'insieme di stati StateNFA, analogamente
	 * a quanto succede nell'algoritmo Subset Construction.
	 */
    using ExtensionDFA = set<StateNFA*, StateNFA::Comparator>;

	/**
	 * Concrete class "StateDFA".
	 * Eredita pubblicamente da State parametrizzata sulla
	 * stessa classe StateDFA.
	 */
    class StateDFA : public State<StateDFA> {

	private:
		ExtensionDFA* m_extension;								// Nodi dell'NFA corrispondente
		unsigned int m_distance = DEFAULT_VOID_DISTANCE;		// Distanza del nodo
		bool m_updated = false;									// Flag che segnala l'aggiornamento
		bool m_processed = false;								// Flag che segnala se è stato processato

    public:
		static string createNameFromExtension(const ExtensionDFA &ext);
		static ExtensionDFA subtractExtensions(const ExtensionDFA &ext1, const ExtensionDFA &ext2);

		StateDFA(ExtensionDFA* extension, bool processed = false);
		~StateDFA();

		set<string> getExitingLabels();
		void copyExitingTransitionsOf(StateDFA* state);
		void copyIncomingTransitionsOf(StateDFA* state);
		void copyAllTransitionsFrom(StateDFA* state);

		ExtensionDFA* lClosure(string l);
//		ExtensionDFA* lClosure(string l, NFA &nfa_mut);
		const ExtensionDFA& getExtension();
		void replaceExtensionWith(ExtensionDFA* new_ext);
		bool hasExtension(const ExtensionDFA &ext);

		int getDistance();
		void setDistance(int distance);
		void initDistancesRecursively(int root_distance);
	    void setBetterDistancesRecursively(int root_distance);

		void setProcessed();
		bool isProcessed();
		bool isUpdated();

		bool isEmpty() const;
		bool isFinal() const;
		void setFinal(bool final);
		StateDFA* duplicate() const;

    };

}
