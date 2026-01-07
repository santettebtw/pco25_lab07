<<<<<<< HEAD
/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \| ____|
 * | |__) | |   | |  | |    ) | | | | ) || |___
 * |  ___/| |   | |  | |   / /| | | |/ / |____|
 * | |    | |___| |__| |  / /_| |_| / /_  ____|
 * |_|     \_____\____/  |____|\___/____||_____|
 */
// Modifications à faire dans le fichier

#include "client.h"
#include <unistd.h>

#include <iostream>

int Client::_nextId = 0;

Client::Client(GraphicSalonInterface *interface, std::shared_ptr<SalonClientInterface> salon)

    : _interface(interface), _salon(salon),  _clientId(_nextId++)
{
    _interface->consoleAppendTextClient(_clientId, "Salut, prêt pour une coupe !");
}

void Client::run()
{
    // TODO
    _interface->consoleAppendTextClient(_clientId, "Le salon est fermé... Zut !");
}
=======
/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \| ____|
 * | |__) | |   | |  | |    ) | | | | ) || |___
 * |  ___/| |   | |  | |   / /| | | |/ / |____|
 * | |    | |___| |__| |  / /_| |_| / /_  ____|
 * |_|     \_____\____/  |____|\___/____||_____|
 */
// Modifications à faire dans le fichier

#include "client.h"
#include <unistd.h>

#include <iostream>

int Client::_nextId = 0;

Client::Client(GraphicSalonInterface *interface, std::shared_ptr<SalonClientInterface> salon)

    : _interface(interface), _salon(salon),  _clientId(_nextId++)
{
    _interface->consoleAppendTextClient(_clientId, "Salut, prêt pour une coupe !");
}

void Client::run()
{
	while (_salon.isInServicce()) {
		while (!_salon.accessSalon())
		{
			_salon.walkAround(_clientId);
		}
	
		// TODO(sss): check if barber is sleeping. Wake him up if so.
		_salon.goForHairCut(_clientId);

		_salon.waitingForHairToGrow();
	}

	_salon.goHome(_clientId);
}
>>>>>>> 7635dc21621b9ddda6660ac7f815e532c7bb19d8
