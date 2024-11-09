#include <iostream>
#include <vector>
using namespace std;

// Exception classes
class NotFoundException : public exception {
    const char* what() const throw() {
        return "Value not found in the tree";
    }
};

class duplicateInsertion : public exception {
    const char* what() const throw() {
        return "Value already exists in the tree";
    }
};

// Custom implementations of algorithm functions
template<typename Iterator, typename T>
Iterator custom_lower_bound(Iterator first, Iterator last, const T& value) {
    while (first != last) {
        Iterator mid = first + (last - first) / 2;
        if (*mid < value)
            first = mid + 1;
        else
            last = mid;
    }
    return first;
}

template<typename Iterator, typename T>
bool custom_binary_search(Iterator first, Iterator last, const T& value) {
    Iterator it = custom_lower_bound(first, last, value);
    return (it != last && !(value < *it));
}

template <typename DT>
class MTree {
protected:
    int M; // Maximum number of children per node (M+1 way split)
    vector<DT> values; // Values stored in the node (M-1 values)
    vector<MTree*> children; // Pointers to child MTrees (M+1 children)

public:
    MTree(int M) : M(M) {}

    ~MTree() {
        for (auto child : children) {
            delete child;
        }
    }

    bool is_leaf() const {
        return children.empty();
    }

    void insert(const DT& value) {
        // If value already exists, throw exception
        if (search(value)) {
            throw duplicateInsertion();
        }

        // If this is a leaf node
        if (is_leaf()) {
            // Insert value in sorted order using custom lower_bound
            auto it = custom_lower_bound(values.begin(), values.end(), value);
            values.insert(it, value);

            // If node exceeds capacity, split it
            if (values.size() >= M) {
                split_node();
            }
        } else {
            // Find the correct child to insert into
            MTree* child = find_child(value);
            child->insert(value);
        }
    }

    void split_node() {
        if (is_leaf()) {
            // Create two new child nodes
            MTree* left = new MTree(M);
            MTree* right = new MTree(M);

            // Calculate split points
            int total = values.size();
            int leftSize = total / 2;

            // Distribute values to children
            left->values = vector<DT>(values.begin(), values.begin() + leftSize);
            right->values = vector<DT>(values.begin() + leftSize + 1, values.end());

            // Keep middle value in current node
            DT middleValue = values[leftSize];
            values.clear();
            values.push_back(middleValue);

            // Set up child pointers
            children.clear();
            children.push_back(left);
            children.push_back(right);
        }
    }

    MTree* find_child(const DT& value) {
        for (size_t i = 0; i < values.size(); i++) {
            if (value < values[i]) {
                return children[i];
            }
        }
        return children.back();
    }

    bool search(const DT& value) {
        // If leaf node, search in values
        if (is_leaf()) {
            return custom_binary_search(values.begin(), values.end(), value);
        }

        // Find appropriate child and continue search
        MTree* child = find_child(value);
        return child->search(value);
    }

    void remove(const DT& value) {
        if (is_leaf()) {
            auto it = custom_lower_bound(values.begin(), values.end(), value);
            if (it == values.end() || *it != value) {
                throw NotFoundException();
            }
            values.erase(it);
            cout << "The value = " << value << " has been removed." << endl;
        } else {
            MTree* child = find_child(value);
            child->remove(value);
        }
    }

    vector<DT> collect_values() {
        vector<DT> allValues;
        if (is_leaf()) {
            allValues.insert(allValues.end(), values.begin(), values.end());
        } else {
            for (auto child : children) {
                vector<DT> childValues = child->collect_values();
                allValues.insert(allValues.end(), childValues.begin(), childValues.end());
            }
        }
        return allValues;
    }

    void buildTree(vector<DT>& input_values) {
        // Clear existing tree
        for (auto child : children) {
            delete child;
        }
        children.clear();
        values.clear();

        // If input is small enough, make this a leaf node
        if (input_values.size() < M) {
            values = input_values;
            return;
        }

        // Calculate split points based on M
        int D = input_values.size() / M; // Divisor
        
        // Create child nodes
        for (int i = 0; i < M; i++) {
            int start = i * D;
            int end = (i == M-1) ? input_values.size() : (i+1) * D;
            
            if (start < end) {
                vector<DT> childValues(input_values.begin() + start, 
                                     input_values.begin() + end);
                MTree* child = new MTree(M);
                child->buildTree(childValues);
                children.push_back(child);
                
                if (i < M-1 && end < input_values.size()) {
                    values.push_back(input_values[end]);
                }
            }
        }
        cout << "The tree has been rebuilt." << endl;
    }

    bool find(DT value) {
        bool found = search(value);
        if (found) {
            cout << "The element with value = " << value << " was found." << endl;
        } else {
            cout << "The element with value = " << value << " not found." << endl;
        }
        return found;
    }

    void print_final_list() {
        vector<DT> final_values = collect_values();
        cout << "Final list: ";
        int count = 0;
        for (const auto& value : final_values) {
            cout << value << " ";
            count++;
            if (count % 20 == 0 && count < final_values.size()) cout << endl;
        }
        cout << endl;
    }
};

int main() {
    int n;
    cin >> n;
    
    vector<int> mySortedValues(n);
    for (int i = 0; i < n; i++) {
        cin >> mySortedValues[i];
    }

    int M;
    cin >> M;

    MTree<int>* myTree = new MTree<int>(M);
    myTree->buildTree(mySortedValues);

    int numCommands;
    cin >> numCommands;

    char command;
    int value;

    for (int i = 0; i < numCommands; i++) {
        cin >> command;
        switch (command) {
            case 'I': {
                cin >> value;
                try {
                    myTree->insert(value);
                } catch (duplicateInsertion& e) {
                    cout << "The value = " << value << " already in the tree." << endl;
                }
                break;
            }
            case 'R': {
                cin >> value;
                try {
                    myTree->remove(value);
                } catch (NotFoundException& e) {
                    cout << "The value = " << value << " not found." << endl;
                }
                break;
            }
            case 'F': {
                cin >> value;
                myTree->find(value);
                break;
            }
            case 'B': {
                vector<int> myValues = myTree->collect_values();
                myTree->buildTree(myValues);
                break;
            }
            default:
                cout << "Invalid command!" << endl;
        }
    }

    myTree->print_final_list();
    delete myTree;
    return 0;
}