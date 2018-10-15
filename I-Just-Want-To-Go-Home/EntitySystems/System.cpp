#include "System.h"
#include "../Core/Game.h"

System::System()
{
	Game::instance().componentCreated.connect(boost::bind(&System::onComponentCreated, this, _1, _2));
	Game::instance().componentDestroyed.connect(boost::bind(&System::onComponentDestroyed, this, _1, _2));
}

System::~System()
{
	// b/c binding is used signal is automatically disconnected when this obj is destroyed.
}
