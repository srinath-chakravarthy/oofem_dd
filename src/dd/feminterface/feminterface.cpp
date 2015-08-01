#include "feminterface.h"
#include "../domain.h"

namespace dd {

	FemInterface::FemInterface(Domain * domain)
		: domain(domain) {
		domain->setFemInterface(this);
	}

}
