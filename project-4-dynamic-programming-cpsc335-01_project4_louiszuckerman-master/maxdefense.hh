///////////////////////////////////////////////////////////////////////////////
// maxdefense.hh
//
// Compute the set of armos that maximizes defense, within a gold budget,
// with the dynamic method or exhaustive search.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

// One armor item available for purchase.
class ArmorItem
{
	//
public:
	//
	ArmorItem(
		const std::string &description,
		size_t cost_gold,
		double defense_points)
		: _description(description),
		  _cost_gold(cost_gold),
		  _defense_points(defense_points)
	{
		assert(!description.empty());
		assert(cost_gold > 0);
	}

	//
	const std::string &description() const { return _description; }
	int cost() const { return _cost_gold; }
	double defense() const { return _defense_points; }

	//
private:
	// Human-readable description of the armor, e.g. "new enchanted helmet". Must be non-empty.
	std::string _description;

	// Cost, in units of gold; Must be positive
	int _cost_gold;

	// Defense points; most be non-negative.
	double _defense_points;
};

// Alias for a vector of shared pointers to ArmorItem objects.
typedef std::vector<std::shared_ptr<ArmorItem>> ArmorVector;

// Load all the valid armor items from the CSV database
// Armor items that are missing fields, or have invalid values, are skipped.
// Returns nullptr on I/O error.
std::unique_ptr<ArmorVector> load_armor_database(const std::string &path)
{
	std::unique_ptr<ArmorVector> failure(nullptr);

	std::ifstream f(path);
	if (!f)
	{
		std::cout << "Failed to load armor database; Cannot open file: " << path << std::endl;
		return failure;
	}

	std::unique_ptr<ArmorVector> result(new ArmorVector);

	size_t line_number = 0;
	for (std::string line; std::getline(f, line);)
	{
		line_number++;

		// First line is a header row
		if (line_number == 1)
		{
			continue;
		}

		std::vector<std::string> fields;
		std::stringstream ss(line);

		for (std::string field; std::getline(ss, field, '^');)
		{
			fields.push_back(field);
		}

		if (fields.size() != 3)
		{
			std::cout
				<< "Failed to load armor database: Invalid field count at line " << line_number << "; Want 3 but got " << fields.size() << std::endl
				<< "Line: " << line << std::endl;
			return failure;
		}

		std::string
			descr_field = fields[0],
			cost_gold_field = fields[1],
			defense_points_field = fields[2];

		auto parse_dbl = [](const std::string &field, double &output) {
			std::stringstream ss(field);
			if (!ss)
			{
				return false;
			}

			ss >> output;

			return true;
		};

		std::string description(descr_field);
		double cost_gold, defense_points;
		if (
			parse_dbl(cost_gold_field, cost_gold) && parse_dbl(defense_points_field, defense_points))
		{
			result->push_back(
				std::shared_ptr<ArmorItem>(
					new ArmorItem(
						description,
						cost_gold,
						defense_points)));
		}
	}

	f.close();

	return result;
}

// Convenience function to compute the total cost and defense in an ArmorVector.
// Provide the ArmorVector as the first argument
// The next two arguments will return the cost and defense back to the caller.
void sum_armor_vector(
	const ArmorVector &armors,
	int &total_cost,
	double &total_defense)
{
	total_cost = total_defense = 0;
	for (auto &armor : armors)
	{
		total_cost += armor->cost();
		total_defense += armor->defense();
	}
}

// Convenience function to print out each ArmorItem in an ArmorVector,
// followed by the total kilocalories and protein in it.
void print_armor_vector(const ArmorVector &armors)
{
	std::cout << "*** Armor Vector ***" << std::endl;

	if (armors.size() == 0)
	{
		std::cout << "[empty armor list]" << std::endl;
	}
	else
	{
		for (auto &armor : armors)
		{
			std::cout
				<< "Ye olde " << armor->description()
				<< " ==> "
				<< "Cost of " << armor->cost() << " gold"
				<< "; Defense points = " << armor->defense()
				<< std::endl;
		}

		int total_cost;
		double total_defense;
		sum_armor_vector(armors, total_cost, total_defense);
		std::cout
			<< "> Grand total cost: " << total_cost << " gold" << std::endl
			<< "> Grand total defense: " << total_defense
			<< std::endl;
	}
}

// Convenience function to print out a 2D cache, composed of an std::vector<std::vector<double>>
// For sanity, will refuse to print a cache that is too large.
// Hint: When running this program, you can redirect stdout to a file,
//	which may be easier to view and inspect than a terminal
void print_2d_cache(const std::vector<std::vector<double>> &cache)
{
	std::cout << "*** 2D Cache ***" << std::endl;

	if (cache.size() == 0)
	{
		std::cout << "[empty]" << std::endl;
	}
	else if (cache.size() > 250 || cache[1].size() > 250)
	{
		std::cout << "[too large]" << std::endl;
	}
	else
	{
		for (const std::vector<double> row : cache)
		{
			for (double value : row)
			{
				std::cout << std::setw(5) << value;
			}
			std::cout << std::endl;
		}
	}
}

