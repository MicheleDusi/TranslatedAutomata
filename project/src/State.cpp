/*
 * State.cpp
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
 */

#include "State.hpp"

#include <list>
#include <map>
#include <set>
#include <string>

#include "Alphabet.hpp"
#include "Debug.hpp"

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

//		// FIXME Migliorare l'implementazione
//		bool flag_found = false;
//		for (auto iterator = this->m_exiting_transitions[label].begin();
//				!flag_new_insertion && iterator != this->m_exiting_transitions[label].end();
//				iterator++) {
//			if ((*iterator)->getName() == child->getName()) {
//				flag_found = true;
//			}
//		}

		// Se la label è un nuovo inserimento || Se per tale label, lo stato non è ancora presente
//		if (flag_new_insertion || !flag_found) {
		if (flag_new_insertion || !this->hasExitingTransition(label, child)) {
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
	 * Nota: questo metodo verrà ereditato dalla classe StateNFA, poiché per un NFA
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
		if (this->m_exiting_transitions.count(label)) {
			return (this->m_exiting_transitions[label].find(child) != this->m_exiting_transitions[label].end());
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
	 * che parta dallo stato "parent" e che sia marcata con l'etichetta "label".
	 */
	template <class S>
	bool State<S>::hasIncomingTransition(string label, S* parent) {
		if (this->m_incoming_transitions.count(label)) {
			return (this->m_incoming_transitions[label].find(parent) != this->m_incoming_transitions[label].end());
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

//////////////////////////////////////////////////////////////////

	/**
	 * Costruttore della classe StateNFA.
	 */
	StateNFA::StateNFA (string name, bool final) : State() {
		m_final = final;
		m_name = name;
	}

	/**
	 * Distruttore della classe StateNFA.
	 */
	StateNFA::~StateNFA () {
		DEBUG_LOG( "Cancellazione dello stato \"%s\"", m_name.c_str() );
	}

///////////////////////////////////////////////////////////////////

	/**
	 * Costruttore della classe StateDFA.
	 */
	StateDFA::StateDFA(string name, bool final) {
		this->m_name = name;
		this->m_final = final;
    }

	/**
	 * Distruttore della classe StateDFA.
	 */
    StateDFA::~StateDFA() {}

	/**
	 * Restituisce lo stato raggiunto da una transizione con una specifica etichetta.
	 * In pratica, fa le stesse operazioni del metodo "getChildren" ma prende solamente il
	 * primo nodo.
	 * Se non viene trovato alcun figlio relativo alla label passata come argomento,
	 * viene restituito un valore nullo.
	 */
	StateDFA* StateDFA::getChild(string label) {
		set<StateDFA*> children = this->getChildren(label);
		if (children.empty()) {
			return NULL;
		} else {
			if (children.size() > 1) {
				DEBUG_LOG_ERROR("Il nodo DFA \"%s\" contiene più di un figlio", this->getName().c_str());
			}
			return *(children.begin());
		}
	}

///////////////////////////////////////////////////////////////////

	/**
	 * Metodo statico.
	 * Crea il nome dello stato concatenando i nomi degli stati dell'estensione.
	 * Questo metodo è utilizzato in automatico per stabilire il nome di un oggetto
	 * ConstructedStateDFA ogni volta che viene assegnata o modificata la sua
	 * estensione.
	 */
	string ConstructedStateDFA::createNameFromExtension(const ExtensionDFA &ext) {
		if (ext.empty()) {
			return EMPTY_EXTENSION_NAME;
		}

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
		ExtensionDFA result = set<StateNFA*, StateNFA::Comparator>();

		for (StateNFA* s : ext1) {
			if (ext2.count(s) == 0) {
				result.insert(s);
			}
		}

		return result;
	}

	/**
	 * Metodo statico.
	 * Computa la epsilon chiusura di un'estensione.
	 */
	ExtensionDFA ConstructedStateDFA::computeEpsilonClosure(const ExtensionDFA &ext) {
		ExtensionDFA result = set<StateNFA*, StateNFA::Comparator>(ext);
		list<StateNFA*> queue = list<StateNFA*>();
		for (StateNFA* s : ext) {
			queue.push_back(s);
		}

		while (!queue.empty()) {
			// Estraggo lo stato da processare
			StateNFA* current = queue.front();
			queue.pop_front();
			// Calcolo gli stati raggiungibili tramite epsilon transitions
			set<StateNFA*> closure = current->getChildren(EPSILON);
			// Per ciascuno di essi
			for (StateNFA* epsilon_child : closure) {
				// Aggiungo lo stato alla epsilon-chiusura dell'estensione
				// Se NON era già contenuto
				if (result.insert(epsilon_child).second) {
					// Allora aggiungo lo stato anche alla coda di stati da processare
					queue.push_back(epsilon_child);
				}
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
	 * Costruttore della classe ConstructedStateDFA.
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
	 * Distruttore della classe ConstructedStateDFA.
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
		set<string> *labels = new set<string>;
		DEBUG_ASSERT_TRUE(labels->size() == 0);

		// Per ciascuno stato dell'estensione
		for (StateNFA* member : m_extension) {
			DEBUG_LOG("Per lo stato dell'estensione \"%s\"", member->getName().c_str());
			// Inserisco le label delle transizioni uscenti
			for (auto &pair: member->getExitingTransitionsRef()) {
				// Se la label marca almeno una transizione
				DEBUG_LOG("Numero di transizioni marcate dalla label %s: %lu", pair.first.c_str(), pair.second.size());
				if (pair.second.size() > 0) {
					DEBUG_LOG("Aggiungo la label \"%s\"", pair.first.c_str());
					labels->insert(pair.first);
				}
			}
		}
		DEBUG_LOG("Lunghezza finale dell'insieme di labels: %lu", labels->size());
		return *labels;
	}

	/**
	 * Restituisce la l-closure data la stringa etichetta:
	 * per tutti gli stati della propria estensione calcola la
	 * l-closure, poi degli stati raggiunti calcola la epsilon-chiusura
	 * Si suppone, pertanto, che l'estensione presente nello stato sia
	 * sempre epsilon-chiusa.
	 */
	ExtensionDFA ConstructedStateDFA::computeLClosureOfExtension(string label) {
//		// Epsilon-chiusura della estensione corrente
//		ExtensionDFA eps_closure = ConstructedStateDFA::computeEpsilonClosure(this->m_extension);

		// Computazione degli stati raggiunti tramite label L
		ExtensionDFA l_closure;
		for (StateNFA* member : this->m_extension) {
			for (StateNFA* child : member->getChildren(label)) {
				l_closure.insert(child);
			}
		}

		// Epsilon chiusura degli stati raggiunti
		return ConstructedStateDFA::computeEpsilonClosure(l_closure);
	}

	/**
	 * Sostituisce interamente l'estensione di questo stato con un'altra.
	 *
	 * Nota: questo metodo causa anche il cambio del nome dello stato, basato
	 * sugli stati dell'NFA che sono contenuti nella nuova estensione.
	 */
	void ConstructedStateDFA::replaceExtensionWith(ExtensionDFA &new_ext) {
		this->m_extension = new_ext;
		this->m_name = createNameFromExtension(m_extension);
		this->m_final = hasFinalStates(m_extension);
	}

	/**
	 * Verifica se lo stato è "vuoto", ossia se la sua estensione è vuota.
	 */
	bool ConstructedStateDFA::isExtensionEmpty() {
		return m_extension.empty();
	}

}
