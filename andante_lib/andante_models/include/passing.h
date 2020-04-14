#pragma once
#include <QString>
#include <QDateTime>

class __declspec(dllexport) passing
{
public:
	passing() = default;
	explicit passing(QString stop_code, QString zone_code, QDateTime passing_time, QString trip_id, const bool is_validation_passing) :
		stop_code_{ std::move(stop_code) }, zone_code_{ std::move(zone_code) }, passing_time_{ std::move(passing_time) },
		trip_id_{ std::move(trip_id) }, is_validation_passing_{ is_validation_passing } { }
	auto get_stop_code() const -> QString { return stop_code_; }
	auto get_zone_code() const -> QString { return zone_code_; }
	auto get_passing_time() const -> QDateTime { return passing_time_; }
	auto get_trip_id() const -> QString { return trip_id_; }
	auto is_validation_passing() const -> bool { return is_validation_passing_; }

	auto set_is_validation_passing(const bool validation) -> void { is_validation_passing_ = validation; }
private:
	QString stop_code_;
	QString zone_code_;
	QDateTime passing_time_;
	QString trip_id_;
	bool is_validation_passing_ = false;
};

class passing_builder
{
public:
	passing_builder& set_stop_code(const QString& stop_code)
	{
		stop_code_ = stop_code;
		return *this;
	}

	passing_builder& set_zone_code(const QString& zone_code)
	{
		zone_code_ = zone_code;
		return *this;
	}

	passing_builder& set_passing_time(const QDateTime& passing_time)
	{
		passing_time_ = passing_time;
		return *this;
	}

	passing_builder& set_trip_id(const QString& trip_id)
	{
		trip_id_ = trip_id;
		return *this;
	}

	passing_builder& set_is_validation_passing(const bool is_validation_passing)
	{
		is_validation_passing_ = is_validation_passing;
		return *this;
	}

	passing build() const
	{
		return passing{ stop_code_, zone_code_, passing_time_, trip_id_, is_validation_passing_ };
	}
private:
	QString stop_code_;
	QString zone_code_;
	QDateTime passing_time_;
	QString trip_id_;
	bool is_validation_passing_ = false;
};