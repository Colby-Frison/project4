#include <iostream>
#include <vector>

using namespace std;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Colby Frison
CS-2413 ~ Data Structure
Project4 ~ M-Tree

An M-tree is basically a 2,3 tree. the tree consists of 3 main variables, 'M', 'values', and 'children'. Starting
with 'M' it means there are M children per node, and 'M-1' values per node; so if M=3 then each node contains 2 
values and has 3 children nodes. The next is 'values' which simply store the values in that given node, its pretty
much the _info. The last is 'children' it is a vector of M-tree nodes that are the children of the given node. This 
makes recursion very simple as it makes each node its own tree (method calls work on children nodes as well as roots)

 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


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

// implementations of algorithm functions
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

/* * * * * * * * * * * * * * * * * * * * * * * * * *
The following is a series of helper functions that are needed
in order to handle the output.

This is like overly complicated for what its doing, but since
the instructions don't allow the string library it has to be done.

* * * * * * * * * * * * * * * * * * * * * * * * * */

void intToCharVector(int num, vector<char>& output) {
    if (num == 0) {
        output.push_back('0');
        return;
    }

    vector<char> temp;
    bool isNegative = false;

    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    while (num > 0) {
        temp.push_back('0' + num % 10);
        num /= 10;
    }

    if (isNegative) {
        temp.push_back('-');
    }

    // Reverse the temp vector and append to output
    for (auto it = temp.rbegin(); it != temp.rend(); ++it) {
        output.push_back(*it);
    }
}

// Helper function to add an integer to a char vector string
void addIntToOutput(vector<char>& output, int num) {
    intToCharVector(num, output);
    //output.push_back(' '); // Add a space after the number
}

// Helper function to add a word to a char vector
void addWordToOutput(vector<char>& output, const string& word) {
    for (char c : word) {
        output.push_back(c);
    }
}

// Helper function to add a new line to the output
void addNewLineToOutput(vector<char>& output) {
    output.push_back('\n');
}

void appendVectorToOutput(vector<char>& output, const vector<char>& toAppend){
    output.insert(output.end(), toAppend.begin(), toAppend.end());
}


// MTree class, main section of program
template <typename DT>
class MTree {
protected:
    int M; // Maximum number of children per node (M+1 way split)
    vector<DT> values; // Values stored in the node (M-1 values)
    vector<MTree*> children; // Pointers to child MTrees (M children) ~ Check on this number cuz there was mixed instructions
    // the pdf says m-1 for values and m for children, but in the code segments it says m-1 then m+1 so idk

public:
    // initialize tree, set max nodes per child
    MTree(int M) : M(M) {
        //know size of vector so reserve size to save time
        values.reserve(M-1);
        children.reserve(M); // check this number
    }

    // loop through entire tree deleting each
    ~MTree() {
        // auto just deduces data type automatically, no real difference, but idk
        for (auto child : children) {
            delete child;
        }
    }

    // if the node has no children its a leaf, good identifier of when to end loop/recursion
    bool is_leaf() const {
        return children.empty(); // vector method, if empty returns true, this is just a remaster of that
    }


