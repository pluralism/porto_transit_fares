#pragma once
#include "fare_ticket.h"

class andante_pass final : public fare_ticket
{
public:
	explicit andante_pass() = default;
	explicit andante_pass(const fare& fare) : fare_ticket{ fare } { }
};
