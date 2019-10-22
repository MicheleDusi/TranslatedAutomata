/******************************************************************************
 * translation.hpp
 *
 * Project: TranslatedAutomata
 *
 * File header per le firme dei metodi della classe Translation.
 * La classe modellizza una traduzione operata su un alfabeto, ed è implementata come
 * mappa che associa ciascun valore di un alfabeto ad un altro.
 *
 * NOTA: La mappa mantiene solamente le associazioni differenti dall'identità. Per qualunque
 * label non memorizzata viene assunta la mappa identità, ossia la traduzione non ha effetto.
 *
 * Inoltre, la classe Translation offre il metodo che implementa l'algoritmo di traduzione
 * di un automa; questo metodo prende in ingresso un automa e applica a ciascuna transizione
 * la mappa della traduzione. Produce in output l'automa tradotto NFA, un automa isomorfo di
 * tipo DFA (ma che contiene ancora dei punti di non-determinismo), e la sequenza di bud generatasi
 * durante la traduzione.
 *
 ******************************************************************************/

#ifndef INCLUDE_TRANSLATION_HPP_
#define INCLUDE_TRANSLATION_HPP_

#include <map>
#include <vector>

#include "alphabet.hpp"
#include "automaton_nfa.hpp"
#include "automaton_dfa.hpp"

using namespace std;

namespace translated_automata {

	using Bud = pair<StateDFA*, string>;

	class Translation {

	private:
		map<string, string> m_translation_map;

	public:
		Translation(Alphabet alpha, map<string, string> translation_map);
		Translation(map<string, string> translation_map);
		virtual ~Translation();

		string translate(string label);
		Alphabet translate(Alphabet alpha);
		template <class State> tuple<NFA*, DFA*, vector<Bud>> translate(Automaton<State> *automaton);

		string toString();

	};

} /* namespace translated_automata */

#endif /* INCLUDE_TRANSLATION_HPP_ */
