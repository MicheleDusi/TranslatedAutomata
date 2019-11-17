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

#include <algorithm>

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
     * Distrugge TUTTI gli stati contenuti nell'automa.
     */
    template <class State>
    Automaton<State>::~Automaton() {
    	for (State* s : m_states) {
    		delete s;
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
        return m_states.find(s) != m_states.end();
    }

    /**
     * Verifica se un automa contiene uno stato associato alla label "name" al suo interno.
     */
    template <class State>
    bool Automaton<State>::hasState(string name) {
    	for (State* s : m_states) {
    		if (s->getName() == name) {
    			return true;
    		}
    	}
    	return false;
    }

    /**
     * Ritorna - se presente - lo stato corrispondente alla label passata come parametro.
     * In caso questo stato non faccia parte dell'automa, viene restituito un puntatore NULL.
     *
     * Nota: In alcuni casi (durante l'esecuzione di algoritmi di costruzione) esiste più di uno stato
     * con lo stesso nome. Questa condizione si verifica solo all'interno di un'esecuzione e viene "corretta" al termine,
     * ma tuttavia non permette il corretto funzionamento di questo metodo (che invece restituisce il primo stato trovato).
     * Per evitare l'insorgere di problematiche legate a stati omonimi, si consiglia di utilizzare il metodo
     * "getStatesByName" che restituisce un insieme contenente TUTTI gli stati con il medesimo nome.
     */
    template <class State>
    State* Automaton<State>::getState(string name) {
    	for (State* s : m_states) {
    		if (s->getName() == name) {
    			return s;
    		}
    	}
    	return NULL;
    }

    /**
     * Restituisce l'insieme di tutti gli stati aventi il nome passato come parametro.
     * Normalmente questo metodo restituisce un unico stato, poiché gli stati sono unici per nome.
     * Tuttavia, durante l'esecuzione di algoritmi di costruzione, l'automa potrebbe trovarsi in condizioni particolari per cui
     * esiste più di uno stato con lo stesso nome. In quel caso, è bene utilizzare questo metodo e non "getState".
     */
    template <class State>
    const vector<State*> Automaton<State>::getStatesByName(string name) {
    	vector<State*> namesake_states; // Insieme di stati omonimi
    	for (State* s : m_states) {
    		if (s->getName() == name) {
    			namesake_states.push_back(s);
    		}
    	}
    	return namesake_states;
    }

    /**
     * Aggiunge uno stato alla mappa degli stati di questo automa.
     * Nel caso esi già uno stato associato a tale nome, quello pre-esistente viene sovrascritto.
     */
    template <class State>
    void Automaton<State>::addState(State* s) {
        m_states.insert(s);
    }

    /**
     * Richiede in ingresso uno stato.
     * Rimuove dall'automa lo stato dell'automa che abbia come nome il nome
     * dello stato in ingresso.
     * Se la rimozione avviene restituisce "TRUE", altrimenti se lo stato
     * non viene trovato restituisce "FALSE".
	 * Questo metodo NON distrugge lo stato.
     */
    template <class State>
    bool Automaton<State>::removeState(State* s) {
       return m_states.erase(s);
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
     *
     * Inoltre, si suppone che questa operazione venga effettuata al termine dell'inserimento di tutti
     * gli stati, poiché causa anche l'assegnamento delle distanze per ogni nodo raggiungibile dallo stato
     * iniziale. Pertanto, al termine di questa chiamata, ogni stato conterrà la distanza dal nodo iniziale,
     * a partire dal nodo iniziale che avrà distanza 0.
     * L'assegnamento delle distanze NON ha effetto sugli stati NON raggiungibili dallo stato impostato come
     * stato iniziale.
     */
    template <class State>
    void Automaton<State>::setInitialState(State* s) {
    	if (hasState(s)) {
			m_initial_state = s;
	    	s->initDistancesRecursively(0);
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
     *
     * Inoltre, si suppone che questa operazione venga effettuata al termine dell'inserimento di tutti
     * gli stati, poiché causa anche l'assegnamento delle distanze per ogni nodo raggiungibile dallo stato
     * iniziale. Pertanto, al termine di questa chiamata, ogni stato conterrà la distanza dal nodo iniziale,
     * a partire dal nodo iniziale che avrà distanza 0.
     * L'assegnamento delle distanze NON ha effetto sugli stati NON raggiungibili dallo stato impostato come
     * stato iniziale.
     */
    template <class State>
    void Automaton<State>::setInitialState(string name) {
    	if (hasState(name)) {
			m_initial_state = getState(name);
	    	m_initial_state->initDistancesRecursively(0);
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
    bool Automaton<State>::isInitial(State* s) {
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
        if (states.find(s) != states.end()) {
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
        set<State*> unreachable = set<State*>(m_states);

        // Lavoro per differenza: rimuovo dall'insieme tutti gli stati raggiungibili
        removeReachableStates(m_initial_state, unreachable);

        // Mi restano tutti gli stati irrangiungibili, sui quali itero
        for (State* s: unreachable) {
        	// Rimuovo dalla mappa dell'automa ogni stato irraggiungibile
            m_states.erase(s);
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
    	for (State* s : m_states) {
    		states_list.push_back(s);
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
		for (State* s : m_states) {
			states_vector.push_back(s);
		}
		return states_vector;
    }

    /**
     * Restituisce l'insieme di etichette usate nelle transizioni di questo automa.
     */
    template <class State>
    const set<string>& Automaton<State>::getLabels() {
        set<string> *labels;
        for (State* s : m_states) {
            for (auto &trans: s->getExitingTransitions()) {
                labels->insert(trans.first);
            }
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
    	return this->connectStates(getState(from), getState(to), label);
    }

    /**
     * Restituisce una descrizione testuale di tutti gli stati appartenenti all'automa,
     * evidenziando lo stato iniziale.
     * Ogni stato deve essere stato istanziato correttamente.
     * Lo stato iniziale deve essere stato impostato correttamente.
     */
    template <class State>
    string Automaton<State>::toString() {
    	string result = "";
    	result += "AUTOMATON (size = " + std::to_string(m_states.size()) + ")\n";
        result += "Starting from state: " + m_initial_state->getName() + '\n';

        // Per ogni stato dell'automa
        for (State* s : m_states) {
            result += s->toString();
        }

        return result;
    }

//    /**
//     * Operatore di uguaglianza per automi.
//     */
//    template <class State>
//    bool Automaton<State>::operator==(Automaton<State>& other) {
//    	// Se gli stati iniziali non sono uguali, certamente i due automi non sono uguali
//        if (*m_initial_state != *other.m_initial_state) {
//        	return false;
//        }
//
//        // Se gli automi non hanno la stessa dimensione (= numero di stati), allora non sono certamente uguali
//        if (m_states.size() != other.m_states.size()) {
//        	return false;
//        }
//
//        // Effettuo il confronto stato per stato dei due automi.
//        // Essendo gli insiemi ordinati, posso basarmi sullo scorrimento degli stati dei due insiemi.
//        // TODO CORREGGERE
//        auto ia = m_states.begin(), ib = other.m_states.begin();
//
//        while (ia != m_states.end() && ib != m_states.end()) {
//            if (*ia != *ib) {
//            	return false;
//            } else if (ia->hasSameTransitions(ib)) {
//            	return false;
//            }
//
//            ++ia; ++ib;
//        }
//
//        return true;
//    }

    /*************
     * Nota: essendo la classe Automaton parametrizzata sul tipo "State",
     * è necessario comunicare al compilatore quali implementazioni verranno utilizzate.
     */
    template class Automaton<StateNFA>;
    template class Automaton<StateDFA>;

} /* namespace translated_automata */
