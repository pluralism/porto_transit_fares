#pragma once
#include <iostream>
#include <QHash>
#include <QPair>
#include <QVector>
#include "utils.h"

class __declspec(dllexport) steiner
{
public:
	auto set_graph(QVector<QVector<int>>& graph) -> void;

	auto calculate(const QVector<int>& terminals) -> QVector<QPair<int, int>>;
	auto reconstruct_fw_path(int u, int v) -> QVector<QPair<int, int>>;
private:
	bool dist_graph_calculated_ = false;
	QVector<QVector<int>> dist_;
	QVector<QVector<int>> next_;
};