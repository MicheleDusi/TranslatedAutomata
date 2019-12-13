/*
 * ResultCollector.hpp
 *
 * Modulo che si occupa di archiviare i risultati di un problema.
 * Permette di effettuare semplici analisi statistiche su di essi, ottenendo
 * dati aggregati sulle performance del processo di risoluzione di un problema.
 */

#ifndef INCLUDE_RESULTCOLLECTOR_HPP_
#define INCLUDE_RESULTCOLLECTOR_HPP_

namespace translated_automata {

	class ResultCollector {
	public:
		ResultCollector();
		virtual ~ResultCollector();
	};

} /* namespace translated_automata */

#endif /* INCLUDE_RESULTCOLLECTOR_HPP_ */
