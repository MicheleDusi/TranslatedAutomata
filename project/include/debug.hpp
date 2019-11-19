/******************************************************************************
 * debug.hpp
 *
 * Project: TranslatedAutomata
 *
 * Semplice libreria che fornisce una comoda implementazione
 * delle più comuni funzioni di debug sotto forma di MACRO.
 *
 * Per attivare la modalità di debug -> agire sulla macro
 * DEBUG_MODE facendo in modo che sia definita (si veda sotto).
 * Per disattivare la modalità di debug -> agire sulla macro
 * DEBUG_MODE commentando la riga della sua definizione.
 *
 ******************************************************************************/

#ifndef INCLUDE_DEBUG_HPP_
#define INCLUDE_DEBUG_HPP_

#include <iostream>
#include <stdio.h>
#include <string>

namespace translated_automata {

	/**
	 * Attiva o disattiva la modalità di DEBUG, a seconda che sia
	 * rispettivamente decommentato o commentato.
	 */
//	#define DEBUG_MODE

	/***********************************************************/

	// Utilità

	/** Concatenazione dei valori di due o più parametri x, y, z */
	#define _CONCAT( x, y )				x ## y
	#define CONCAT( x, y )				_CONCAT( x, y )

	/** "Stringhificazione" del valore di un parametro x */
	#define _STRINGIFY( x ) 			#x
	#define STRINGIFY( x )				_STRINGIFY( x )

	/** Prefisso per la generazione di ID unici */
	#define ID_PREFIX 					debug_var_

	/** Macro per la generazione di ID unici.*/
	#define UNIQUE_ID					CONCAT( ID_PREFIX ## __FILE__, __LINE__ )

	/** Aggiunge delle parentesi per identificare un tag nei messaggi di log */
	#define TAG_BRACKETS( text ) 		"[" text "]"

	/** Definizione di alcuni colori di uso comune secondo la codifica ANSI ESCAPE */
	#define COLOR_RED( text ) 			"\033[1;31m" text "\033[0m"
	#define COLOR_GREEN( text ) 		"\033[1;32m" text "\033[0m"
	#define COLOR_YELLOW( text ) 		"\033[1;33m" text "\033[0m"
	#define COLOR_PURPLE( text ) 		"\033[1;35m" text "\033[0m"
	#define COLOR_CYAN( text ) 			"\033[1;36m" text "\033[0m"

	/** Tag facilmente identificabili per i messaggi di log */
	#define DEBUG_TAG 					TAG_BRACKETS( COLOR_CYAN  ( "DEBUG"   ) )
	#define SUCCESS_TAG 				TAG_BRACKETS( COLOR_GREEN ( "SUCCESS" ) )
	#define FAIL_TAG					TAG_BRACKETS( COLOR_YELLOW( "FAIL"    ) )
	#define ERROR_TAG					TAG_BRACKETS( COLOR_RED   ( "ERROR"   ) )


	// Verifiche sull'attivazione della modalità debug.

	/**
	 * Macro function.
	 * Si espande solamente se è attiva la modalità DEBUG.
	 * Può essere usato per espandere qualunque cosa, non solo statements; è compito del programmatore
	 * verificare che un'eventuale espansione vuota non generi problemi.
	 */
	#ifdef DEBUG_MODE
		#define IF_DEBUG_ACTIVE( ... ) __VA_ARGS__ 		// EXPAND
	#else
		#define IF_DEBUG_ACTIVE( ... ) 					// DO NOT EXPAND
	#endif

	/*
	 * Verifico se il progetto viene compilato da GCC, poiché la soluzione
	 * adottata sotto (relativa al problema della trailing comma) non è portabile.
	 */
	#ifdef __GNUC__

