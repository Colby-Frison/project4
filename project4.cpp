#include <iostream>
#include <vector>

using namespace std;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Colby Frison
CS-2413 ~ Data Structure
Project4 ~ M-Tree

An M-tree is basically a 2,3 tree. The tree consists of 3 main variables, 'M', 'values', and 'children'. 
Starting with 'M' it means there are M children per node, and 'M-1' values per node; so if M=3 then each node 
contains 2 values and has 3 children nodes. The next is 'values' which simply store the values in that given 
node, its pretty much the _info. The last is 'children' it is a vector of M-tree nodes that are the children of 
the given node. This makes recursion very simple as it makes each node its own tree (method calls work on 
children nodes as well as roots).

Output is handled in a kind of strange way. In previous projects the string library was allowed so I could just 
add everything to a string then print the string. Since it isn't allowed outputs are broken into characters and
put into a vector. Then the vector is outputted, this is described more in detail at its section in the code.
This way of outputting stuff makes the main method look a little messy, but its still the same functionality of
the provided main method.

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Part 2: Line 548

Part 3: Line 888

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


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


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Lower bound is a method used to find the lowest bound an element can be inserted without breaking the order of 
elements. By using an iterator the method returns a pointer to this element allowing for quicker access and less
work to traverse the following elements as well.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
template<typename Iterator, typename T>
Iterator lowerBound(Iterator first, Iterator last, const T& value) {
    while (first != last) {
        Iterator mid = first + (last - first) / 2;
        if (*mid < value)
            first = mid + 1;
        else
            last = mid;
    }
    return first;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
This method is essentially an extension of lowerBound that uses the returned iterator to see if the provided 
value is in the dataset. The actual return statement first checks to make sure the iterator is not past the last 
element of the array, then it checks to see if the value the iterator is pointing to is not less than value. 
This essentially checks if value is == *it since the lower bound finds the first position where the value is not 
less than the given value.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
template<typename Iterator, typename T>
bool binarySearch(Iterator first, Iterator last, const T& value) {
    Iterator it = lowerBound(first, last, value);
    return (it != last && !(value < *it));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
The following few methods are all made to handle the output. Since the string library wasn't allowed I couldn't 
just add the output to a string then output it like normal so this stuff had to be done.

The output is handled by a vector of character. Both numbers(in character form), characters, and new lines are 
put here to properly handle the output.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// make the int able to be put in the char vector
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

// add int made char to the output vector
void addIntToOutput(vector<char>& output, int num) {
    intToCharVector(num, output);
}

// add a word to the output vector
void addWordToOutput(vector<char>& output, const string& word) {
    for (char c : word) {
        output.push_back(c);
    }
}

// add a new line to the output vector
void addNewLineToOutput(vector<char>& output) {
    output.push_back('\n');
}

// Add a vector to the output vector. This is used in the print final list method since it also outputs a vector.
void appendVectorToOutput(vector<char>& output, const vector<char>& toAppend){
    output.insert(output.end(), toAppend.begin(), toAppend.end());
}



// MTree class, main section of program
template <typename DT>
class MTree {
protected:
    int M; // Maximum number of children per node
    vector<DT> values; // Values stored in the node (M-1 values)
    vector<MTree*> children; // Pointers to child MTrees (M children)

public:
    // initialize tree, set max nodes per child
    MTree(int M) : M(M) {
        //know size of vector so reserve size to save time
        values.reserve(M-1);
        children.reserve(M);
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


    /* * * * * * * * Insert Method * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Checks if value already exists in the tree, if not move on.abort

    Then check if node is a leaf, is so find where the val goes, then insert
    if the number of values exceed M-1 go to split node method. This is the 
    base case that stops the recursion

    if it is not a leaf recursively traverse the method to find the right
    insert location
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    void insert(const DT& value) {
        // use search to see if value is already in the tree, if so throw exception
        if (search(value)) {
            throw duplicateInsertion();
        }

        // If this is a leaf node
        if (is_leaf()) {
            // Insert value in sorted order using custom lower_bound
            auto it = lowerBound(values.begin(), values.end(), value);
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

    /* * * * * * * Split_Node Method * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    The split node method follows the logic of buildTree essentially one to one, the reason this isn't called 
    inside buildTree is because buildTree was made first then I used relatively the same logic here. I would go 
    back and make adjustments, but I'm scared to touch it. In short the function of the method is as follows: 

    It splits the current node into M child nodes, distributing the values evenly and updating the current node 
    with the appropriate separator values
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
                int end;
                if (i == M-1) {
                    end = input_values.size();
                } else {
                    end = (i + 1) * D;
                }

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


    /* * * * * find_child Method * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    This is a simple helper method in a few different methods.
    It iterates through the values vector, if the target value is less than the 
    val in the vector then if it exists it would be in that child. It continues
    to do that until the end of the vector. If it does not find it it returns the 
    last child in the children vector.
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    MTree* find_child(const DT& value) {
        for (size_t i = 0; i < values.size(); i++) {
            if (value < values[i]) {
                return children[i];
            }
        }
        return children.back();
    }

    /* * * * * Search Method * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Checks if leaf, if it is check values vector, if its there return true
    if not return false. This is the base case of the recursive method.

    Then finds child node where it would be based on pushed value. Then recursive
    call is made to continue search.
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    bool search(const DT& value) {
        // If leaf node, search in values
        if (is_leaf()) {
            return binarySearch(values.begin(), values.end(), value);
        }

        // Find appropriate child and continue search
        MTree* child = find_child(value);
        return child->search(value);
    }


    /* * * * * Remove Method * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    Simple remove method, the first if statement is in regards to leaf nodes.
    It first uses the helper method to get a pointer to the location, or lack there
    of, of the value. if its pointing to the end of the vector, that means value
    is greater than all vals in the vector, meaning its not in it, the second statement
    checks to see if the value being pointed to is == to the value. These are both crucial
    in keeping everything defined and correct. it those are both false it deletes the value 
    that position in the values vector.

    If its note a leaf it uses the find_child to get the proper child node, then makes a 
    recursive call to repeat the above process in teh child node.
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    void remove(const DT& value) {
        if (is_leaf()) {
            auto it = lowerBound(values.begin(), values.end(), value);
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

    /* * * * * BuildTree Method * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    The build tree method is probably one of the more important methods in the MTree class.
    This method constructs an MTree from a given set of elements. It organizes the tree structure 
    by splitting the input values into smaller manageable subtrees through recursion.

    The basic process it goes through is:
        1. Clear the tree
        2. Check to see if the number of values are less than M
             If so make a leaf node and end recursion there
        3. Calculate how many values each child node will have
        4. Actually create the child nodes and input proper amount of values
             find the values that go into that specific child node
             create vector to hold these values
             recursively call to create a new Mtree child node using the sub-vector previously created
             add these newly created children nodes to the children vector
             add the split value; if node is not the last one add the value a the end index to the values vector

    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    void buildTree(vector<DT>& input_values) {

        // Clear existing tree
        ~MTree();

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

    /* * * * * Collect Values Method * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    This one is also pretty simple. First starts by checking leaf nodes.
    just uses inbuilt vector functions to add all elements from values vector
    to allValues vector. 

    Then uses recursion to gather values of child nodes then adds it to allValues
    Then return it... pretty simple
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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

    /* * * * * * * * find Method * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    The find method is basically just a call to the search method with some cout statements.
    It also returns a bool based on if the value was found.
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    bool find(DT value) {
        bool found = search(value);
        return found;
    }

    /* * * * * * * * print final list Method * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    This method just prints out the entire m-tree, and returns it as a char vector. The strange output 
    is to make the addition of it to the output vector easier. This is what the append vector to output 
    method is for.
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
            case 'I': { // insert
                cin >> value;

                try {
                    myTree->insert(value);

                    // add output
                    addWordToOutput(output, "The value = ");
                    addIntToOutput(output, value);
                    addWordToOutput(output, " has been inserted.");
                    addNewLineToOutput(output);
                } catch (duplicateInsertion& e) {
                    // add output
                    addWordToOutput(output, "The value = ");
                    addIntToOutput(output, value);
                    addWordToOutput(output, " already in the tree.");
                    addNewLineToOutput(output);
                }

                break;
            }
            case 'R': { // remove
                cin >> value;

                try {
                    myTree->remove(value);

                    // add output
                    addWordToOutput(output, "The value = ");
                    addIntToOutput(output, value);
                    addWordToOutput(output, " has been removed.");
                    addNewLineToOutput(output);
                } catch (NotFoundException& e) {
                    // add output
                    addWordToOutput(output, "The value = ");
                    addIntToOutput(output, value);
                    addWordToOutput(output, " not found.");
                    addNewLineToOutput(output);
                }

                break;
            }
            case 'F': { // find
                cin >> value;

                // add first part of the output
                addWordToOutput(output, "The element with value = ");
                addIntToOutput(output, value);

                // use find to see if it was in or not
                if(myTree->find(value)){
                    addWordToOutput(output, " was found.");
                    addNewLineToOutput(output);
                } else {
                    addWordToOutput(output, " not found.");
                    addNewLineToOutput(output);
                }

                break;
            }
            case 'B': { // rebuild tree
                vector<int> myValues = myTree->collect_values();
                myTree->buildTree(myValues);

                addWordToOutput(output, "The tree has been rebuilt.");
                addNewLineToOutput(output);
                
                break;
            }
            default: {
                addWordToOutput(output, "Invalid command!");
                addNewLineToOutput(output);
            }
        }
    }

    appendVectorToOutput(output, myTree->print_final_list());
    delete myTree;

    string result(output.begin(), output.end());// create string based on output vector
    cout << result << endl; // output result string

    return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 
part 2 : LLM usage

This project didn't have a lot of LLM usage, the main use was creating a general outline of the code based on the 
instructions along with understanding the logic of certain functions, then a bunch of smaller uses that were used
to understand certain syntax, and how to do certain things, but I tried to rely on actual sources instead of just
going straight to AI. I also used an LLM to figure out tht output I talked about it earlier, but in short; no 
string library -> issues creating output string -> char vector. Then an LLM helped create the char vector methods.
Theres also those algorithms at the beginning that a LLM helped create.

I used Chat-gpt to help with the initial creation since it can read PDFs. But i wanted to try out microsoft's 
copilot LLM because its free, and its unlimited (unlike chat-gpt). It worked pretty well, but its website is kinda
messed up, whenever you try to copy and paste anything it pastes to a single line instead of formatting the way 
it looks on the website. Because of that Ill do my best to reorganize the responses, but I may also end up just
summarizing the response. 

I'll start with chat-gpt then go to microsoft copilot

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

Me: 
    Here are the instructions for a project, write an outline for the code that complies with all specifications.

__________________________________________________________________________________________________________________

Chat-gpt: 
    Here’s the outline and initial structure for the MTree class in C++ based on the instructions from your 
    project file:

    Class Definition and Node Properties: The MTree class should handle the multiway search tree (M-tree) 
    operations: insertion, deletion, search, and rebuilding the tree to maintain balance.

    Methods and Functionality:
        Insertion (insert): Add values to the tree, with node splitting when necessary.
        Deletion (remove): Remove specified values from the tree.
        Search (search and find): Locate values in the tree, traversing nodes by split points.
        Rebuilding (buildTree): Reconstruct the tree from all current values for balance.

    Exception Handling:
        duplicateInsertion if a value already exists.
        NotFoundException if attempting to remove a value not in the tree.

    Here’s a partial implementation to start with, including handling template-based values and some functions:

    #include <iostream>
    #include <vector>
    #include <algorithm> // for std::lower_bound

    using namespace std;

    class duplicateInsertion : public exception {
    public:
        const char* what() const noexcept override {
            return "Duplicate insertion error: value already exists in the tree.";
        }
    };

    class NotFoundException : public exception {
    public:
        const char* what() const noexcept override {
            return "Value not found in the tree.";
        }
    };

    template <typename DT>
    class MTree {
    protected:
        int M;
        vector<DT> values;  // M-1 values in non-leaf nodes
        vector<MTree*> children;  // M children

        

    public:
        MTree(int M) : M(M) {}

        ~MTree() {
            for (auto child : children) delete child;
        }
        
        bool is_leaf() const {
            return children.empty();
        }

        void insert(const DT& value) {
            if (find(value)) {
                throw duplicateInsertion();
            }
            // Insert into appropriate leaf and handle splitting if needed
        }

        bool search(const DT& value) {
            if (is_leaf()) {
                // Search in leaf node
                return search(values.begin(), values.end(), value) != values.end();
            } else {
                // Traverse child nodes based on split points
            }
            return false;
        }

        MTree* find_child(const DT& value) {
            // Find the correct child node to navigate to
        }

        void remove(const DT& value) {
            if (!find(value)) {
                throw NotFoundException();
            }
            // Remove value and handle rebalancing
        }

        void split_node() {
            // Split the node if it exceeds capacity (i.e., >= M)
        }

        void buildTree(vector<DT>& input_values) {
            // Build or rebuild the tree from the sorted values
        }

        vector<DT> collect_values() {
            // Collect all leaf node values into a sorted vector
            vector<DT> all_values;
            return all_values;
        }
    };

    The main program will leverage this MTree class and its methods to handle the commands, as specified in your 
    example. Let me know if you need further assistance with specific methods or modifications!

Me: 
    The algorithm library is not allowed, generate the lower_bound method you said is needed so the library
    does not need to be inputted.

__________________________________________________________________________________________________________________

Chat-gpt: 

    Here's an implementation of the lower_bound method that avoids the <algorithm> library:

    template <typename Iterator, typename T>
    Iterator custom_lower_bound(Iterator first, Iterator last, const T& value) {
        while (first != last) {
            Iterator mid = first + (last - first) / 2;
            if (*mid < value) {
                first = mid + 1;
            } else {
                last = mid;
            }
        }
        return first;
    }

__________________________________________________________________________________________________________________

That'll be the only time I put a large code segment since its the only large code segment made by AI. There are 
still a few small ones though.

Next up is the Microsoft copilot usage.
__________________________________________________________________________________________________________________

Me: 
    Here is the class I have been working on in C++ called MTree. It was created partially using AI so there are 
    some methods that are confusing, I'm going to ask questions about it so please familiarize yourself with the 
    class. 

    **I also gave it the mostly completed class**

__________________________________________________________________________________________________________________

Copilot: 
    Got it! I’ll familiarize myself with the M-Tree class. Feel free to ask any questions you have. Here’s a 
    quick overview of what I see:

    **Overview of all methods, exception classes and helper functions**

__________________________________________________________________________________________________________________

Various small questions mainly for syntax, and certain questions about method logic, but no code stuff really.
__________________________________________________________________________________________________________________

Me: 
    I made some adjustments, but the code is now too long to send. The general idea is i changed all cout into 
    adding to a string. The issue is I am not able to use #include <string>, how can I add integers to the output 
    string without using any additional inputs

    remember the only libraries I'm allowed to use is vector and iostream, how can i add integers to a string only 
    with these 2 includes

__________________________________________________________________________________________________________________

Copilot: 
    Understood! Without the <string> and <sstream> libraries, you can still achieve this by constructing a string 
    manually using std::vector and std::iostream. One approach is to use character arrays or a std::vector<char> 
    to build the string and then concatenate integers by converting them to characters. Here’s how you can do it:

        Using std::vector<char> and std::iostream:

        Example:

            #include <iostream>
            #include <vector>
            using namespace std;

            // Helper function to convert an integer to a character vector
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
                output.push_back(' '); // Add a space after the number
            }

            int main() {
                vector<char> output;

                // Adding integers to the output string
                addIntToOutput(output, 42);
                addIntToOutput(output, 123);
                addIntToOutput(output, -456);
                addIntToOutput(output, 0);

                // Convert vector<char> to string and print
                string result(output.begin(), output.end());
                cout << result << endl;

                return 0;
            }
        
        Explanation:

        intToCharVector Function:

            Converts an integer to a sequence of characters and appends them to the output vector.

            Handles negative numbers and zero appropriately.

            Uses a temporary vector to store the digits in reverse order, then appends them in the correct order.

        addIntToOutput Function:

            Calls intToCharVector to convert the integer to characters.

            Appends a space after the number to separate it from subsequent numbers.

        Main Program:

            Uses addIntToOutput to add integers to the output vector.

            Converts the output vector to a string and prints it.

    This approach avoids using additional libraries while achieving the desired functionality of concatenating 
    integers into a string-like representation.

    Feel free to test this out and modify it as needed. Let me know if you have any more questions or need further 
    adjustments!

_________________________________________________________________________________________________________________

Me: 
    how do i add new lines and spaces to the vector

__________________________________________________________________________________________________________________

Copilot: 
    Adding words, new lines (returns), and spaces to a vector<char> can be done similarly to how we add integers. 
    Let's go through it step by step:

    Adding Words:
        You can simply iterate over each character in the word and push it into the vector<char>.
        Adding Returns (\n for new lines):

        You can directly push the newline character ('\n') into the vector<char>.

    Adding Spaces:
        You can directly push the space character (' ') into the vector<char>.

    Example Code:
        Here's how you can incorporate words, new lines, and spaces into the vector<char>:

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

__________________________________________________________________________________________________________________

Me: 
    can you add another method to add another vector<char> to the end

__________________________________________________________________________________________________________________

Copilot:
    Sure! You can add a method to concatenate another vector<char> to the end of your existing output vector. 
    This is quite straightforward. I'll show you how to add such a method to your MTree class.

        // New helper function to append another vector<char> to the output 
        void appendVectorToOutput(vector<char>& output, const vector<char>& toAppend) {
            output.insert(output.end(), toAppend.begin(), toAppend.end());
        }

__________________________________________________________________________________________________________________

Thats pretty much the extent of the LLM, so now onto part 3.
__________________________________________________________________________________________________________________


Part 3: Debugging and testing plan

Specific tests: 
    Pretty much all of my testing was done with debug lines along with comparing the output my code gave with the
    output provided to us. I would simply go sift through the output my code created and compare it for each 
    command along with certain sequences of commands with the provided output code. This is kind of a primitive 
    method of comparison, but it got the job done. I also created a lot, like a lot, of debug lines i used these
    lines along with my knowledge of trees and specifically what I knew about 2,3 trees to figure out if the code
    was working as intended. 
Issues and Resolutions: 
    The first major issue I encountered was how to output the code without the string library. Last project I used
    this library to add statements to a string, this was useful as ai could simply add words, spaces, integers,
    and new lines to the string, so it could all be done in line then simply printed out. But for this project the
    string library was not allowed so I had to find another way to create the output statement. With the 
    assistance of a large language model I found I could add all the necessary elements to a vector in the form of
    characters, then output the vector by simply looping through it. This is definitely a very complicated 
    solution and probably made the program marginally slower, but its a solid solution.
    
    
    
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */