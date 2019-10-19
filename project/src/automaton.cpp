/******************************************************************************
 * automaton.cpp
 *
 * Project: TranslatedAutomaton
 *
 * File sorgente contenente la classe "Automaton", che rappresenta in astratto
 * un Automa a stati finiti.
 * Questa classe viene ereditata dalle due sottoclassi concrete DFA e NFA,
 * rispettivamente Automa a Stati Finit Non Deterministico e Automa a Stati Finiti
 * Deterministico.
 *
 ******************************************************************************/

#include "automaton.hpp"

#include "debug.hpp"
#include "state_nfa.hpp"
#include "state_dfa.hpp"

namespace translated_automata {

	/**
	 * Costruttore della classe Automaton.
	 * Istanzia lo stato iniziale a NULL
	 * e la mappa degli stati come vuota.
	 */
    template <class State>
    Automaton<State>::Automaton()
	: m_states() {
    	m_initial_state = NULL;
    }


    /**
     * Distruttore della classe Automaton.
     */
    template <class State>
    Automaton<State>::~Automaton() {
    	// Itero sulla mappa di stringhe e stati
        for (auto iterator = m_states.begin(); iterator != m_states.end(); ++iterator) {
        	// Verifico se lo stato associato ad una stringa è ancora NON nullo
            if (iterator->second != NULL) {
                delete iterator->second;
            }
            else {
            	DEBUG_LOG_ERROR("Puntatore NULL per lo stato associato alla label %s", iterator->first.c_str() );
            }
        }
    }

    /**
     * Restituisce la dimensione dell'automa, ossia il numero di stati.
     */
    template <class State>
    int Automaton<State>::size() {
        return m_states.size();
    }

    /**
     * Verifica se un automa contiene lo stato s al suo interno.
     * I confronti avvengono tramite il nome dello stato.
     */
    template <class State>
    bool Automaton<State>::hasState(State* s) {
        auto search = m_states.find(s->getName());
        return search != m_states.end();
    }

    /**
     * Verifica se un automa contiene uno stato associato alla label "name" al suo interno.
     */
    template <class State>
    bool Automaton<State>::hasState(string name) {
        auto search = m_states.find(name);
        return search != m_states.end();
    }

    /**
     * Ritorna - se presente - lo stato corrispondente alla
     * label passata come parametro.
     * In caso questo stato non faccia parte dell'automa, viene
     * restituito un puntatore NULL.
     */
    template <class State>
    State* Automaton<State>::getState(string name) {
        State* state = m_states[name];
        return state;
    }

    /**
     * Aggiunge uno stato alla mappa degli stati di questo automa.
     * Nel caso esi già uno stato associato a tale nome, quello pre-esistente viene sovrascritto.
     */
    template <class State>
    void Automaton<State>::addState(State* s) {
        m_states[s->getName()] = s;
    }

    /**
     * Richiede in ingresso uno stato.
     * Rimuove dall'automa lo stato dell'automa che abbia come nome il nome
     * dello stato in ingresso.
     * Se la rimozione avviene restituisce "TRUE", altrimenti se lo stato
     * non viene trovato restituisce "FALSE".
	 * Questo metodo NON distrugge lo stato dell'automa.
     */
    template <class State>
    bool Automaton<State>::removeState(State* s) {
       return m_states.erase(s->getName());
    }

    /**
	 * Richiede in ingresso una stringa.
	 * Rimuove dall'automa lo stato dell'automa che abbia come nome la stringa
	 * passata come parametro.
	 * Se la rimozione avviene restituisce "TRUE", altrimenti se lo stato
	 * non viene trovato restituisce "FALSE".
	 * Questo metodo NON distrugge lo stato dell'automa.
	 */
	template <class State>
	bool Automaton<State>::removeState(string name) {
	   return m_states.erase(name);
	}

	/**
	 * Richiede in ingresso una stringa.
	 * Rimuove e distrugge dall'automa lo stato dell'automa che abbia come nome la stringa
	 * passata come parametro.
	 * Se la distruzione avviene restituisce "TRUE", altrimenti se lo stato
	 * non viene trovato restituisce "FALSE".
	 * Nota: occupandosi della distruzione dello stato, lo stato non sarà più raggiungibile.
	 */
    template <class State>
    bool Automaton<State>::deleteState(string name) {
        auto search = m_states.find(name);
        if (search == m_states.end()) {
            return false;
        }

        State* s = m_states[name];
        m_states.erase(name);
        delete s;

        return true;
    }

    /**
     * Imposta uno stato come stato iniziale.
     * All'istanziazione, il riferimento allo stato iniziale ha valore NULL.
     *
     * In caso questo metodo venga chiamato una seconda volta, il nodo iniziale viene
     * "sovrascritto", e si perde il riferimento al precedente.
     *
     * Nota: non è possibile impostare come stato iniziale uno stato non appartenente all'automa.
     * In caso lo stato non faccia parte dell'automa, l'operazione non verrà effettuata.
     */
    template <class State>
    void Automaton<State>::setInitialState(State* s) {
    	if (hasState(s)) {
			m_initial_state = s;
			m_states[s->getName()] = s;
    	}
    }

