/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \| ____|
 * | |__) | |   | |  | |    ) | | | | ) || |___
 * |  ___/| |   | |  | |   / /| | | |/ / |____|
 * | |    | |___| |__| |  / /_| |_| / /_  ____|
 * |_|     \_____\____/  |____|\___/____||_____|
 */
// Modifications à faire dans le fichier

#include "pcosalon.h"

#include <pcosynchro/pcothread.h>

#include <iostream>

PcoSalon::PcoSalon(GraphicSalonInterface *interface, unsigned int capacity)
    : _interface(interface), _clients(capacity, -1), _writeIndex(0), _readIndex(0),
	_nbClients(0), _capacity(capacity)
{
    // TODO
}

/********************************************
 * Méthodes de l'interface pour les clients *
 *******************************************/
bool PcoSalon::accessSalon(unsigned clientId)
{
	_mutex.lock();
	while (_nbClients >= _capacity) {
		_waitNotFull.wait(&_mutex);
	}
	_clients[_writeIndex] = clientId;
	_writeIndex = (_writeIndex + 1) % _capacity;
	_nbClients++;
	_mutex.unlock();
}


void PcoSalon::goForHairCut(unsigned clientId)
{
    // TODO
}

void PcoSalon::waitingForHairToGrow(unsigned clientId)
{
	// TODO
}


void PcoSalon::walkAround(unsigned clientId)
{
    // TODO
}


void PcoSalon::goHome(unsigned clientId){
    // TODO
}


/********************************************
 * Méthodes de l'interface pour le barbier  *
 *******************************************/
unsigned int PcoSalon::getNbClient()
{
	return _nbClients;
}

void PcoSalon::goToSleep()
{
	// TODO
}

void PcoSalon::pickNextClient()
{
    // TODO
}


void PcoSalon::waitClientAtChair()
{
    // TODO
}


void PcoSalon::beautifyClient()
{
    // TODO
}

/********************************************
 *    Méthodes générales de l'interface     *
 *******************************************/
bool PcoSalon::isInService()
{
    // TODO
}


void PcoSalon::endService()
{
    // TODO
}

/********************************************
 *   Méthodes privées pour les animations   *
 *******************************************/

template<typename F>
void PcoSalon::animationCall(F&& f)
{
#if PCO_USE_HOARE_MONITOR
    monitorOut();
    f();
    monitorIn();
#else
    _mutex.unlock();
    f();
    _mutex.lock();
#endif
}

void PcoSalon::animationClientAccessEntrance(unsigned clientId)
{
    animationCall([&]{ _interface->clientAccessEntrance(clientId); });
}

void PcoSalon::animationClientSitOnChair(unsigned clientId, unsigned clientSitNb)
{
    animationCall([&]{ _interface->clientSitOnChair(clientId, clientSitNb);} );
}

void PcoSalon::animationClientSitOnWorkChair(unsigned clientId)
{
    animationCall([&]{_interface->clientSitOnWorkChair(clientId);} );
}

void PcoSalon::animationClientWaitForHairToGrow(unsigned clientId)
{
    animationCall([&]{_interface->clientWaitHairToGrow(clientId, true);} );
}

void PcoSalon::animationClientWalkAround(unsigned clientId)
{
    animationCall([&]{_interface->clientWalkAround(clientId);} );
}

void PcoSalon::animationBarberGoToSleep()
{
    animationCall([&]{_interface->barberGoToSleep();} );
}

void PcoSalon::animationWakeUpBarber()
{
    animationCall([&]{_interface->clientWakeUpBarber();} );
}

void PcoSalon::animationBarberCuttingHair()
{
    animationCall([&]{_interface->barberCuttingHair();});
}

void PcoSalon::animationClientGoHome(unsigned clientId){
    animationCall([&]{_interface->clientWaitHairToGrow(clientId, false);});
}
