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
    : _interface(interface), _waitingClientsCount(0), _isInService(true),
      _isBarberAsleep(false), _isClientOnWorkChair(false), _waitingSeats(capacity, true)
{
}

/********************************************
 * Méthodes de l'interface pour les clients *
 *******************************************/
bool PcoSalon::accessSalon(unsigned clientId)
{
    monitorIn();

    if (!_isInService) {
        monitorOut();
        return false;
    }

    // find available seat
    size_t seatIndex = 0;
    for (; seatIndex < _waitingSeats.size(); ++seatIndex) {
        if (_waitingSeats[seatIndex]) {
            break;
        }
    }

    if (seatIndex == _waitingSeats.size()) {
        monitorOut();
        return false;
    }

    _waitingSeats[seatIndex] = false;
    animationClientSitOnChair(clientId, seatIndex);

    if (!_isInService) {
        _waitingSeats[seatIndex] = true;
        monitorOut();
        return false;
    }

    // wake barber if first client, otherwise wait for turn
    _waitingClientsCount++;
    if (_isBarberAsleep && _waitingClientsCount == 1) {
        signal(_barberWaitingForClient);
    } else {
        wait(_clientWaitingForTurn);
    }
    _waitingSeats[seatIndex] = true;

    monitorOut();
    return true;
}


void PcoSalon::goForHairCut(unsigned clientId)
{
    monitorIn();

    animationClientSitOnWorkChair(clientId);

    _isClientOnWorkChair = true;

    signal(_barberWaitingForClientAtChair);
    wait(_clientWaitingForHaircut);
    
    _isClientOnWorkChair = false;
    _waitingClientsCount--;

    monitorOut();
}

void PcoSalon::waitingForHairToGrow(unsigned clientId)
{
    monitorIn();
    animationClientWaitForHairToGrow(clientId);
    monitorOut();
}


void PcoSalon::walkAround(unsigned clientId)
{
    monitorIn();
    animationClientWalkAround(clientId);
    monitorOut();
}


void PcoSalon::goHome(unsigned clientId)
{
    monitorIn();
    animationClientGoHome(clientId);
    monitorOut();
}


/********************************************
 * Méthodes de l'interface pour le barbier  *
 *******************************************/
unsigned int PcoSalon::getNbClient()
{
    return _waitingClientsCount;
}

void PcoSalon::goToSleep()
{
    monitorIn();

    _isBarberAsleep = true;
    animationBarberGoToSleep();
    wait(_barberWaitingForClient);
    animationWakeUpBarber();
    _isBarberAsleep = false;
    
    monitorOut();
}

void PcoSalon::pickNextClient()
{
    monitorIn();
    signal(_clientWaitingForTurn);
    monitorOut();
}

void PcoSalon::waitClientAtChair()
{
    monitorIn();
    if (!_isClientOnWorkChair) {
        wait(_barberWaitingForClientAtChair);
    }
    monitorOut();
}

void PcoSalon::beautifyClient()
{
    monitorIn();
    animationBarberCuttingHair();
    signal(_clientWaitingForHaircut);
    monitorOut();
}

/********************************************
 *    Méthodes générales de l'interface     *
 *******************************************/
bool PcoSalon::isInService()
{
    return _isInService;
}

void PcoSalon::endService()
{
    monitorIn();

    _isInService = false;
    // wake up all waiting threads for clean termination
    for (size_t i = 0; i < _waitingClientsCount; ++i) {
        signal(_clientWaitingForTurn);
    }
    signal(_barberWaitingForClientAtChair);
    signal(_clientWaitingForHaircut);
    
    monitorOut();
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
