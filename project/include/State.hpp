/*
 * State.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header per il sorgente "state.cpp".
 * Contiene la definizione della classe astratta "State", che viene implementata
 * dalle classe "StateNFA" e "StateDFA" ed è parametrizzata sul tipo "S".
 *
 */

#ifndef INCLUDE_STATE_H_
#define INCLUDE_STATE_H_

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <cstdbool>

using std::string;
using std::map;
using std::set;

#define DEFAULT_VOID_DISTANCE 1U<<30

namespace translated_automata {

	/**
	 * Abstract class "State".
	 * Classe padre di StateNFA, StateDFA, ConstructedDFA.
	 * Nel dominio concettuale, uno "State" è uno stato di un "automa a stati finiti",
	 * sia esso deterministico o non deterministico. E' caratterizzato da un nome
	 * univoco ed è legato ad altri stati mediante transizioni, ciascuna delle quali
	 * è marcata da una label.
	 * La classe "State" prevede, quando estesa, l'implementazione dei metodi virtuali
	 * "isFinal" e "duplicate".
	 */
	template <class S>
	class State {

    private:
        map<string, set<S*>> m_exiting_transitions;		// Transizioni uscenti dallo stato
        map<string, set<S*>> m_incoming_transitions;	// Transizioni entranti nello stato

        S* getThis() const;

	protected:
		string m_name = "";									// Nome dello stato
		bool m_final = false;								// Flag che indica se lo stato è finale o meno
		unsigned int m_distance = DEFAULT_VOID_DISTANCE;	// Distanza del nodo dal nodo iniziale

    public:
		State();											// Costruttore
        virtual ~State();									// Distruttore (virtuale)

        string getName() const;
        bool isFinal();
        void setFinal(bool final);
		void connectChild(string label, S* child);
		void disconnectChild(string label, S* child);
		void detachAllTransitions();
		set<S*> getChildren(string label);
		set<S*> getParents(string label);

		bool hasExitingTransition(string label);
		bool hasExitingTransition(string label, S* child);
		bool hasIncomingTransition(string label);
		bool hasIncomingTransition(string label, S* child);
		map<string, set<S*>> getExitingTransitions();
		map<string, set<S*>> getIncomingTransitions();
		const map<string, set<S*>>& getExitingTransitionsRef();
		const map<string, set<S*>>& getIncomingTransitionsRef();
		int getExitingTransitionsCount();
		int getIncomingTransitionsCount();
		void copyExitingTransitionsOf(S* other_state);
		void copyIncomingTransitionsOf(S* other_state);
		void copyAllTransitionsOf(S* other_state);

		bool hasSameTransitionsOf(S* other_state);
		bool hasSameTransitionsNamesOf(S* other_state);

		unsigned int getDistance();
		void setDistance(unsigned int distance);
		void initDistancesRecursively(int root_distance);
	    int getMinimumParentsDistance();

		string toString() const;

		bool operator<(const S &other) const;
		bool operator==(const S &other) const;
		bool operator!=(const S &other) const;
//		int compareNames(const S &other) const;

		struct Comparator {
            bool operator() (const S* lhs, const S* rhs) const {
                return lhs->getName() < rhs->getName();
            }
        };

    };

	/**
	 * Classe StateNFA.
	 * Eredita in maniera pubblica da State parametrizzata sulla stessa classe StateNFA.
	 * Concettualmente, un oggetto "StateNFA" rappresenta uno stato di un NFA, ossia
	 * di un automa a stati finiti non deterministico.
	 * Un oggetto StateNFA è caratterizzato dalla possibilità di avere più transizioni
	 * uscenti marcate con la stessa label, anche dirette a stati differenti.
	 */
    class StateNFA : public State<StateNFA> {

    public:
        StateNFA(string name, bool final = false);
		~StateNFA();

    };

	/**
	 * Concrete class "StateDFA".
	 * Eredita pubblicamente da State parametrizzata sulla stessa classe StateDFA.
	 * Concettualmente, un oggetto "StateDFA" rappresenta uno stato di un DFA, ossia
	 * di un automa a stati finiti deterministico.
	 * Un oggetto StateDFA ha la caratteristica di avere un solo nodo figlio per
	 * ciascuna (eventuale) label. Questo implica che non avrà mai più di N transizioni
	 * uscenti, dove N è la cardinalità dell'alfabeto.
	 * Nel nostro problema, uno StateDFA è fortemente legato al concetto di "Estensione":
	 * l'estensione di uno stato DFA è un'insieme di stati NFA a cui lo stato corrisponde,
	 * secondo un processo di Subset Construction che traduce l'NFA in DFA.
	 */
    class StateDFA : public State<StateDFA> {

    public:
		StateDFA(string name, bool final = false);
		~StateDFA();

		StateDFA* getChild(string label);

    };

	/**
	 * Estensione di uno stato StateDFA = insieme di stati StateNFA, utilizzati
	 * negli algoritmi di costruzione degli automi deterministici da automi a stati
	 * non deterministici.
	 */
	using ExtensionDFA = set<StateNFA*, StateNFA::Comparator>;

	/**
	 * Classe concreta "ConstructedStateDFA".
	 * Rappresenta uno stato di un automa DFA che è stato costruito sulla base di un automa NFA,
	 * secondo un algoritmo di costruzione come SubsetConstruction o EmbeddedSubsetConstruction.
	 * Ha la caratteristica principale di mantenere il riferimento agli stati dell'NFA da cui è
	 * stato originato, riferimento che facilita le operazioni durante l'esecuzione degli algoritmi
	 * sopra citati.
	 * Inoltre, contiene la definizione dell'estensione di uno stato DFA (denominata "ExtensionDFA"),
	 * ossia dell'insieme di stati NFA originali. Tale estensione è implementata come std::set di StateNFA
	 * ordinati mediante un Comparator operante sui nomi degli stati.
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

}

#endif /* INCLUDE_STATE_H_ */
