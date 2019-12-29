/*
 * Properties.hpp
 *
 * File che si occupa di aggregare in un'unica posizione le differenti
 * proprietà utili nel programma.
 */

#ifndef INCLUDE_PROPERTIES_HPP_
#define INCLUDE_PROPERTIES_HPP_

// Numero di test effettuati
#define TESTCASES							10

// Proprietà del problema
//#define PROBLEM_TYPE						TRANSLATION_PROBLEM						// Si parte da un DFA e da una traduzione
#define PROBLEM_TYPE						DETERMINIZATION_PROBLEM					// Si parte da un NFA

// Proprietà dell'alfabeto
#define ALPHABET_CARDINALITY 				5

// Proprietà di un automa
//#define AUTOMATON_TYPE						AUTOMATON_RANDOM,					// Automa generato casualmente, senza una precisa struttura (solo DFA)
//#define AUTOMATON_TYPE						AUTOMATON_STRATIFIED,				// Automa a strati, con distanze crescenti (solo DFA)
#define AUTOMATON_TYPE						AUTOMATON_STRATIFIED_WITH_SAFE_ZONE		// Automa a strati, con punti di non determinismo solo oltre una certa distanza (solo NFA)
#define AUTOMATON_SIZE 						100
#define AUTOMATON_FINAL_PROBABILITY 		0.1
#define AUTOMATON_TRANSITION_PERCENTAGE 	0.5
#define AUTOMATON_MAX_DISTANCE			 	10
#define AUTOMATON_SAFE_ZONE_DISTANCE		8

// Proprietà di una traduzione
#define TRANSLATION_MIXING_FACTOR 			0.9
#define TRANSLATION_OFFSET 					1
#define TRANSLATION_EPSILON_PERCENTAGE		0.3

// Moduli e funzionalità opzionali
#define DO_AUTOMATON_PRUNING				true		// In caso sia attivato, evita la formazione e la gestione dello stato con estensione vuota, tramite procedura Automaton Pruning
#define DO_USE_REMOVING_LABEL				true		// In caso sia attivato, utilizza una label apposita per segnalare le epsilon-transizione, che deve essere rimossa durante la determinizzazione
#define DO_CHECK_DISTANCE_IN_TRANSLATION	true		// In caso sia attivato, durante la traduzione genera dei Bud solamente se gli stati soddisfano una particolare condizione sulla distanza [FIXME è una condizione che genera bug]

#define DO_PRINT_STATS						true
#define DO_PRINT_TRANSLATION				false
#define DO_PRINT_ORIGINAL_AUTOMATON			false
#define DO_DRAW_ORIGINAL_AUTOMATON			false
#define DO_PRINT_SC_SOLUTION				false
#define DO_DRAW_SC_SOLUTION					false
#define DO_PRINT_ESC_SOLUTION				false
#define DO_DRAW_ESC_SOLUTION				false

// Risultati, cartelle e nomi dei files
#define DIR_RESULTS 						"results/"
#define FILE_NAME_ORIGINAL_AUTOMATON 		"original"
#define FILE_NAME_SC_SOLUTION 				"sc_solution"
#define FILE_NAME_ESC_SOLUTION 				"esc_solution"
#define FILE_EXTENSION_GRAPHVIZ 			".gv"
#define FILE_EXTENSION_PDF 					".pdf"

#endif /* INCLUDE_PROPERTIES_HPP_ */
