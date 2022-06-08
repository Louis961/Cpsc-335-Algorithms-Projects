#include "maxdefense.hh"
#include <iostream>
#include "timer.hh"


int main()
{
	int dynamic_n = 4000;		//alter for test
	std::unique_ptr<ArmorVector> all_armor = load_armor_database("armor.csv");
	std::unique_ptr<ArmorVector> dynamic_armors = filter_armor_vector(*all_armor, 1, 2500, dynamic_n);

	Timer time;

	dynamic_max_defense(*dynamic_armors, 100);
	double elapsed = time.elapsed();

	std::cout << "Dynamic n: " << dynamic_n << " Time: " << elapsed << std::endl;

	int exhaustive_n = 20;		//alter for test
	std::unique_ptr<ArmorVector> exhaustive_armors = filter_armor_vector(*all_armor, 1, 2500, exhaustive_n);
	exhaustive_max_defense(*exhaustive_armors, 100);
	elapsed = time.elapsed();

	std::cout << "Exhaustive n: " << exhaustive_n << " Time: " << elapsed << std::endl;


	return 0;
}
