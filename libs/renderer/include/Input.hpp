#pragma once

#include <tuple>

struct Input
{
	Input()
	{
		id = -1;
		source = InputSource::UNKNOWN;
		state = InputState::IDLE;
		mods = 0;
	}
	Input(int id, int source, int state, int mods)
	  : id(id)
	  , source(source)
	  , state(state)
	  , mods(mods)
	{}

	int id;
	int source;
	int state;
	int mods;

	bool operator==(const Input& rhs) const
	{
		return (this->id == rhs.id) && (this->source == rhs.source) &&
		       (this->state == rhs.state) && (this->mods == rhs.mods);
	}

	bool operator!=(const Input& rhs) const
	{
		/*if (this->state != -1) {
			return (this->id != rhs.id) || (this->source != rhs.source) ||
			       (this->state != rhs.state) && (this->mods != rhs.mods);
		} else {*/
			return (this->id != rhs.id) || (this->source != rhs.source) ||
			       (this->state != rhs.state) || (this->mods != rhs.mods);
		//}
	}

	bool operator<(const Input& rhs) const
	{
		/*if (this->state != -1) {*/
			return std::tie(this->id, this->source, this->state, this->mods) <
			       std::tie(rhs.id, rhs.source, rhs.state, rhs.mods);
		/*} else {
			return std::tie(this->id, this->source) < std::tie(rhs.id, rhs.source);
		}*/
	}
};