	/**
	 * Macro function.
	 * Stampa un messaggio di log, elencando il nome del file e il numero di riga.
	 * E' anche possibile passare argomenti in numero variabile da visualizzare, aggiustando
	 * opportunamente il formato del messaggio come in C.
	 *
	 * Note:
	 * __FILE__ si espande con il nome del file.
	 * __LINE__ si espande con il numero di linea (unsigned int).
	 * __VA_ARGS__ si espande alla lista di parametri variadic, se presenti.
	 *
	 */
	#define DEBUG_LOG( message, ... ) 																				\
		IF_DEBUG_ACTIVE(																							\
			printf( DEBUG_TAG " %s(%u) : " message "\n", __FILE__, __LINE__, ##__VA_ARGS__ )						\
		)

	/**
	 * Macro function.
	 * Stampa un messaggio di log con un tag iniziale che indica il successo di un'operazione.
	 * Il log è accompagnato dal nome del file e dal numero di riga.
	 * Per ulteriori informazioni, vedere la definizione di "DEBUG_LOG".
	 */
	#define DEBUG_LOG_SUCCESS( message, ...) 														\
		DEBUG_LOG( SUCCESS_TAG " " message, ##__VA_ARGS__ )

	/**
	 * Macro function.
	 * Stampa un messaggio di log con un tag iniziale che indica il fallimento di un'operazione.
	 * Il log è accompagnato dal nome del file e dal numero di riga.
	 * Per ulteriori informazioni, vedere la definizione di "DEBUG_LOG".
	 */
	#define DEBUG_LOG_FAIL( message, ...) 															\
		DEBUG_LOG( FAIL_TAG " " message, ##__VA_ARGS__ )

	/**
	 * Macro function.
	 * Stampa un messaggio di log con un tag iniziale che indica un errore interno al processo
	 * di compilazione.
	 * Il log è accompagnato dal nome del file e dal numero di riga.
	 * Per ulteriori informazioni, vedere la definizione di "DEBUG_LOG".
	 */
	#define DEBUG_LOG_ERROR( message, ...) 															\
		DEBUG_LOG( ERROR_TAG " " message, ##__VA_ARGS__ )

	/**
	 * Macro function.
	 * Permette di marcare una fase esplicitandone l'inizio e la fine con due messaggi di log.
	 * Se l'intera fase viene completata, entrambi i messaggi segnalano un successo.
	 *
	 * Esempio di utilizzo:
	 *
	 *		DEBUG_MARK_PHASE( nome_della_fase_da_marcare , con , eventuali , parametri ) {
	 *			...
	 * 			statements
	 *			...
	 *		}
	 *
	 * Nota: per il buon funzionamento di questa macro, è auspicabile l'utilizzo di parentesi
	 * graffe per racchiudere tutti gli statements della fase da marcare.
	 * E' possibile non utilizzare le graffe, ma in tal caso la macro marcherà unicamente
	 * lo statement successivo, segnalandone l'inizio e la fine come previsto.
	 *
	 * Nota: è possibile utilizzare più volte questa macro, eventualmente indentando i blocchi
	 * uno dentro l'altro (in questo caso, solo con le parentesi graffe).
	 *
	 * Nota: alcune chiamate a questa macro SULLA STESSA RIGA del file sorgente potrebbero
	 * non funzionare, poiché la macro function utilizza "__LINE__" e necessita che questo
	 * valore sia diverso ad ogni chiamata.
	 */
	#define DEBUG_MARK_PHASE( phase_name, ... )														\
		IF_DEBUG_ACTIVE(																			\
			_DEBUG_MARK_PHASE( UNIQUE_ID, phase_name, ##__VA_ARGS__ )								\
		)

	#define _DEBUG_MARK_PHASE( counter_id, phase_name, ... )										\
		int UNIQUE_ID = 0;																			\
		for ( 																						\
				DEBUG_ENTERING_PHASE( phase_name, ##__VA_ARGS__ );									\
				UNIQUE_ID < 1; 																		\
				DEBUG_EXITING_PHASE( phase_name, ##__VA_ARGS__ ), UNIQUE_ID++						\
			)
		/* Commento:
		 * L'idea è utilizzare il costrutto "for" per eseguire un pezzo di codice PRIMA
		 * degli statements, e un pezzo di codice DOPO l'esecuzione degli statements.
		 * Il ciclo deve risultare "trasparente" all'esterno, quindi viene utilizzata una
		 * variabile generata casualmente per eseguire una ed una sola iterazione.
		 */

	#define DEBUG_ENTERING_PHASE( phase_name, ... )													\
		DEBUG_LOG_SUCCESS( "Entering phase [%s] \"" COLOR_PURPLE( phase_name ) "\"", debugAcquireTicket().c_str(), ##__VA_ARGS__ )

	#define DEBUG_EXITING_PHASE( phase_name, ... )													\
		DEBUG_LOG_SUCCESS( "Exiting phase  [%s] \"" COLOR_PURPLE( phase_name ) "\"", debugReleaseTicket().c_str(), ##__VA_ARGS__ )

	#endif

	/*
	 * Caso complementare: sto usando un qualunque pre-processore che non faccia parte di GCC.
	 */
	#ifndef __GNUC__

	/**
	 * Macro function.
	 * Stampa un messaggio di log, elencando il nome del file e il numero di riga.
	 * E' anche possibile passare argomenti in numero variabile da visualizzare, aggiustando
	 * opportunamente il formato del messaggio come in C.
	 *
	 * Note:
	 * __FILE__ si espande con il nome del file.
	 * __LINE__ si espande con il numero di linea (unsigned int).
	 * __VA_ARGS__ si espande alla lista di parametri variadic, se presenti.
	 * __VA_OPT__(x) è una macro function che si espande all'argomento x solo se __VA_ARGS__ ha almeno un argomento.
	 *
	 */ // TODO AL MOMENTO USA __VA_OPT__(), MA NON E' TESTATO
	#define DEBUG_LOG( message, ...) 																				\
		IF_DEBUG_ACTIVE(																							\
			printf( DEBUG_TAG " %s(%u) : " message "\n", __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__ )				\
		)

	/**
	 * Macro function.
	 * Stampa un messaggio di log con un tag iniziale che indica il successo di un'operazione.
	 * Il log è accompagnato dal nome del file e dal numero di riga.
	 * Per ulteriori informazioni, vedere la definizione di "DEBUG_LOG".
	 *
	 */ // TODO AL MOMENTO USA __VA_OPT__(), MA NON E' TESTATO
	#define DEBUG_LOG_SUCCESS( message, ...) 														\
		DEBUG_LOG( SUCCESS_TAG " " message __VA_OPT__(,) __VA_ARGS__ )

	/**
	 * Macro function.
	 * Stampa un messaggio di log con un tag iniziale che indica il fallimento di un'operazione.
	 * Il log è accompagnato dal nome del file e dal numero di riga.
	 * Per ulteriori informazioni, vedere la definizione di "DEBUG_LOG".
	 *
	 */ // TODO AL MOMENTO USA __VA_OPT__(), MA NON E' TESTATO
	#define DEBUG_LOG_FAIL( message, ...) 															\
		DEBUG_LOG( FAIL_TAG " " message __VA_OPT__(,) __VA_ARGS__ )

	/**
	 * Macro function.
	 * Stampa un messaggio di log con un tag iniziale che indica un errore interno al processo
	 * di compilazione.
	 * Il log è accompagnato dal nome del file e dal numero di riga.
	 * Per ulteriori informazioni, vedere la definizione di "DEBUG_LOG".
	 *
	 */ // TODO AL MOMENTO USA __VA_OPT__(), MA NON E' TESTATO
	#define DEBUG_LOG_ERROR( message, ...) 															\
		DEBUG_LOG( ERROR_TAG " " message __VA_OPT__(,) __VA_ARGS__ )

	/**
	 * Macro function.
	 * Permette di marcare una fase esplicitandone l'inizio e la fine con due messaggi di log.
	 * Se l'intera fase viene completata, entrambi i messaggi segnalano un successo.
	 *
	 * Esempio di utilizzo:
	 *
	 *		DEBUG_MARK_PHASE( nome_della_fase_da_marcare , con , eventuali , parametri ) {
	 *			...
	 * 			statements
	 *			...
	 *		}
	 *
	 * Nota: per il buon funzionamento di questa macro, è auspicabile l'utilizzo di parentesi
	 * graffe per racchiudere tutti gli statements della fase da marcare.
	 * E' possibile non utilizzare le graffe, ma in tal caso la macro marcherà unicamente
	 * lo statement successivo, segnalandone l'inizio e la fine come previsto.
	 *
	 * Nota: è possibile utilizzare più volte questa macro, eventualmente indentando i blocchi
	 * uno dentro l'altro (in questo caso, solo con le parentesi graffe).
	 *
	 * Nota: alcune chiamate a questa macro SULLA STESSA RIGA del file sorgente potrebbero
	 * non funzionare, poiché la macro function utilizza "__LINE__" e necessita che questo
	 * valore sia diverso ad ogni chiamata.
	 */ // TODO AL MOMENTO USA __VA_OPT__(), MA NON E' TESTATO
	 */
	#define DEBUG_MARK_PHASE( phase_name, ... )														\
		IF_DEBUG_ACTIVE(																			\
			_DEBUG_MARK_PHASE( UNIQUE_ID, phase_name __VA_OPT__(,) __VA_ARGS__ )					\
		)

	#define _DEBUG_MARK_PHASE( counter_id, phase_name, ... )										\
		int UNIQUE_ID = 0;																			\
		for ( 																						\
				DEBUG_ENTERING_PHASE( phase_name __VA_OPT__(,) __VA_ARGS__ );						\
				UNIQUE_ID < 1; 																		\
				DEBUG_EXITING_PHASE( phase_name __VA_OPT__(,) __VA_ARGS__ ), UNIQUE_ID++			\
			)
		/* Commento:
		 * L'idea è utilizzare il costrutto "for" per eseguire un pezzo di codice PRIMA
		 * degli statements, e un pezzo di codice DOPO l'esecuzione degli statements.
		 * Il ciclo deve risultare "trasparente" all'esterno, quindi viene utilizzata una
		 * variabile generata casualmente per eseguire una ed una sola iterazione.
		 */

	#define DEBUG_ENTERING_PHASE( phase_name, ... )													\
		DEBUG_LOG_SUCCESS( "Entering phase [%s] \"" COLOR_PURPLE( phase_name ) "\"", debugAcquireTicket().c_str() __VA_OPT__(,) __VA_ARGS__ )

	#define DEBUG_EXITING_PHASE( phase_name, ... )													\
		DEBUG_LOG_SUCCESS( "Exiting phase  [%s] \"" COLOR_PURPLE( phase_name ) "\"", debugReleaseTicket().c_str() __VA_OPT__(,) __VA_ARGS__ )

	#endif

	/**
	 * Macro function.
	 * Si aspetta in ingresso l'ID di una variabile. Verifica che la variabile sia NULL,
	 * e stampa un messaggio coerente con quanto atteso.
	 */
	#define DEBUG_ASSERT_NULL( variable_id ) 														\
		if (variable_id == NULL) {																	\
			DEBUG_LOG_SUCCESS( "Variable \"%s\" == NULL, expected NULL", #variable_id );			\
		} else {																					\
			DEBUG_LOG_FAIL( "Variable \"%s\" == NOT NULL, expected NULL", #variable_id );			\
		}

	/**
	 * Macro function.
	 * Si aspetta in ingresso l'ID di una variabile. Verifica che la variabile sia NON NULL,
	 * e stampa un messaggio coerente con quanto atteso.
	 */
	#define DEBUG_ASSERT_NOT_NULL( variable_id )													\
		if (variable_id != NULL) {																	\
			DEBUG_LOG_SUCCESS( "Variable \"%s\" == NOT NULL, expected NOT NULL", #variable_id );	\
		} else {																					\
			DEBUG_LOG_FAIL( "Variable \"%s\" == NULL, expected NOT NULL", #variable_id );			\
		}

	/**
	 * Macro function.
	 * Si aspetta in ingresso una condizione. Verifica che tale condizione sia TRUE,
	 * e stampa un messaggio coerente con quanto atteso.
	 */
	#define DEBUG_ASSERT_TRUE( condition ) 															\
		if (condition) {																			\
			DEBUG_LOG_SUCCESS( "Condition (%s) == TRUE, expected TRUE", #condition );				\
		} else {																					\
			DEBUG_LOG_FAIL( "Condition (%s) == FALSE, expected TRUE", #condition );					\
		}

	/**
	 * Macro function.
	 * Si aspetta in ingresso una condizione. Verifica che tale condizione sia TRUE,
	 * e stampa un messaggio coerente con quanto atteso.
	 */
	#define DEBUG_ASSERT_FALSE( condition )															\
		if (!(condition)) {																			\
			DEBUG_LOG_SUCCESS( "Condition (%s) == FALSE, expected FALSE", #condition );				\
		} else {																					\
			DEBUG_LOG_FAIL( "Condition (%s) == TRUE, expected FALSE", #condition );					\
		}

	 // Firme di funzioni

	 using std::string;

	 string debugAcquireTicket();
	 string debugReleaseTicket();

}

#endif /* INCLUDE_DEBUG_HPP_ */

