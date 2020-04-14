#pragma once
#include <QString>
#include <QVector>
#include <utility>
#include "passing.h"

class __declspec(dllexport) trip
{
public:
	trip() = default;
	trip(QString trip_id, QString operator_code, QString line_code) : trip_id_{std::move(trip_id)},
		operator_code_{std::move(operator_code)}, line_code_{std::move(line_code)} { }
	trip(const trip&) = default;
	auto operator=(const trip&)->trip& = default;
	trip(trip&&) noexcept = default;
	auto operator=(trip&&) noexcept->trip& = default;
	~trip() = default;

	auto get_trip_id() const -> QString { return trip_id_; }
	auto get_operator_code() const -> QString { return operator_code_; }
	auto get_line_code() const -> QString { return line_code_; }
	auto get_passings() const -> QVector<passing> { return passings_; }
	auto add_passing(const passing& passing) -> void { passings_.append(passing); }
	auto add_passing(passing&& passing) -> void { passings_.append(std::move(passing)); }
	auto get_validation_time() const -> QDateTime { return passings_[0].get_passing_time(); }
private:
	QString trip_id_;
	QString operator_code_;
	QString line_code_;

	QVector<passing> passings_;
};
