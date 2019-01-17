#include "linalg.h"

#include "Car.h"
#include "RL_structs.h"

class DoNothing {
public:
	Car * car_;
	Input controls_;
	bool finished_;

	explicit DoNothing(Car * car) {
		car_ = car;
		controls = Input();
	}
	explicit step(float dt = 1.0f / 60.0f) {}
};