    /**
     * Imposta come stato iniziale lo stato dell'automa associato al nome passato come parametro.
     *
     * In caso questo metodo venga chiamato più di una volta, il nodo iniziale viene
     * "sovrascritto" e viene perso il riferimento al precedente.
     *
     * Nota: non è possibile impostare come stato iniziale uno stato non appartenente all'automa.
     * In caso lo stato non faccia parte dell'automa, l'operazione non verrà effettuata.
     */
    template <class State>
    void Automaton<State>::setInitialState(string name) {
    	if (hasState(name)) {
			m_initial_state = m_states[name];
    	}
    }

    /**
     * Verifica se uno stato associato ad un certo nome è impostato come stato iniziale.
     */
    template <class State>
    bool Automaton<State>::isInitial(string name) {
        return (m_initial_state->getName() == name);
    }

    /**
     * Verifica se uno stato è impostato come stato iniziale.
     */
    template <class State>
    bool Automaton<State>::isInitial(State* s)
    {
        return (m_initial_state == s);
        /* Nota:
         * Prima il confronto veniva effettuato tramite i nomi, ma poiché è stato definito
         * l'operatore di uguaglianza come confronto di nomi, quest'implementazione non dovrebbe
         * dare problemi.
         */
    }

    /**
     * Restituisce il nodo iniziale
     */
    template <class State>
    State* Automaton<State>::getInitialState() {
        return m_initial_state;
    }

    /**
     * Rimuove e distrugge lo stato con nome "", se presente.
     */
    template <class State>
    bool Automaton<State>::deleteEmptyState() {
    	// Verifico se esiste uno stato con associato un nome vuoto
        if (m_states.count("")) {
            State* s = m_states[""];
            m_states.erase("");
            delete s;
            return true;
        }
        return false;
    }

    /**
     * Metodo privato.
     *
     * Rimuove da un insieme tutti i nodi che sono raggiungibili (leggi: CONNESSI) dal nodo s passato come parametro
     * mediante una transizione s->s', più tutti quelli che sono raggiungibili anche da essi in cascata.
     * Inoltre, rimuove anche lo stato s.
     * Questo metodo è usato per rimuovere tutti i nodi connessi (in maniera DIREZIONALE) allo stato iniziale,
     * ossia tutti quelli raggiungibili nell'automa.
     */
    template <class State>
    void Automaton<State>::removeReachableStates(State* s, set<State*> &states) {
    	// Verifico se contengo lo stato s
        if (states.count(s)) {
            states.erase(s);
            // Rimuovo lo stato dalla mappa. Se lo rimuovessi DOPO la chiamata ricorsiva, un ciclo
            // di transizioni sugli stati genererebbe uno stack di chiamate ricorsive illimitato.

            // Per tutte le transizioni uscenti da s
            for (auto &pair: s->getExitingTransitions()) {
            	// Per ciascuno stato raggiunto dalle transizioni
                for (State* child: pair.second) {
                	removeReachableStates(child, states); // Chiamata ricorsiva sui figli
                }
            }
        }
    }

    /**
     * Rimuove gli stati dell'automa che non sono più raggiungibili dallo stato iniziale,
     * ossia tutti gli stati dell'automa che non possono essere "visitati" tramite una sequenza di transizioni.
     *
     * L'idea è quella di prendere l'insieme di tutti gli stati dell'automa e rimuovere quelli raggiungibili.
     * Gli stati che rimarranno saranno necessariamente gli stati irraggiungibili.
     *
     * Restituisce gli stati che sono stati rimossi e che risultavano irraggiungibili.
     */
    template <class State>
    set<State*> Automaton<State>::removeUnreachableStates() {
    	// Creo l'insieme di tutti gli stati dell'automa
        set<State*> unreachable;
        for (auto &pair: m_states) {
            unreachable.insert(pair.second);
        }

        // Lavoro per differenza: rimuovo dall'insieme tutti gli stati raggiungibili
        removeReachableStates(m_initial_state, unreachable);

        // Mi restano tutti gli stati irrangiungibili, sui quali itero
        for (State* s: unreachable) {
        	// Rimuovo dalla mappa dell'automa ogni stato irraggiungibile
            m_states.erase(s->getName());
        }

        return unreachable;
    }

    /**
     * Restituisce la lista di tutti gli stati dell'automa sotto forma di list.
     * Gli stati sono restituiti come puntatori.
     * La classe "list" permette un'aggiunta o rimozione in mezzo alla lista
     * senza reallocazione della coda.
     */
    template <class State>
    const list<State*> Automaton<State>::getStatesList() {
    	list<State*> states_list;
    	for (auto &pair : m_states) {
    		states_list.push_back(pair.second);
    	}
        return states_list;
    }

