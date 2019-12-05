/******************************************************************************
 * state.cpp
 *
 * Project: TranslatedAutomata
 *
 * File sorgente per l'implementazione della classe astratta "State", definita
 * nel file header "state.hpp".
 * Nel dominio concettuale, uno "State" è uno stato di un "automa a stati finiti",
 * sia esso deterministico o non deterministico. E' caratterizzato da un nome
 * univoco ed è legato ad altri stati mediante transizioni, ciascuna delle quali
 * è marcata da una label.
 *
 ******************************************************************************/

#include "state.hpp"

#include <list>
#include <map>
#include <set>
#include <string>

#include "alphabet.hpp"
#include "debug.hpp"
#include "state_dfa.hpp"
#include "state_nfa.hpp"

using std::list;
using std::map;
using std::set;
using std::string;

namespace translated_automata {

	/**
	 * Classe astratta "State".
	 * Viene implementata da StateNFA e StateDFA.
	 */

	/**
	 * Costruttore della classe State.
	 * Inizializza come vuoti gli insiemi di transizioni entranti e uscenti.
	 */
	template <class S>
	State<S>::State ()
	: m_exiting_transitions(), m_incoming_transitions() {
		DEBUG_LOG_SUCCESS( "Nuovo oggetto State creato correttamente" );
	}

	/**
	 * Distruttore della classe State.
	 */
	template <class S>
	State<S>::~State () {
		DEBUG_LOG( "Distruzione dell'oggetto State \"%s\"", m_name.c_str() );
	}

	/**
	 * Metodo privato.
	 * Restituisce il puntatore all'oggetto corrente, automaticamente
	 * castato con il tipo corretto.
	 */
	template <class S>
	S* State<S>::getThis() const {
		return (S*) this;
	}

	/**
	 * Restituisce il nome dello stato.
	 */
	template <class S>
	string State<S>::getName() const {
		return m_name;
	}

	/**
	 * Restituisce TRUE se lo stato è marcato come stato finale.
	 */
	template <class S>
	bool State<S>::isFinal() {
		return m_final;
	}

	/**
	 * Imposta lo stato come FINAL oppure NON_FINAL,
	 * a seconda del valore passato come parametro.
	 */
	template <class S>
	void State<S>::setFinal(bool final) {
		m_final = final;
	}

	/**
	 * Collega lo stato soggetto allo stato passato come parametro, con una transizione
	 * etichettata dalla label "label" passata come parametro.
	 * Nota: se la transizione esiste già, non viene aggiunta nuovamente.
	 */
	template <class S>
	void State<S>::connectChild(string label, S* child)	{
		bool flag_new_insertion = false;
		// Verifico se la label ha già un set associato nello stato CORRENTE.
		// In caso il set non ci sia, viene creato
		if (this->m_exiting_transitions.count(label) == 0) {
			this->m_exiting_transitions[label] = std::set<S*>();
			flag_new_insertion = true;
		}

		// Verifico se la label ha già un set associato nello stato FIGLIO
		// In caso il set non ci sia, viene creato
		if (child->m_incoming_transitions.count(label) == 0) {
			child->m_incoming_transitions[label] = std::set<S*>();
			flag_new_insertion = true;
		}

		// FIXME Migliorare l'implementazione
		bool flag_found = false;
		for (auto iterator = this->m_exiting_transitions[label].begin();
				!flag_new_insertion && iterator != this->m_exiting_transitions[label].end();
				iterator++) {
			if ((*iterator)->getName() == child->getName()) {
				flag_found = true;
			}
		}

		// Se la label è un nuovo inserimento || Se per tale label, lo stato non è ancora presente
		if (flag_new_insertion || !flag_found) {
			// Aggiungo una transizione uscente da questo stato
			this->m_exiting_transitions[label].insert(child);
			// Aggiungo una transizione entrante allo stato di arrivo
			child->m_incoming_transitions[label].insert(getThis());
		}
	}

	/**
	 * Disconnette due stati, rimuovendo la l-transizione che parte da questo
	 * stato e arriva in quello passato come parametro.
	 *
	 * Precondizione: si suppone che tale transizione sia esistente
	 */
	template <class S>
	void State<S>::disconnectChild(string label, S* child) {
		// Ricerca del figlio da disconnettere
		auto iterator = this->m_exiting_transitions[label].find(child);

		if (iterator != this->m_exiting_transitions[label].end()) {
			this->m_exiting_transitions[label].erase(iterator);
			child->m_incoming_transitions[label].erase(getThis());
		}

		// Se le transizioni uscenti per la label si svuotano, elimino il set vuoto di stati
		if (m_exiting_transitions[label].empty()) {
			m_exiting_transitions.erase(label);
		}
		// Se le transizioni entranti del figlio per la label si svuotano, elimino il set vuoto di stati
		if (child->m_incoming_transitions[label].empty()) {
			child->m_incoming_transitions.erase(label);
		}
	}

