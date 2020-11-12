#include "_list.hpp"
#include <vector>

using namespace std;

int main() {
    // initiliazer list construction
    List<int> list = { 7, 5, 6, 1, 4 };

    vector<int> array = { 4, 6, 7, 8, 2 };

    // List supports several operations with ranges
    List<int> result = list.get_concated(array.begin(), array.end());

    // List can pe printed using std::cout
    cout << "Concating list:" << list << "and array: ( ";
    for(auto element : array)
        cout << element << ' ';

    cout << ")\nResult: ";
    // List can use some standart range operations
    for(auto element : result)
        cout << element << ' ';

    // Iterators provide use with std::sort and others
    std::sort(result.begin(), result.end());
    cout << endl << "Sorted list: " << result << endl;
};
