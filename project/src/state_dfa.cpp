/******************************************************************************
 * state_dfa.cpp
 *
 * Project: TranslatedAutomata
 *
 * Sorgente contenente l'implementazione della classe concreta "StateDFA", figlia
 * della classe astratta "State".
 * Concettualmente, un oggetto "StateDFA" rappresenta uno stato di un DFA, ossia
 * di un automa a stati finiti deterministico.
 * Un oggetto StateDFA ha la caratteristica di avere un solo nodo figlio per
 * ciascuna (eventuale) label. Questo implica che non avrà mai più di N transizioni
 * uscenti, dove N è la cardinalità dell'alfabeto.
 *
 ******************************************************************************/

#include "state_dfa.hpp"

#include <vector>

using std::vector;

namespace translated_automata {

	/**
	 * Costruttore.
	 */
	StateDFA::StateDFA(ExtensionDFA extension, bool processed) {
        m_extension = extension;
        m_name = createNameFromExtension(m_extension);
    }

	/**
	 * Distruttore.
	 */
    StateDFA::~StateDFA() {
        m_extension.clear();
    }

    /**
     * Metodo statico.
     * Crea il nome dello stato concatenando i nomi degli stati dell'estensione.
     */
    string StateDFA::createNameFromExtension(const ExtensionDFA &ext) {
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
     * Sottrae da un'estensione gli stati di un'altra estensione.
     */
    ExtensionDFA StateDFA::subtractExtensions(const ExtensionDFA &ext1, const ExtensionDFA &ext2) {
        ExtensionDFA result;

        for (StateNFA* s : ext1) {
        	if (ext2.count(s) == 0) {
        		result.insert(s);
        	}
        }

        return result;
    }

    /**
     * Verifica se lo stato è finale.
     * Uno stato DFA è finale se contiene nella sua estensione almeno uno stato NFA finale.
     */
    bool StateDFA::isFinal() const {
        for (StateNFA* member : m_extension) {
            if (member->isFinal()) {
                return true;
            }
        }

        return false;
    }

    /**
     * Verifica se lo stato è "vuoto", ossia se la sua estensione è vuota.
     */
    bool StateDFA::isEmpty() const {
        return m_extension.empty();
    }

    /**
     * Restituisce l'estensione dello stato.
     */
    const ExtensionDFA& StateDFA::getExtension() {
        return m_extension;
    }

    /**
     * Sostituisce interamente l'estensione di questo stato con un'altra.
     * Inoltre, per ciascuno stato dell'estensione (vecchia e nuova), vengono aggiornate le intensioni.
     */
    void StateDFA::replaceExtensionWith(ExtensionDFA new_ext) {
        m_extension = new_ext;
        m_name = createNameFromExtension(m_extension);
    }

    /**
     * Restituisce la l-closure data la stringa etichetta:
     * per tutti gli stati della propria estensione, calcola la
     * l-closure, e dopo unisce tutti gli insiemi.
     *
     * Nota: Non è necessario estendere il tutto con epsilon transizioni,
     * poiché esse non vengono considerate.
     */
    ExtensionDFA StateDFA::computeLClosure(string label) {
        ExtensionDFA closure;

        for (StateNFA* member : m_extension) {
            for (StateNFA* child_ : member->getChildren(label)) {
            	StateNFA* child = (StateNFA*) child_;
                closure.insert(child);
            }
        }

        return closure;
    }

    /**
     * Restituisce tutte le etichette delle transizioni uscenti dagli stati
     * dell'estensione.
     */
    set<string> StateDFA::getExitingLabels() {
        set<string> labels;

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
     * Guarda tutte le transizioni uscenti dallo stato "state".
     * Se trova una transizione che questo stato non possiede,
     * la aggiunge come transizione propria.
     * Al termine dell'esecuzione, è garantito che questo stato
     * contenga almeno tutte le transizioni uscenti dallo stato
     * passato come parametro.
     */
    void StateDFA::copyExitingTransitionsOf(StateDFA* state) {
    	// Per tutte le transizioni uscenti dallo stato "state"
        for (auto &pair: state->getExitingTransitions()) {
            string label = pair.first;
            for (StateDFA* child: pair.second) {
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
    void StateDFA::copyIncomingTransitionsOf(StateDFA* state) {
    	// Per tutte le transizioni entranti nello stato "state"
        for (auto &pair: state->getIncomingTransitions()) {
            string label = pair.first;
            for (State* parent: pair.second) {
                if (!parent->hasExitingTransition(label, this)) {
                    parent->connectChild(label, this);
                }
            }
        }
    }

    /**
     * Copia tutte le transizioni entranti in e uscenti da uno stato d
     * all'interno di questo stato.
     * Le transizioni già esistenti non vengono duplicate.
     */
    void StateDFA::copyAllTransitionsOf(StateDFA* state) {
        copyIncomingTransitionsOf(state);
        copyExitingTransitionsOf(state);
    }

    /**
     * Restituisce la distanza di questo stato.
     */
    unsigned int StateDFA::getDistance() {
        return m_distance;
    }

    /**
     * Imposta la distanza di questo stato.
     */
    void StateDFA::setDistance(int distance) {
        m_distance = distance;
    }

    /**
     * Imposta i valori delle distanze partendo dallo stato corrente
     * come radice, considerando ongi transizione come un incremento unitario sulla
     * distanza.
     * Nota: questo metodo funziona unicamente all'inizio, e imposta le distanze diverse dal
     * valore iniziale "DEFAULT_VOID_DISTANCE". Per impostare le distanze in modo differente,
     * sovrascrivendo solo le distanze migliori, è meglio utilizzare il metodo "setBetterDistancesRecursively".
     */
    void StateDFA::initDistancesRecursively(int root_distance) {
    	// Imposto la distanza corrente
        this->setDistance(root_distance);
        // Preparo la lista di nodi su cui effettuare l'aggiornamento della distanza
        vector<StateDFA*> updated_list;
        updated_list.push_back(this);

        while (updated_list.size() > 0) {		// Finché la queue non è vuota

            StateDFA* current_state = updated_list.front();
            updated_list.erase(updated_list.begin());

            // Per tutte le transizioni uscenti dallo stato corrente
            for (auto &trans: current_state->getExitingTransitionsRef()) {
            	// Per tutti i figli raggiunti
                for (State* child_ : trans.second) {
                	StateDFA* child = (StateDFA*) child_;
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
     * Imposta i valori delle distanze partendo dallo stato corrente
     * come radice, considerando ongi transizione come un incremento unitario sulla
     * distanza.
     * Nota: questo metodo funziona unicamente se deve sovrascrivere distanze con valori inferiori
     * ai precedenti. In caso una distanza sia già "bassa", un valore più alto non ne provocherà il
     * cambiamento.
     * Il metodo implementa un'algoritmo di flooding in profondità.
     */
    void StateDFA::setBetterDistancesRecursively(int root_distance) {
    	// Verifico che la distanza sia migliore della precedente
    	if (this->getDistance() > root_distance) {
    		this->setDistance(root_distance);

    		// Per tutte le transizioni uscenti dallo stato corrente
			for (auto &trans : this->getExitingTransitionsRef()) {
				// Per tutti i figli raggiunti
				for (State* child_ : trans.second) {
                	StateDFA* child = (StateDFA*) child_;
					// Se la distanza nuova è migliore
					if (child->getDistance() > root_distance + 1) {
						// Richiamo ricorsivamente la funzione
						child->setBetterDistancesRecursively(root_distance + 1);
					}
				}
			}
    	}
    }

    /**
     * Restituisce la minima distanza fra tutte le distanze dei genitori.
     */
    int StateDFA::getMinimumParentsDistance() {
    	int minimum = DEFAULT_VOID_DISTANCE;
    	// Per tutte le transizioni entranti
    	for (auto &pair : this->getIncomingTransitionsRef()) {
    		// Per tutti gli stati genitori
    		for (StateDFA* parent : pair.second) {
    			// Se la distanza è inferiore
    			if (parent->m_distance < minimum) {
    				minimum = parent->m_distance;
    			}
    		}
    	}
    	return minimum;
    }

    /**
     * Verifica se lo stato ha una specifica estensione passata come parametro.
     * Il confronto viene effettuato mediante la stringa contenente tutti i nomi dell'estensione.
     */
    bool StateDFA::hasExtension(const ExtensionDFA &ext) {
        return (getName() == createNameFromExtension(ext));
    }

}