	/**
	 * Rimuove tutte le transizioni entranti e uscenti
	 * da questo stato.
	 * Le transizioni vengono aggiornate anche sui nodi che
	 * risultavano precedentemente connessi.
	 */
	template <class S>
	void State<S>::detachAllTransitions() {
		// Rimuove le transizioni uscenti
		for (auto &pair: m_exiting_transitions) {
			string label = pair.first;
			for (S* child : pair.second) {
				getThis()->disconnectChild(label, child);
			}
		}

		// Rimuove le transizioni entranti
		for (auto &pair: m_incoming_transitions) {
			string label = pair.first;
			for (S* parent: pair.second) {
				parent->disconnectChild(label, getThis());
			}
		}
	}

	/**
	 * Metodo protetto.
	 * Restituisce l'insieme di stati raggiunti da tutte le transizioni
	 * uscenti da questo nodo marcate con la label "l" passata come parametro.
	 * In pratica, restituisce la l-closure di questo nodo.
	 *
	 * Nota: questo metodo verrà "wrappato" dalla classe StateNFA, poiché per un NFA
	 * è possibile avere più figli marcati con la stessa label. Nel caso di uno stato
	 * StateDFA, invece, sarà opportuno operare alcuni controlli per verificare che
	 * esista un unico figlio per ciascuna label.
	 */
	template <class S>
	set<S*> State<S>::getChildren(string label) {
		auto search = m_exiting_transitions.find(label);
		// Con "auto" sto esplicitando il processo di type-inference
		if (search != m_exiting_transitions.end()) {
			// Restituisco i nodi alla transizione uscente
			return m_exiting_transitions[label];
		} else {
			// Restituisco un insieme vuoto
			return set<S*>();
		}
	}

	/**
	 * Restituisce gli stati che hanno un transizione marcata da una specifica label
	 * che punta a questo stato. In pratica, tutti gli stati "padri" secondo una certa
	 * label.
	 */
	template <class S>
	set<S*> State<S>::getParents(string label) {
		auto search = m_incoming_transitions.find(label);
		if (search != m_incoming_transitions.end()) {
			return m_incoming_transitions[label];
		} else {
			return set<S*>();
		}
	}

	/**
	 * Verifica se lo stato soggetto ha una transizione USCENTE
	 * marcata con la label passata come parametro.
	 */
	template <class S>
	bool State<S>::hasExitingTransition(string label)	{
		auto search = m_exiting_transitions.find(label);
		return search != m_exiting_transitions.end();
	}

	/**
	 * Verifica se lo stato soggetto ha una transizione USCENTE che vada
	 * allo stato "child" e che sia marcata con l'etichetta "label".
	 */
	template <class S>
	bool State<S>::hasExitingTransition(string label, S* child) {
		auto search = m_exiting_transitions.find(label);
		if (search != m_exiting_transitions.end()) {
			return search->second.count(child);
		} else {
			return false;
		}
	}

	/**
	 * Verifica se lo stato soggetto ha una transizione ENTRANTE
	 * marcata con la label passata come parametro.
	 */
	template <class S>
	bool State<S>::hasIncomingTransition(string label)	{
		auto search = m_incoming_transitions.find(label);
		return search != m_incoming_transitions.end();
	}

	/**
	 * Verifica se lo stato soggetto ha una transizione ENTRANTE
	 * che parta dallo stato "child" e che sia marcata con l'etichetta "label".
	 */
	template <class S>
	bool State<S>::hasIncomingTransition(string label, S* child) {
		auto search = m_incoming_transitions.find(label);
		if (search != m_incoming_transitions.end()) {
			return search->second.count(child);
		} else {
			return false;
		}
	}

	/**
	 * Restituisce la mappa di transizioni uscenti da questo stato.
	 */
	template <class S>
	map<string, set<S*>> State<S>::getExitingTransitions() {
		return m_exiting_transitions;
	}

	/**
	 * Restituisce la mappa di transizioni entranti in questo stato.
	 */
	template <class S>
	map<string, set<S*>> State<S>::getIncomingTransitions() {
		return m_incoming_transitions;
	}

