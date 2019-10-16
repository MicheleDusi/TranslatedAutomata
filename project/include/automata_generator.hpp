/******************************************************************************
 * automata_generator.cpp
 *
 * Project: TranslatedAutomata
 *
 * File header della classe AutomataGenerator, avente la responsabilità
 * di generare automi di differenti tipologie e caratteristiche, a seconda dei
 * parametri richiesti nella chiamata dei metodi.
 * La classe AutomataGenerator è parametrizzata sul tipo "State"; il suo metodo
 * "generateRandomAutomaton" restituisce un automa parametrizzato sullo stesso tipo.
 * Il metodo, così come il costruttore, sono virtuali; questo fa della classe una
 * classe astratta che necessita di un'implementazione concreta per essere usata.
 *
 ******************************************************************************/

#ifndef INLCUDE_AUTOMATA_GENERATOR_HPP_
#define INCLUDE_AUTOMATA_GENERATOR_HPP_

#include "automaton.hpp"

namespace translated_automata {

	/**
	 * Un alfabeto è una collezione di "simboli". Ogni simbolo
	 * viene implementato, in questo caso, come stringa.
	 * Questa soluzione permette una maggiore versatilità
	 * e non limita la cardinalità dell'alfabeto al numero di
	 * lettere dell'alfabeto latino.
	 */
	using Alphabet = set<string>;

	template <class State>
	class AutomataGenerator {

	private:
		constexpr static char letters[] = "abcdefghijklmnopqrstuvwxyz";
		constexpr static int default_alphabet_cardinality = 1;
		constexpr static int default_size = 2;
		constexpr static double default_transition_percentage = 0.5;
		constexpr static double default_final_percentage = 0.1;

		Alphabet m_alphabet;
		int m_size 							= default_size;
		double m_transition_percentage 		= default_transition_percentage;
		double m_final_percentage 			= default_final_percentage;

		Alphabet generateAlphabet(const char *chars, int chars_size, int cardinality);
		void resetNames();
		string generateUniqueName();

	public:
		AutomataGenerator();
		virtual ~AutomataGenerator();

		void setAlphabet(Alphabet alpha);
		void setAlphabetCardinality(int size);
		void setSize(int size);
		void setTransitionPercentage(double percentage);
		void setFinalPercentage(double percentage);

		virtual Automaton<State> * generateRandomAutomaton();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_AUTOMATA_GENERATOR_HPP_ */
