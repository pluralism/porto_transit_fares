#pragma once
#include <QVector>
#include <utility>
#include "fare_ticket.h"

class fare_solution
{
public:
	explicit fare_solution() = default;
	explicit fare_solution(const int price, QVector<fare_ticket> tickets) : price_{price}, tickets_{std::move(tickets)} { }

	auto get_price() const -> int { return price_; }
	auto get_tickets() const -> QVector<fare_ticket> { return tickets_; }
private:
	int price_{};
	QVector<fare_ticket> tickets_{};
};