	/**
	 * Restituisce una reference alla mappa di transizioni, ossia l'indirizzo della memoria
	 * in cui la mappa è salvata.
	 * Restituire un indirizzo permette di usare questo metodo come lvalue in un assegnamento, ad esempio.
	 */
	template <class S>
	const map<string, set<S*>>& State<S>::getExitingTransitionsRef() {
		return m_exiting_transitions;
	}

	/**
	 * Restituisce una reference alla mappa di transizioni entranti, ossia l'indirizzo della memoria
	 * in cui la mappa è salvata.
	 * Restituire un indirizzo permette di usare questo metodo come lvalue in un assegnamento, ad esempio.
	 */
	template <class S>
	const map<string, set<S*>>& State<S>::getIncomingTransitionsRef() {
		return m_incoming_transitions;
	}

	/**
	 * Conta le transizioni uscenti dallo stato.
	 * Per ciascuna label, conteggia la quantità di transizioni riferite a tale label uscenti
	 * dallo stato corrente.
	 */
	template <class S>
	int State<S>::getExitingTransitionsCount() {
		int count = 0;
		for (auto &pair: m_exiting_transitions) {// Ciclo su tutti gli elementi della mappa
			count += pair.second.size();
		}
		// "second" prende il secondo elemento della coppia,
		// in questo caso l'insieme di stati raggiunti dalle transizioni con label predefinita
		return count;
	}

	/**
	 * Conta le transizioni entranti nello stato.
	 */
	template <class S>
	int State<S>::getIncomingTransitionsCount() {
		int count = 0;
		for (auto &pair: m_incoming_transitions) {
			count += pair.second.size();
		}
		return count;
	}
    /**
     * Guarda tutte le transizioni uscenti dallo stato "state".
     * Se trova una transizione che questo stato non possiede,
     * la aggiunge come transizione propria.
     * Al termine dell'esecuzione, è garantito che questo stato
     * contenga almeno tutte le transizioni uscenti dallo stato
     * passato come parametro.
     */
	template <class S>
	void State<S>::copyExitingTransitionsOf(S* state) {
    	// Per tutte le transizioni uscenti dallo stato "state"
        for (auto &pair: state->getExitingTransitionsRef()) {
            string label = pair.first;
            for (S* child: pair.second) {
                if (!this->hasExitingTransition(label, child)) {
                    this->connectChild(label, child);
                }
            }
        }
    }

    /**
     * Guarda tutte le transizioni entranti nello stato "state".
     * Se trova una transizione che questo stato non possiede,
     * la aggiunge come transizione propria.
     * Al termine dell'esecuzione, è garantito che questo stato
     * contenga almeno tutte le transizioni entranti nello stato
     * passato come parametro.
     */
	template <class S>
	void State<S>::copyIncomingTransitionsOf(S* state) {
    	// Per tutte le transizioni entranti nello stato "state"
        for (auto &pair: state->getIncomingTransitionsRef()) {
            string label = pair.first;
            for (S* parent: pair.second) {
                if (!parent->hasExitingTransition(label, this->getThis())) {
                    parent->connectChild(label, this->getThis());
                }
            }
        }
    }

    /**
     * Copia tutte le transizioni entranti in e uscenti da uno stato d
     * all'interno di questo stato.
     * Le transizioni già esistenti non vengono duplicate.
     */
	template <class S>
	void State<S>::copyAllTransitionsOf(S* state) {
        copyIncomingTransitionsOf(state);
        copyExitingTransitionsOf(state);
    }

	/**
	 * Verifica se lo stato ha le stesse transizioni (entranti E uscenti) dello stato
	 * passato come parametro.
	 * Le transizioni vengono confrontate tramite puntatore; gli stati devono perciò essere
	 * effettivamente gli stessi.
	 */
	template <class S>
	bool State<S>::hasSameTransitionsOf(S* other_state) {
		// Verifico che il numero di transizioni uscenti sia uguale
		if (this->m_exiting_transitions.size() != other_state->m_exiting_transitions.size()) {
			return false;
		}

		// Per tutte le transizioni uscenti
		for (auto &pair: m_exiting_transitions) {
			string label = pair.first;
			set<S*> other_children = other_state->m_exiting_transitions[label];

			// Verifico che il numero di figli sia uguale
			if (pair.second.size() != other_children.size()) {
				return false;
			}

			// Verifico che tutti i figli siano coincidenti
			for (auto &child: pair.second) {
				// Se il figlio non è contenuto nell'altra lista
				if (other_children.count(child) <= 0) {
					return false;
				}
			}
		}

		// Verifico che il numero di transizioni entranti sia uguale
		if (this->m_incoming_transitions.size() != other_state->m_incoming_transitions.size()) {
			return false;
		}

		// Per tutte le transizioni entranti
		for (auto &pair: m_incoming_transitions) {
			string label = pair.first;
			set<S*> other_parents = other_state->m_incoming_transitions[label];

			// Verifico che il numero di padri sia uguale
			if (pair.second.size() != other_parents.size()) {
				return false;
			}

			// Verifico che tutti i padri siano coincidenti
			for (auto &parent: pair.second) {
				// Se il padre non è contenuto nell'altra lista
				if (other_parents.count(parent) <= 0) {
					return false;
				}
			}
		}
		return true;
	}