// Filter the vector source, i.e. create and return a new ArmorVector
// containing the subset of the armor items in source that match given
// criteria.
// This is intended to:
//	1) filter out armor with zero or negative defense that are irrelevant to our optimization
//	2) limit the size of inputs to the exhaustive search algorithm since it will probably be slow.
//
// Each armor item that is included must have at minimum min_defense and at most max_defense.
//	(i.e., each included armor item's defense must be between min_defense and max_defense (inclusive).
//
// In addition, the the vector includes only the first total_size armor items that match these criteria.
std::unique_ptr<ArmorVector> filter_armor_vector(
	const ArmorVector &source,
	double min_defense,
	double max_defense,
	int total_size)
{
	std::unique_ptr<ArmorVector> filtered_vector(new ArmorVector);
	size_t i = 0;
	size_t accepted_counter = 0;
	size_t sourceSize = source.size();
	//std::cout << std::endl << "New Vector - Total Size: " << total_size << " MIn Defense: " << min_defense << " Max Defense: " << max_defense << std::endl;

	while (i < sourceSize - 1 && accepted_counter != total_size)
	{
		if (source.at(i)->defense() >= min_defense && source.at(i)->defense() <= max_defense)
		{
			//std::cout << "Running" << std::endl;
			const std::string description = source.at(i)->description();
			double cost_gold = source.at(i)->cost();
			double defense_points = source.at(i)->defense();

			filtered_vector->push_back(
				std::shared_ptr<ArmorItem>(
					new ArmorItem(
						description,
						cost_gold,
						defense_points)));

			accepted_counter++;
		}
		i++;
	}
	//std::cout << "Total Size: " << filtered_vector->size() << std::endl;
	return filtered_vector;
}

// Compute the optimal set of armor items with a dynamic algorithm.
// Specifically, among the armor items that fit within a total_cost gold budget,
// choose the selection of armors whose defense is greatest.
// Repeat until no more armor items can be chosen, either because we've run out of armor items,
// or run out of gold.
std::unique_ptr<ArmorVector> dynamic_max_defense(
	const ArmorVector &armors,
	int total_cost)
{
	std::unique_ptr<ArmorVector> bestArmor(new ArmorVector);

	// Similar to Knapsack problem, make two arrays, one weighted with gold values, other with corresponding defense values
	// Split armors vector into two separate vectors, X(wt) array weight = gold_cost of item(), V(val) array value = item_defense()
	// W = total_cost, n = size of armors

	const size_t n = armors.size() + 1;
	const size_t W = total_cost + 1;

	std::vector<std::vector<double>> cache;

	cache.resize(n);

	// initialzie gold cost and defense value vectors to 0
	for (size_t i = 0; i < n; cache[i].resize(W), ++i)
		;

	for (size_t i = 0; i < W; cache[0][i] = 0, ++i)
		;

	for (size_t i = 0; i < armors.size(); ++i)
	{
		for (size_t j = 0; j < W; ++j)
		{
			if (j >= armors[i]->cost())
			{
				cache[i + 1][j] = std::max(cache[i][j], cache[i][j - armors[i]->cost()] + armors[i]->defense());
			}
			else
			{
				cache[i + 1][j] = cache[i][j];
			}
		}
	}

	int max_cache = cache[armors.size()][total_cost];

	auto w = total_cost;

	for (int i = armors.size(); i > 0 && max_cache > 0; i--)
	{
		if (cache[i][w] != cache[i - 1][w])
		{
			bestArmor->push_back(armors.at(i - 1));
			w = w - armors.at(i - 1)->cost();
		}
	}

	return bestArmor;
}

// Compute the optimal set of armor items with an exhaustive search algorithm.
// Specifically, among all subsets of armor items,
// return the subset whose gold cost fits within the total_cost budget,
// and whose total defense is greatest.
// To avoid overflow, the size of the armor items vector must be less than 64.
std::unique_ptr<ArmorVector> exhaustive_max_defense(
	const ArmorVector &armors,
	double total_cost)
{
	const int n = armors.size();
	assert(n < 64);

	std::unique_ptr<ArmorVector> bestArmor_vec(new ArmorVector);
	int candidate_cost, best_cost;
	double candidate_defense, best_defense;

	for (uint64_t i = 0; i <= (pow(2, n) - 1); i++)
	{
		std::unique_ptr<ArmorVector> candidate_vec(new ArmorVector);
		for (uint64_t j = 0; j <= n - 1; j++)
		{
			if (((i >> j) & 1) == 1)
			{
				//std::cout << "Bitwise if Loop Running" << std::endl;
				candidate_vec->push_back(armors.at(j));
			}
		}
		sum_armor_vector(*candidate_vec, candidate_cost, candidate_defense);
		sum_armor_vector(*bestArmor_vec, best_cost, best_defense);

		if (candidate_cost <= total_cost)
		{
			if (bestArmor_vec->empty() || candidate_defense > best_defense)
			{
				bestArmor_vec->clear();

				for (int k = 0; k < candidate_vec->size(); k++)
				{
					bestArmor_vec->push_back(candidate_vec->at(k));
				}
			}
		}
	}

	return bestArmor_vec;
}
