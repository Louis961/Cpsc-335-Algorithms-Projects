#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "ItemCollection.hpp"

using std::string;
using std::ifstream;
using std::cout;
using std::endl;

// function to return the hash value based on the first digit
unsigned int hashfct1(unsigned int barcode) {
  // TO BE COMPLETED
    unsigned int d;
    int pos = 7;
    d = (int)(barcode/pow(10,(pos-1))) % 10;

    return d;
}

// function to return the hash value based on the second digit
unsigned int hashfct2(unsigned int barcode) {
  // TO BE COMPLETED
    unsigned int d;
    int pos = 6;
    d = (int)(barcode/pow(10,(pos-1))) % 10;
    return d;
}

// function to return the hash value based on the third digit
unsigned int hashfct3(unsigned int barcode) {
  // TO BE COMPLETED
    unsigned int d;
    int pos = 5;
    d = (int)(barcode/pow(10,(pos-1))) % 10;
    return d;
}

// function to return the hash value based on the fourth digit
unsigned int hashfct4(unsigned int barcode) {
  // TO BE COMPLETED
    unsigned int d;
    int pos = 4;
    d = (int)(barcode/pow(10,(pos-1))) % 10;
    return d;
}

// function to return the hash value based on the fifth digit
unsigned int hashfct5(unsigned int barcode) {
    // TO BE COMPLETED
    unsigned int d;
    int pos = 3;
    d = (int)(barcode/pow(10,(pos-1))) % 10;
    return d;
}

// function to return the hash value based on the fourth digit
unsigned int hashfct6(unsigned int barcode) {
    // TO BE COMPLETED
    unsigned int d;
    int pos = 2;
    d = (int)(barcode/pow(10,(pos-1))) % 10;
    return d;
}

// function to return the hash value based on the fifth digit
unsigned int hashfct7(unsigned int barcode) {
    // TO BE COMPLETED
    unsigned int d;
    int pos = 1;
    d = (int)(barcode/pow(10,(pos-1))) % 10;
    return d;
}


// Constructor for struct Item
Item::Item(string itemColor, string itemShape, string itemBrand,
		 unsigned int barcode):itemColor_(itemColor),itemShape_(itemShape), itemBrand_(itemBrand),
				       barcode_(barcode)
{};

// Load information from a text file with the given filename
// THIS FUNCTION IS COMPLETE
void ItemCollection::readTextfile(string filename) {
  ifstream myfile(filename);

  if (myfile.is_open()) {
    cout << "Successfully opened file " << filename << endl;
    string itemColor;
    string itemShape;
    string itemBrand;
    unsigned int barcode;
    while (myfile >> itemColor >> itemShape >> itemBrand >> barcode) {
			if (itemColor.size() > 0)
      	addItem(itemColor, itemShape, itemBrand, barcode);
    }
    myfile.close();
  }
  else
    throw std::invalid_argument("Could not open file " + filename);
}

void ItemCollection::addItem(string itemColor, string itemShape, string itemBrand, unsigned int barcode) {
  // TO BE COMPLETED
  // function that adds the specified pair of glasses to main display (i.e., to all hash tables)

  // Given information about one pair of glasses, create an Item object and insert into each of the seven hashtables.  
  // Note that each hashtable has the product number as the key and a Item object as the value. To be completed.
  Item tempItem(itemColor, itemShape, itemBrand, barcode);
  hT1[barcode], hT2[barcode], hT3[barcode], hT4[barcode], hT5[barcode], hT6[barcode], hT7[barcode] = tempItem;
}

bool ItemCollection::removeItem(unsigned int barcode) {
  // TO BE COMPLETED
  // function that removes the pair of glasses specified by the barcode from the display
  // if pair is found, then it is removed and the function returns true
  // else returns false
  if (hT1.find(barcode) == hT1.end())
  {
    return false;
  }
  
  else
  {
    auto removeIterator = hT1.find(barcode);
    hT1.erase(removeIterator);

    removeIterator = hT2.find(barcode);
    hT2.erase(removeIterator);

    removeIterator = hT3.find(barcode);
    hT3.erase(removeIterator);

    removeIterator = hT4.find(barcode);
    hT4.erase(removeIterator);

    removeIterator = hT5.find(barcode);
    hT5.erase(removeIterator);

    removeIterator = hT6.find(barcode);
    hT6.erase(removeIterator);

    removeIterator = hT7.find(barcode);
    hT7.erase(removeIterator);
  }

  return true;
}

