/******************************************************************************
 * bud.hpp
 *
 * Project: TranslatedAutomata
 *
 * Contiene la definizione di un Bud come coppia di ConstructedStateDFA e string.
 * La definizione è stata spostata in un file header apposito per minimizzare le
 * dipendenze fra i vari moduli del progetto, aumentando la granularità delle
 * funzionalità da importare.
 *
 ******************************************************************************/

#ifndef INCLUDE_BUD_HPP_
#define INCLUDE_BUD_HPP_

#include <string>

#include "constructed_state_dfa.hpp"

using std::pair;

namespace translated_automata {

	using Bud = pair<ConstructedStateDFA*, string>;

}

#endif /* INCLUDE_BUD_HPP_ */
