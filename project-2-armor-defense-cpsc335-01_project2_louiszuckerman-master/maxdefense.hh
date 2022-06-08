///////////////////////////////////////////////////////////////////////////////
// maxdefense.hh
//
// Compute the set of armos that maximizes defense, within a gold budget,
// with the greedy method or exhaustive search.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
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
		double cost_gold,
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
	double cost() const { return _cost_gold; }
	double defense() const { return _defense_points; }

	//
private:
	// Human-readable description of the armor, e.g. "new enchanted helmet". Must be non-empty.
	std::string _description;

	// Cost, in units of gold; Must be positive
	double _cost_gold;

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
	double &total_cost,
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
// followed by the total gold cost and defense in it.
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

		double total_cost, total_defense;
		sum_armor_vector(armors, total_cost, total_defense);
		std::cout
			<< "> Grand total cost: " << total_cost << " gold" << std::endl
			<< "> Grand total defense: " << total_defense
			<< std::endl;
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
	// TODO: implement this function, then delete the return statement below
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
	//std::cout << "Total Size: " << filtered_vector->size();
	return filtered_vector;
}

// Compute the optimal set of armor items with a greedy algorithm.
// Specifically, among the armor items that fit within a total_cost gold budget,
// choose the armors whose defense is greatest.
// Repeat until no more armor items can be chosen, either because we've run out of armor items,
// or run out of gold.
std::unique_ptr<ArmorVector> greedy_max_defense(
	const ArmorVector &armors,
	double total_cost)
{
	// TODO: implement this function, then delete the return statement below
	std::unique_ptr<ArmorVector> result(new ArmorVector);

	double result_cost = 0;
	auto bestArmor = std::shared_ptr<ArmorItem>(new ArmorItem("test helmet", 1, 0));
	double size = armors.size();

	std::unique_ptr<ArmorVector> todo(new ArmorVector(armors));

	while (!todo->empty())
	{
		double candidate, best = 0;
		int index_best = 0;

		for (int j = 0; j < todo->size(); j++)
		{
			candidate = todo->at(j)->defense() / todo->at(j)->cost();

			if (candidate > best)
			{
				best = candidate;
				bestArmor = todo->at(j);
				index_best = j;
			}
		}

		todo->erase(todo->begin() + index_best);

		double temp_cost = bestArmor->cost();
		if (temp_cost + result_cost <= total_cost)
		{
			result->push_back(bestArmor);
			result_cost += bestArmor->cost();
		}
	}

	return result;
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

	// TODO: implement this function, then delete the return statement below
	std::unique_ptr<ArmorVector> bestArmor_vec(new ArmorVector);
	double candidate_cost, best_cost, candidate_defense, best_defense;

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