    /**
     * Restituisce il vettore dinamico di tutti gli stati dell'automa sotto forma di vector.
     * Gli stati sono restituiti come puntatori.
     * La classe "vector" permette un accesso casuale con tempo costante.
     */
    template <class State>
    const vector<State*> Automaton<State>::getStatesVector() {
    	vector<State*> states_vector;
		for (auto &pair : m_states) {
			states_vector.push_back(pair.second);
		}
		return states_vector;
    }

    /**
     * Restituisce l'insieme di etichette usate nelle transizioni di questo automa.
     */
    template <class State>
    const set<string>& Automaton<State>::getLabels() {
        set<string> *labels;
        for (auto &pair : m_states) {
            State* s = pair.second;
            for (auto &trans: s->getExitingTransitions())
                labels->insert(trans.first);
        }

        return *labels;
    }

    /**
     * Inserisce una transizione fra i due stati marcata con la label passata come parametro.
     * Il metodo funziona solamente se entrambi gli stati sono appartenenti all'automa, e in
     * tal caso restituisce TRUE.
     * In caso contrario restituisce FALSE.
     */
    template <class State>
    bool Automaton<State>::connectStates(State *from, State *to, string label) {
    	if (this->hasState(from) && this->hasState(to)) {
    		from->connectChild(label, to);
    		return true;
    	} else {
    		return false;
    	}
    }

    /**
	 * Inserisce una transizione fra i due stati marcata con la label passata come parametro.
	 * Il metodo funziona solamente se entrambi gli stati sono appartenenti all'automa, e in
     * tal caso restituisce TRUE.
     * In caso contrario restituisce FALSE.
	 */
    template <class State>
    bool Automaton<State>::connectStates(string from, string to, string label) {
    	return this->connectStates(m_states[from], m_states[to], label);
    }

    /**
     * Stampa tutti gli stati appartenenti all'automa, evidenziando lo stato iniziale.
     * Ogni stato deve essere stato istanziato correttamente.
     * Lo stato iniziale deve essere stato impostato correttamente.
     */
    template <class State>
    void Automaton<State>::print() {
        std::cout << "Initial: " << m_initial_state->getName() << '\n';

        // Per ogni stato dell'automa
        for (auto it = m_states.begin(); it != m_states.end(); ++it) {
            std::cout << it->second->toString();
        }
    }

    /**
     * Operatore di uguaglianza per automi.
     */
    template <class State>
    bool Automaton<State>::operator==(Automaton<State>& other) {
    	// Se gli stati iniziali non sono uguali, certamente i due automi non sono uguali
        if (*m_initial_state != *other.m_initial_state) {
        	return false;
        }

        // Se gli automi non hanno la stessa dimensione (= numero di stati), allora non sono certamente uguali
        if (m_states.size() != other.m_states.size()) {
        	return false;
        }

        // Effettuo il confronto stato per stato dei due automi.
        // Essendo le mappe ordinate, posso basarmi sullo scorrimento degli stati delle due mappe.
        auto ia = m_states.begin(), ib = other.m_states.begin();

        while (ia != m_states.end() && ib != m_states.end()) {
            if (ia->first != ib->first) {
            	return false;
            } else if (ia->second->hasSameTransitions(ib->second)) {
            	return false;
            }

            ++ia; ++ib;
        }

        return true;
    }

    /**
     * Crea una copia completa dell'automa.
     *
     * Nota: funziona solamente se l'automa di partenza è ben formato, ossia:
     * - Tutti i suoi stati hanno transizioni uscenti solo verso stati dell'automa.
     * - Tutti i suoi stati hanno transizioni entranti solo da stati dell'automa.
     */
    template <class State>
    Automaton<State>* Automaton<State>::clone() {
    	// Preparo il nuovo automa
        Automaton<State>* aut = new Automaton<State>();

        // Copio tutti gli stati
        for (auto &pair: m_states) {
            State* s = pair.second;
            State* s_copy = s->duplicate();
            aut->addState(s_copy);
        }

        // Copio tutte le transizioni
        for (auto &pair: m_states) {
            State* s = pair.second;
            State* s_copy = aut->getState(s->getName());
            for (auto &trans: s->getExitingTransitionsRef()) {
                string label = trans.first;
                for (State* child: trans.second)
                	s_copy->connectChild(label, aut->getState(child->getName()));
            }
        }

        // Copio lo stato iniziale
        aut->setInitialState(m_initial_state->getName());

        return aut;
    }

    /**
     * Metodo che confronta due stati. Il confronto non ha un significato nel dominio,
     * poiché è effettuato mediante puntatori, ma serve unicamente per avere un ordinamento
     * totale.
     */
    template <class State>
    bool compareStates(State* lhs, State* rhs) {
      return *lhs < *rhs;
    }


    /*************
     * Nota: essendo la classe Automaton parametrizzata sul tipo "State",
     * è necessario comunicare al compilatore quali implementazioni verranno utilizzate.
     */
    template class Automaton<StateNFA>;
    template class Automaton<StateDFA>;

} /* namespace translated_automata */
