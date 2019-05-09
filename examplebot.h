#pragma once

#include "bot.h"

#include "simulation/input.h"

class ExampleBot : public Bot
{
public:
	ExampleBot(int _index, int _team, std::string _name);
	Input GetOutput(const rlbot::flat::GameTickPacket* gameTickPacket, const rlbot::flat::FieldInfo* fieldInfo, const rlbot::flat::BallPrediction* ballPrediction);
};