    /* * * * * * * * Insert Method * * * * * * * * *
    Checks if value already exists in the tree, if not move on.abort

    Then check if node is a leaf, is so find where the val goes, then insert
    if the number of values exceed M-1 go to split node method. This is the 
    base case that stops the recursion

    if it is not a leaf recursively traverse the method to find the right
    insert location
    * * * * * * * * * * * * * * * * * * * * * * * * */
    void insert(const DT& value) {
        // use search to see if value is already in the tree, if so throw exception
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
            child->insert(value); // recursive statement
        }
    }

    /* * * * * * * Split_Node Method * * * * * * * * *
    Create left and right nodes to split values to, then split original
    values list in half assigning values to each node.abort

    Middle value is retained, then the original node is cleared and 
    middle is pushed back into the vector. then children are declared

    * * * * * * * * * * * * * * * * * * * * * * * * */
    void split_node() {

        if (is_leaf()) {
            // Clear existing children
            for (auto child : children) {
                delete child;
            }
            children.clear();

            int total = values.size();
            int D = total / M;  // Divisor
            
            vector<MTree*> newChildren;
            vector<DT> newValues;

            for (int i = 0; i < M; i++) {
                int start = i * D;
                int end = (i == M - 1) ? total : (i + 1) * D;

                if (start < end) {
                    MTree* child = new MTree(M);
                    child->values.assign(values.begin() + start, values.begin() + end);
                    newChildren.push_back(child);
                    
                    if (i < M - 1 && end < total) {
                        newValues.push_back(values[end]);
                    }
                }
            }

            values = newValues;
            children = newChildren;

        }
    }


    /* * * * * find_child Method * * * * * * * * *
    This is a simple helper method in a few different methods.
    It iterates through the values vector, if the target value is less than the 
    val in the vector then if it exists it would be in that child. It continues
    to do that until the end of the vector. If it does not find it it returns the 
    last child in the children vector.
    * * * * * * * * * * * * * * * * * * * * * * */
    MTree* find_child(const DT& value) {
        for (size_t i = 0; i < values.size(); i++) {
            if (value < values[i]) {
                return children[i];
            }
        }
        return children.back();
    }

    /* * * * * Search Method * * * * * * * * * * *
    Checks if leaf, if it is check values vector, if its there return true
    if not return false. This is the base case of the recursive method.

    Then finds child node where it would be based on pushed value. Then recursive
    call is made to continue search.
    * * * * * * * * * * * * * * * * * * * * * * */
    bool search(const DT& value) {
        // If leaf node, search in values
        if (is_leaf()) {
            return custom_binary_search(values.begin(), values.end(), value);
        }

        // Find appropriate child and continue search
        MTree* child = find_child(value);
        return child->search(value);
    }


    /* * * * * Remove Method * * * * * * * * * * *
    Simple remove method, the first if statement is in regards to leaf nodes.
    It first uses the helper method to get a pointer to the location, or lack there
    of, of the value. if its pointing to the end of the vector, that means value
    is greater than all vals in the vector, meaning its not in it, the second statement
    checks to see if the value being pointed to is == to the value. These are both crucial
    in keeping everything defined and correct. it those are both false it deletes the value 
    that position in the values vector.

    If its note a leaf it uses the find_child to get the proper child node, then makes a 
    recursive call to repeat the above process in teh child node.
    * * * * * * * * * * * * * * * * * * * * * * */
    void remove(const DT& value) {
        if (is_leaf()) {
            auto it = custom_lower_bound(values.begin(), values.end(), value);
            if (it == values.end() || *it != value) {
                throw NotFoundException();
            }
            values.erase(it);
            //cout << "The value = " << value << " has been removed." << endl; // change this
        } else {
            MTree* child = find_child(value);
            child->remove(value);
        }
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
            //output statement for start

            int end;
            if (i == M-1) {
                end = input_values.size();
            } else {
                end = (i + 1) * D;
            }

            
            if (start < end) {
                vector<DT> childValues(input_values.begin() + start, 
                                     input_values.begin() + end);
                MTree* child = new MTree(M);
                child->buildTree(childValues); // recursive call
                children.push_back(child);
                
                if (i < M-1 && end < input_values.size()) {
                    values.push_back(input_values[end]);
                }
            }
        }
    }

    /* * * * * Collect Values Method * * * * * *
    This one is also pretty simple. First starts by checking leaf nodes.
    just uses inbuilt vector functions to add all elements from values vector
    to allValues vector. 

    Then uses recursion to gather values of child nodes then adds it to allValues
    Then return it... pretty simple
    * * * * * * * * * * * * * * * * * * * * * * */
    vector<DT> collect_values() {
        vector<DT> allValues;
        if (is_leaf()) {
            allValues.insert(allValues.end(), values.begin(), values.end());
        } else {
            for (auto child : children) {
                vector<DT> childValues = child->collect_values(); // recursive call
                allValues.insert(allValues.end(), childValues.begin(), childValues.end());
            }
        }
        return allValues;
    }

    /* * * * * * * * find Method * * * * * * * * *
    The find method is basically just a call to the search method with some cout statements.
    It also returns a bool based on if the value was found.
    * * * * * * * * * * * * * * * * * * * * * * */
    bool find(DT value) {
        bool found = search(value);
        return found;
    }

    // I have it printing the full list at the end, but I dont think this is useful so probably delete 
    // if it isn't really needed
    vector<char> print_final_list(){
        vector<DT> final_values = collect_values();

        vector<char> output;

        addWordToOutput(output, "Final list: ");
        int count = 0;
        for (const auto& value : final_values) {
            addIntToOutput(output, value);
            addWordToOutput(output, " ");
            count++;
            if (count % 20 == 0 && count < final_values.size()) 
                addNewLineToOutput(output);
        }
        addNewLineToOutput(output);

        return output;
    }
};

// main method, compare with instructions to make sure all is good
int main() {
    // output handler
    vector<char> output;

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
                    addWordToOutput(output, "The value = ");
                    addIntToOutput(output, value);
                    addWordToOutput(output, " has been inserted.");
                    addNewLineToOutput(output);

                    // value 121 is removed after 122 is inserted
                } catch (duplicateInsertion& e) {
                    addWordToOutput(output, "The value = ");
                    addIntToOutput(output, value);
                    addWordToOutput(output, " already in the tree.");
                    addNewLineToOutput(output);
                }
                break;
            }
            case 'R': {
                cin >> value;
                try {
                    myTree->remove(value);
                    addWordToOutput(output, "The value = ");
                    addIntToOutput(output, value);
                    addWordToOutput(output, " has been removed.");
                    addNewLineToOutput(output);
                } catch (NotFoundException& e) {
                    addWordToOutput(output, "The value = ");
                    addIntToOutput(output, value);
                    addWordToOutput(output, " not found.");
                    addNewLineToOutput(output);
                }
                break;
            }
            case 'F': {
                cin >> value;
                addWordToOutput(output, "The element with value = ");
                addIntToOutput(output, value);
                if(myTree->find(value)){
                    addWordToOutput(output, " was found.");
                    addNewLineToOutput(output);
                } else {
                    addWordToOutput(output, " not found.");
                    addNewLineToOutput(output);
                }
                break;
            }
            case 'B': {
                vector<int> myValues = myTree->collect_values();
                myTree->buildTree(myValues);
                addWordToOutput(output, "The tree has been rebuilt.");
                addNewLineToOutput(output);
                
                /*
                vector<char> temp;
                appendVectorToOutput(temp, myTree->print_final_list());
                string temp1(temp.begin(), temp.end());// create string based on output vector
                cout << temp1 << endl;
                */

                break;
            }
            default:
                addWordToOutput(output, "Invalid command!");
                addNewLineToOutput(output);
        }
    }

    appendVectorToOutput(output, myTree->print_final_list());
    delete myTree;

    string result(output.begin(), output.end());// create string based on output vector
    cout << result << endl; // output result string

    return 0;
}