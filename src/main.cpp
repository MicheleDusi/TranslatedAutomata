/******************************************************************************
 * main.cpp
 *
 * Author: Michele Dusi
 * Project: TranslatedAutomata
 *
 * Main del progetto. Gestisce il flusso di esecuzione in maniera
 * centralizzata, richiamando le funzioni necessarie.
 *
 ******************************************************************************/

#include "main.hpp"

#include <iostream>
#include <string>
#include <set>

#include "debug.hpp"

using std::set;

using namespace translated_automata;

int main(int argc, char **argv) {
	DEBUG_MARK_PHASE( main ) {

	std::cout << "Hello world!" << std::endl;

	}
	return 0;
}