unsigned int ItemCollection::bestHashing()
{
  // TO BE COMPLETED
  // function that decides the best hash function, i.e. the ones among
  // fct1-fct7 that creates the most balanced hash table for the current
  // ItemCollection data member allItems

  // Hints:
  // Calculate the balance of each hashtable, one by one.
  /*
	 for (unsigned i=0; i<10; ++i) {
    cout << hT1.bucket_size(i); // Call bucket_size() to get the size of each bucket
  }
	*/
  // Then, calculate the lowest balance

  unsigned int count1, count2, count3, count4, count5, count6, count7;
  unsigned int min1, min2, min3, min4, min5, min6, min7, max1, max2, max3, max4, max5, max6, max7;
  unsigned int bucketTotals[10];

  // Hash 1 place count of buckets in array bucketTotals
  for (unsigned i = 0; i < 10; i++)
  {
    count1 = hT1.bucket_size(i); // Call bucket_size() to get the size of each bucket

    bucketTotals[i] = count1; //assign bucket size to array for processing
  }

  min1 = 99999;
  max1 = 0;

  for (unsigned i = 0; i < 10; i++)
  {
    // find minimum bucket for hash
    if (bucketTotals[i] < min1)
    {
      min1 = bucketTotals[i];
    }

    // find maximum bucket for hash
    if (bucketTotals[i] > max1)
    {
      max1 = bucketTotals[i];
    }
  }

  // Hash 2
  for (unsigned i = 0; i < 10; i++)
  {
    count2 = hT2.bucket_size(i);

    bucketTotals[i] = count2;
  }

  min2 = 99999;
  max2 = 0;

  for (unsigned i = 0; i < 10; i++)
  {
    if (bucketTotals[i] < min2)
    {
      min2 = bucketTotals[i];
    }

    if (bucketTotals[i] > max2)
    {
      max2 = bucketTotals[i];
    }
  }

  // Hash 3
  for (unsigned i = 0; i < 10; i++)
  {
    count3 = hT3.bucket_size(i);

    bucketTotals[i] = count3;
  }

  min3 = 99999;
  max3 = 0;

  for (unsigned i = 0; i < 10; i++)
  {
    if (bucketTotals[i] < min3)
    {
      min3 = bucketTotals[i];
    }

    if (bucketTotals[i] > max3)
    {
      max3 = bucketTotals[i];
    }
  }

  // Hash 4
  for (unsigned i = 0; i < 10; i++)
  {
    count4 = hT4.bucket_size(i);

    bucketTotals[i] = count4;
  }

  min4 = 99999;
  max4 = 0;

  for (unsigned i = 0; i < 10; i++)
  {
    if (bucketTotals[i] < min4)
    {
      min4 = bucketTotals[i];
    }

    if (bucketTotals[i] > max4)
    {
      max4 = bucketTotals[i];
    }
  }

  // Hash 5
  for (unsigned i = 0; i < 10; i++)
  {
    count5 = hT5.bucket_size(i);

    bucketTotals[i] = count5;
  }

  min5 = 99999;
  max5 = 0;

  for (unsigned i = 0; i < 10; i++)
  {
    if (bucketTotals[i] < min5)
    {
      min5 = bucketTotals[i];
    }

    if (bucketTotals[i] > max5)
    {
      max5 = bucketTotals[i];
    }
  }

  // Hash 6
  for (unsigned i = 0; i < 10; i++)
  {
    count6 = hT6.bucket_size(i);

    bucketTotals[i] = count6;
  }

  min6 = 99999;
  max6 = 0;

  for (unsigned i = 0; i < 10; i++)
  {
    if (bucketTotals[i] < min6)
    {
      min6 = bucketTotals[i];
    }

    if (bucketTotals[i] > max6)
    {
      max6 = bucketTotals[i];
    }
  }

  // Hash 7
  for (unsigned i = 0; i < 10; i++)
  {
    count7 = hT7.bucket_size(i);

    bucketTotals[i] = count7;
  }

  min7 = 99999;
  max7 = 0;

  for (unsigned i = 0; i < 10; i++)
  {
    if (bucketTotals[i] < min7)
    {
      min7 = bucketTotals[i];
    }

    if (bucketTotals[i] > max7)
    {
      max7 = bucketTotals[i];
    }
  }

  unsigned int balance[7];

  // populate balance array with difference between maximum and minimum bucket totals
  balance[0] = max1 - min1;
  balance[1] = max2 - min2;
  balance[2] = max3 - min3;
  balance[3] = max4 - min4;
  balance[4] = max5 - min5;
  balance[5] = max6 - min6;
  balance[6] = max7 - min7;

  unsigned int min_loc = 0;

  // finds the first minimum element index in balance
  for (unsigned int i = 0; i < 7; i++)
  {
    if (balance[i] < balance[min_loc])
    {
      min_loc = i;
    }
  }

// min_loc+1 accounts for array index starting at 0
  return min_loc + 1;
}

// ALREADY COMPLETED
size_t ItemCollection::size() {
    if ((hT1.size() != hT2.size()) || (hT1.size() != hT3.size()) || (hT1.size() != hT4.size()) || (hT1.size() != hT5.size())|| (hT1.size() != hT6.size()) || (hT1.size() != hT7.size()))
  	throw std::length_error("Hash table sizes are not the same");
    
	return hT1.size();
}
