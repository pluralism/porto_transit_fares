#pragma once
// ReSharper disable CppUnusedIncludeDirective
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QVector>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "../../andante_lib/andante_models/include/trip.h"
#include "../../andante_lib/andante_models/include/fare.h"
#include "../../andante_lib/andante_models/include/zone.h"

class __declspec(dllexport) converter
{
public:
	static converter& get_instance()
	{
		static converter instance;
		return instance;
	}
	converter(const converter&) = delete;
	auto operator=(const converter&) -> converter& = delete;
	
	auto convert_trips_to_json(const QString& file_name) -> QJsonDocument;
	auto save_trips_to_json(const QJsonDocument& json_data, const QString& out_file_name) -> bool;
	auto load_trips_from_json(const QString& file_name) -> QVector<trip>;

	auto convert_fares_to_json(const QString& file_name) -> QJsonDocument;
	auto save_fares_to_json(const QJsonDocument& json_data, const QString& out_file_name) -> bool;
	auto load_fares_from_json(const QString& file_name) -> QVector<fare>;

	auto convert_zones_to_json(const QString& file_name) -> QJsonDocument;
	auto save_zones_to_json(const QJsonDocument& json_data, const QString& out_file_name) -> bool;
	auto load_zones_from_json(const QString& file_name) -> QVector<zone>;
private:
	converter() = default;
	
	auto parse_trips_from_file(const QString& file_name) -> QVector<trip>;
	auto parse_fares_from_file(const QString& file_name) -> QVector<fare>;
	auto parse_zones_from_file(const QString& file_name) -> QVector<zone>;
	auto write_json_to_file(const QJsonDocument& document, const QString& out_file_name) -> bool;

	auto is_passing_data_valid(const QJsonObject& passing_object) -> bool;
	auto is_fare_data_valid(const QJsonObject& fare_object) -> bool;
	auto is_trip_data_valid(const QJsonObject& trip_object) -> bool;
	static constexpr char trip_delimiter[] = "TRIP";
	static constexpr char passings_delimiter[] = "Passings";
};
