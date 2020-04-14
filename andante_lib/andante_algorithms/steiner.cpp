#include "include/steiner.h"

auto steiner::calculate(const QVector<int>& terminals) -> QVector<QPair<int, int>>
{
	if (terminals.size() == 2)
	{
		return reconstruct_fw_path(terminals[0], terminals[1]);
	}

	constexpr auto max = INT_MAX;
	const auto K = terminals.size();
	const auto C = (1 << (K - 1)) - 1;
	const auto n = dist_.size();
	QVector<QVector<int>> S(1 << (K - 1), QVector<int>(n, max));
	QHash<QPair<int, int>, int> smd_k;

	for (auto i = 0; i < K - 1; i++)
	{
		for (auto j = 0; j < n; j++)
		{
			// S(m, D), where "m" is "j" and "D" is "1 << i"
			S[1 << i][j] = dist_[terminals[K - i - 1]][j];
			smd_k[qMakePair(1 << i, j)] = j;
		}
	}

	for (auto m = 2; m < K - 1; m++)
	{
		auto z = 0;
		for (auto D = (1 << m) - 1; D < (1 << (K - 1)); z = D | (D - 1), D = (z + 1) | ((~z & -~z) - 1) >> (__clzsi2(D) + 1))
		{
			for (auto i = 0; i < n; i++)
			{
				S[D][i] = max;
			}

			for (auto j = 0; j < n; j++)
			{
				auto u = max;
				// Break D set in two sets: E and F
				for (auto E = D & (D - 1); E > 0; E = D & (E - 1))
				{
					// F is D - E
					auto F = D & ~E;
					// S[E][j] + S[F][j] represents S_k(D)
					if (u > S[E][j] + S[F][j])
					{
						u = S[E][j] + S[F][j];
					}
				}

				for (auto i = 0; i < n; i++)
				{
					/**
					 * S[D][i] is S(m, D), so i is "m" and "j" is "k"
					 * S(m, D) is the Steiner path for "{m u D}"
					 */
					if (S[D][i] > u + dist_[i][j])
					{
						S[D][i] = u + dist_[i][j];
						smd_k[qMakePair(D, i)] = j;
					}
				}
			}
		}
	}

	auto best_k = max, best_e = max, best_f = max;
	// We're now ready for step ||Y|| - 1, so "D = C"
	auto v = max;
	for (auto j = 0; j < n; j++)
	{
		auto u = max, round_e = max, round_f = max;
		// auto E = C & (C - 1); E > 0; E = C & (E - 1)
		for (auto D = K - 2, E = 1 << D; D >= 0; E = 1 << D--)
		{
			// F = C - E
			auto F = C & ~E;
			// S[E][j] + S[F][j] is S_k(D), where D is broken in E and F sets
			if (u > S[E][j] + S[F][j])
			{
				u = S[E][j] + S[F][j];
				round_e = E;
				round_f = F;
			}
		}

		if (v > u + dist_[terminals[0]][j])
		{
			// This is equivalent to S(1, D)
			v = u + dist_[terminals[0]][j];
			best_k = j;
			best_e = round_e;
			best_f = round_f;
		}
	}

	auto result = QVector<QPair<int, int>>{};
	result.append(reconstruct_fw_path(terminals[0], best_k));

	auto e_value = max;
	auto current_index = K - 1;
	while (best_e)
	{
		if (best_e & 1)
		{
			e_value = terminals[current_index];
		}
		current_index--;
		best_e >>= 1;
	}

	result.append(reconstruct_fw_path(e_value, best_k));

	// Find out the "k" value that generated S(best_k, best_f)
	const auto best_f_k = smd_k[qMakePair(best_f, best_k)];
	result.append(reconstruct_fw_path(best_k, best_f_k));

	auto values = QVector<int>{};
	auto new_index = K - 1;
	while (best_f)
	{
		if (best_f & 1)
		{
			values.append(terminals[new_index]);
		}
		new_index--;
		best_f >>= 1;
	}

	for (const auto& val : values)
	{
		result.append(reconstruct_fw_path(best_f_k, val));
	}

	return result;
}

auto steiner::reconstruct_fw_path(int u, const int v) -> QVector<QPair<int, int>>
{
	QVector<QPair<int, int>> result = { qMakePair(u, u) };
	auto last_u{ u };

	while (u != v)
	{
		last_u = u;
		u = next_[u][v];
		if (u != last_u)
		{
			result.append(qMakePair(u, last_u));
		}
	}

	return result;
}

auto steiner::set_graph(QVector<QVector<int>>& graph) -> void
{
	const auto graph_size = graph.size();
	dist_ = QVector<QVector<int>>(graph_size, QVector<int>(graph_size));
	next_ = QVector<QVector<int>>(graph_size, QVector<int>(graph_size));

	for (auto i = 0; i < graph_size; i++)
	{
		for (auto j = 0; j < graph_size; j++)
		{
			if (i == j)
			{
				dist_[i][j] = 0;
			}
			else
			{
				dist_[i][j] = graph[i][j];
			}
			next_[i][j] = j;
		}
	}

	// Floyd-Warshall algorithm
	for (auto k = 0; k < graph_size; k++)
	{
		for (auto i = 0; i < graph_size; i++)
		{
			for (auto j = 0; j < graph_size; j++)
			{
				if (dist_[i][k] != INT_MAX && dist_[k][j] != INT_MAX)
				{
					if (dist_[i][j] > dist_[i][k] + dist_[k][j])
					{
						dist_[i][j] = dist_[i][k] + dist_[k][j];
						next_[i][j] = next_[i][k];
					}
				}
			}
		}
	}

	dist_graph_calculated_ = true;
}