	/**
	 * Metodo che verifica se le transizioni presenti nello stato corrispondono (solo
	 * a livello di nome) con le transizioni dello stato passato come parametro.
	 * Questo metodo è utilizzato per confrontare due automi isomorfi, senza che siano
	 * presenti stati in comune fra essi, ma solamente stati omonimi.
	 */
	template <class S>
	bool State<S>::hasSameTransitionsNamesOf(S* other_state) {
		// Verifico che il numero di transizioni uscenti sia uguale
		if (this->m_exiting_transitions.size() != other_state->m_exiting_transitions.size()) {
			return false;
		}

		// Per tutte le transizioni uscenti
		for (auto &pair : m_exiting_transitions) {
			string label = pair.first;
			set<S*> other_children = other_state->m_exiting_transitions[label];

			// Verifico che il numero di figli sia uguale
			if (pair.second.size() != other_children.size()) {
				return false;
			}

			// Verifico che tutti i figli siano coincidenti
			for (auto &child : pair.second) {
				// Se il figlio non è contenuto nell'altra lista
				bool found_flag = false;
				for (auto other_child : other_children) {
					// Appena trovo una corrispondenza, esco dal ciclo
					if (*child == *other_child) {
						found_flag = true;
						break;
					}
				}
				// Se non ho trovato corrispondenze, esco
				if (!found_flag) {
					return false;
				}
			}
		}

		// Verifico che il numero di transizioni entranti sia uguale
		if (this->m_incoming_transitions.size() != other_state->m_incoming_transitions.size()) {
			return false;
		}

		// Per tutte le transizioni entranti
		for (auto &pair: m_incoming_transitions) {
			string label = pair.first;
			set<S*> other_parents = other_state->m_incoming_transitions[label];

			// Verifico che il numero di padri sia uguale
			if (pair.second.size() != other_parents.size()) {
				return false;
			}

			// Verifico che tutti i genitori siano coincidenti
			for (auto &parent : pair.second) {
				// Se il genitore non è contenuto nell'altra lista
				bool found_flag = false;
				for (auto other_parent : other_parents) {
					// Appena trovo una corrispondenza, esco dal ciclo
					if (*parent == *other_parent) {
						found_flag = true;
						break;
					}
				}
				// Se non ho trovato corrispondenze, esco
				if (!found_flag) {
					return false;
				}
			}
		}
		return true;
	}

    /**
     * Restituisce la distanza di questo stato.
     */
	template <class S>
    unsigned int State<S>::getDistance() {
        return m_distance;
    }

    /**
     * Imposta la distanza di questo stato.
     */
	template <class S>
    void State<S>::setDistance(unsigned int distance) {
        m_distance = distance;
    }

    /**
     * Imposta i valori delle distanze partendo dallo stato corrente
     * come radice, considerando ongi transizione come un incremento unitario sulla
     * distanza.
     * In realtà l'assegnamento delle distanze non è implementato con un algoritmo ricorsivo, ma si utilizza
     * una coda che permette un'attraversamento dell'automa di tipo breadth-first (attraversamento in ampiezza).
     * Nota: questo metodo viene richiamato appena terminata la costruzione di un automa, per definire le distanze
     * di tutti gli stati; è quindi importante che tutti gli stati facciano parte dello stesso automa, e che tutti abbiano
     * una distanza non ancora inizializzata (ossia uguale al valore di default).
     */
	template <class S>
    void State<S>::initDistancesRecursively(int root_distance) {
    	// Imposto la distanza corrente
        this->setDistance(root_distance);
        // Preparo la lista di nodi su cui effettuare l'aggiornamento della distanza
        list<S*> updated_list;
        updated_list.push_back(this->getThis());

        while (updated_list.size() > 0) {		// Finché la queue non è vuota

            S* current_state = updated_list.front();
            updated_list.pop_front();

            // Per tutte le transizioni uscenti dallo stato corrente
            for (auto &trans: current_state->getExitingTransitionsRef()) {
            	// Per tutti i figli raggiunti
                for (S* child : trans.second) {
                	// Se la distanza non vale quanto la distanza iniziale di default
                    if (child->getDistance() == DEFAULT_VOID_DISTANCE) {
                    	// Imposto la distanza del figlio = 1 + dist(padre)
                        child->setDistance(current_state->getDistance() + 1);
                        // Aggiungo il figlio alla lista, in modo da aggiornarlo in futuro
                        updated_list.push_back(child);
                    }
                }
            }
        }
    }

