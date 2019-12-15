/*
 * Properties.hpp
 *
 * File che si occupa di aggregare in un'unica posizione le differenti
 * proprietà utili nel programma.
 */

#ifndef INCLUDE_PROPERTIES_HPP_
#define INCLUDE_PROPERTIES_HPP_

// Numero di test effettuati
#define TESTCASES							5

// Proprietà dell'alfabeto
#define ALPHABET_CARDINALITY 				20

// Proprietà di un automa
#define AUTOMATON_TYPE						DFA_RANDOM
//#define AUTOMATON_TYPE						DFA_STRATIFIED
#define AUTOMATON_SIZE 						100
#define AUTOMATON_FINAL_PROBABILITY 		0.1
#define AUTOMATON_TRANSITION_PERCENTAGE 	0.1
#define AUTOMATON_MAX_DISTANCE			 	5

// Proprietà di una traduzione
#define TRANSLATION_MIXING_FACTOR 			0.9
#define TRANSLATION_OFFSET 					1
#define TRANSLATION_EPSILON_PERCENTAGE		0

// Moduli e funzionalità opzionali
#define DO_PRINT_STATS						true
#define DO_PRINT_TRANSLATION				true
#define DO_PRINT_ORIGINAL_AUTOMATON			false
#define DO_DRAW_ORIGINAL_AUTOMATON			false
#define DO_PRINT_SC_SOLUTION				false
#define DO_DRAW_SC_SOLUTION					false
#define DO_PRINT_ESC_SOLUTION				false
#define DO_DRAW_ESC_SOLUTION				false

#endif /* INCLUDE_PROPERTIES_HPP_ */
