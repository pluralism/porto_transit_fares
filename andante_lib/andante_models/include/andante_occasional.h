#pragma once
#include <utility>
#include "fare_ticket.h"

class __declspec(dllexport) andante_occasional final : public fare_ticket
{
public:
	explicit andante_occasional() = default;
	explicit andante_occasional(const fare& fare, passing first_passing) : fare_ticket{ fare }, first_passing_{ std::move(first_passing) } { }

	auto get_validation_time() const -> QDateTime { return first_passing_.get_passing_time(); }
	auto get_validation_zone() const -> QString { return first_passing_.get_zone_code(); }
	auto get_first_passing() const -> passing { return first_passing_; }
private:
	passing first_passing_;
};