    /**
     * Restituisce la minima distanza fra tutte le distanze dei genitori.
     */
	template <class S>
    int State<S>::getMinimumParentsDistance() {
    	int minimum = DEFAULT_VOID_DISTANCE;
    	// Per tutte le transizioni entranti
    	for (auto &pair : this->getIncomingTransitionsRef()) {
    		// Per tutti gli stati genitori
    		for (S* parent : pair.second) {
    			// Se la distanza è inferiore
    			if (parent->m_distance < minimum) {
    				minimum = parent->m_distance;
    			}
    		}
    	}
    	return minimum;
    }

	/**
	 * Restituisce una stringa contenente tutte le informazioni relative allo stato.
	 */
	template <class S>
	string State<S>::toString() const {
		// Stringa che verrà restituita
		string result = "";

		// Inserisco il nome dello stato
		result += "\033[33;1m" + getThis()->getName() + "\033[0m";

		// Distanza dello stato
		result += " (dist = " + std::to_string(m_distance) + ")";

		// Se lo stato è final, aggiungo un'etichetta alla stringa visualizzata
		if (getThis()->isFinal()) {
			result += " [FINAL]";
		}

		result += "\n\t" + (std::to_string(getThis()->getExitingTransitionsCount())) + " exiting transitions:\n";
		if (!this->m_exiting_transitions.empty()) {
			// Per tutte le label delle transizioni uscenti
			for (auto &pair: m_exiting_transitions) {
				string label = pair.first;
				// Per tutti gli stati associati ad una label
				for (S* state: pair.second) {
					// Inserisco le informazioni riguardanti la transizione uscente
					result += "\t━━┥" + SHOW(label) + "┝━━▶ " + state->getName() + "\n";
//					result += "\t━━┥" + SHOW(label) + "┝━━▶ " + state->getName() + "\033[35m[id = " + (std::to_string((long int)state)) + "]\033[0m" + "\n";
				}
			}
		}
/*
		result += "\n\t" + (std::to_string(getThis()->getIncomingTransitionsCount())) + " incoming transitions:\n";
		if (!this->m_incoming_transitions.empty()) {
			// Per tutte le label delle transizioni entranti
			for (auto &pair: m_incoming_transitions) {
				string label = pair.first;
				// Per tutti gli stati associati ad una label
				for (S* state: pair.second) {
					// Inserisco le informazioni riguardanti la transizione entrante
					result += "\t\t" + state->getName() + " ━━(" + SHOW(label) + ")━━▶" + '\n';
				}
			}
		}
*/

		return result;
	};

	/**
	 * Definisce un operatore "<" di confronto, basato sul confronto dei nomi.
	 */
	template <class S>
	bool State<S>::operator<(const S &other) const	{
		return getThis()->getName() < other.getName();
	}

	/**
	 * Definisce un operatore di uguaglianza "==", basato sull'uguaglianza dei nomi.
	 */
	template <class S>
	bool State<S>::operator==(const S &other) const {
		return getThis()->getName() == other.getName();
	}

	/**
	 * Definisce un operatore di disuguaglianza "!=", basato sul confronto dei nomi.
	 */
	template <class S>
	bool State<S>::operator!=(const S &other) const {
			return getThis()->getName() != other.getName();
	}

	/**
	 * Confronta due stati sulla base del loro nome.
	 */
//	template <class S>
//	int State<S>::compareNames(const S &other) const {
//		return getThis()->getName().compare(other.getName());
//	}

    /*************
     * Nota: essendo la classe State parametrizzata sul tipo "S",
     * è necessario comunicare al compilatore quali implementazioni verranno utilizzate.
     */
    template class State<StateNFA>;
    template class State<StateDFA>;

}
