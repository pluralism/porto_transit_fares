#pragma once
#include <QVector>
#include <utility>
#include "fare.h"
#include "passing.h"

class __declspec(dllexport) fare_ticket
{
public:
	explicit fare_ticket() = default;
	explicit fare_ticket(const fare& fare) : fare_{fare} { }

	auto add_passing(const passing& passing) -> void { passings_.append(passing); }
	auto add_passings(const QVector<passing>& passings) -> void { passings_.append(passings); }
	auto set_fare(const fare& fare) -> void { fare_ = fare; }

	auto get_fare() const -> fare { return fare_; }
	auto get_passings() const -> QVector<passing> { return passings_; }
protected:
	fare fare_;
	QVector<passing> passings_;
};