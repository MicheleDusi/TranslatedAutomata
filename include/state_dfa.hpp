/*
 * state_dfa.h
 *
 */

#ifndef INCLUDE_STATE_DFA_H_
#define INCLUDE_STATE_DFA_H_

#include "state.hpp"
#include "state_nfa.hpp"

namespace translated_automata
{

	/**
	 * Definisco un'estensione di uno stato StateDFA come un'insieme di stati StateNFA,
	 * analogamente a quanto succede nell'algoritmo Subset Construction.
	 */
    using ExtensionDFA = set<StateNFA*, StateNFA::Comparator>;

	/**
	 * Template class "State".
	 */
    class StateDFA : public State<StateDFA>
    {

	private:
        ExtensionDFA* m_extension;
        int m_distance = -1;
        bool m_updated = false;
        bool m_processed = false;

    public:
        static string createName(const ExtensionDFA &ext);
        static ExtensionDFA subtractExt(const ExtensionDFA &ext1, const ExtensionDFA &ext2);

        StateDFA(ExtensionDFA* extension, bool processed=false);
        ~StateDFA();

        set<string> transitions();
        set<StateDFA*> copyTransitionsFrom(StateDFA* d);
        void copyIncomingTransitionsFrom(StateDFA* d);
        set<StateDFA*> copyAllTransitionsFrom(StateDFA* d);

        ExtensionDFA* lClosure(string l);
//        ExtensionDFA* lClosure(string l, NFA &nfa_mut);
        const ExtensionDFA& extension();
        void replaceExtension(ExtensionDFA* new_ext);
        bool hasExtension(const ExtensionDFA &ext);
        bool isUpdated();

        void setDistance(int dist);
        void setDistancesRecursively(int dist);
        int getDistance();

        void setProcessed();
        bool isProcessed();

    };
}

#endif /* INCLUDE_STATE_DFA_H_ */
