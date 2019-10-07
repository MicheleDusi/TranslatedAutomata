/*
 * state.h
 *
 */

#ifndef INCLUDE_STATE_H_
#define INCLUDE_STATE_H_

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>

using std::string;
using std::vector;
using std::map;
using std::set;

namespace translated_automata
{

	/**
	 * Abstract class "State"
	 */
	template <class S>
	class State
    {

    private:
		/** Transizioni uscenti dallo stato */
        map<string, set<S*>> m_transitions;
        /** Transizioni entranti nello stato */
        map<string, set<S*>> m_incoming_transitions;

        S* getThis() const;
        void removeChild(string label, S* child);
        void removeParent(string label, S* parent);

	protected:
		string m_name = "";

    public:
		/** Metodi astratti */
        virtual bool isEmpty() const = 0;
        virtual bool isFinal() const = 0;
        virtual S* duplicate() const = 0;


		State();
        ~State();
        string getName() const;
		void connect(string transition, S* child);
        void disconnect(string transition, S* child);
        void detach();
        set<S*> getChildren(string label);
        S* getChild(string label);
        set<S*> getParents(string label);
        bool hasTransition(string label);
        bool hasTransition(string label, S* child);
        map<string, set<S*>> getTransitions();
        map<string, set<S*>> getIncomingTransitions();
        const map<string, set<S*>>& getTransitionsRef();
        const map<string, set<S*>>& getIncomingTransitionsRef();
        int transitionsCount();
        int incomingTransitionsCount();
        string toString() const;
        bool operator<(const S &other) const;
        bool operator==(const S &other) const;
        bool operator!=(const S &other) const;
    };
}

#endif /* INCLUDE_STATE_H_ */
