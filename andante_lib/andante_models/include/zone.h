#pragma once
#include <QString>
#include <QSet>
#include <utility>

class __declspec(dllexport) zone
{
public:
	zone() = default;
	explicit zone(QString name) : name_{std::move(name)} { }
	auto get_name() const -> QString { return name_; }
	auto get_adjacent_zones() const -> QSet<zone> { return adjacent_zones_; }
	auto add_adjacent_zone(const zone& zone) -> void { adjacent_zones_.insert(zone); }
	auto operator==(const zone& other) const -> bool
	{
		return name_ == other.name_;
	}
private:
	QString name_{};
	QSet<zone> adjacent_zones_{};
};


inline uint qHash(const zone &key)
{
	return qt_hash(key.get_name());
}