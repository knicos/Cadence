#include <cadence/agent.h>

using namespace cadence;
using namespace cadence::doste;

class Assigner : public Agent {
	public:
	Assigner(const OID &o): Agent(o) { registerEvents(); }
	~Assigner() {};

	OBJECT(Agent, Assigner);

	BEGIN_EVENTS(Agent);
	EVENT(evt_condition, (*this)("condition"));
	END_EVENTS;
};

OnEvent(Assigner, evt_condition) {
	if (get("condition") == True) {
		get("object").set(get("property"), get("value"));
	}
}

IMPLEMENT_EVENTS(Assigner, Agent);

class IsDefiner : public Agent {
	public:
	IsDefiner(const OID &o): Agent(o) { registerEvents(); }
	~IsDefiner() {};

	OBJECT(Agent, IsDefiner);

	BEGIN_EVENTS(Agent);
	EVENT(evt_condition, (*this)("condition"));
	END_EVENTS;
};

OnEvent(IsDefiner, evt_condition) {
	if (get("condition") == True) {
		get("object").define(get("property"), get("definition"));
	}
}

IMPLEMENT_EVENTS(IsDefiner, Agent);

class BecomesDefiner : public Agent {
	public:
	BecomesDefiner(const OID &o): Agent(o) { registerEvents(); }
	~BecomesDefiner() {};

	OBJECT(Agent, BecomesDefiner);

	BEGIN_EVENTS(Agent);
	EVENT(evt_condition, (*this)("condition"));
	END_EVENTS;
};

OnEvent(BecomesDefiner, evt_condition) {
	if (get("condition") == True) {
		get("object").definefuture(get("property"), get("definition"));
	}
}

IMPLEMENT_EVENTS(BecomesDefiner, Agent);

extern "C" void initialise(const cadence::doste::OID &base) {
	Object::registerType<Assigner>();
	Object::registerType<BecomesDefiner>();
	Object::registerType<IsDefiner>();
}

extern "C" void cadence_update() {
	
}

extern "C" void finalise() {

}
