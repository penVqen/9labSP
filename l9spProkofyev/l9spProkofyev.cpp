#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

struct Cheese {
    char brand[50];
    char manufacturer[50];
    float fat_content;
    float price;
};

class BinaryTree {
private:
    struct Node {
        Cheese cheese;
        Node* left;
        Node* right;
    };

    Node* root;

    void deleteNode(Node*& node, const float price, const float fat_content);
    void printInOrder(Node* node, Cheese*& arr, int& index) const;
    bool search(Node* node, const float price, const float fat_content) const;
    void insert(Node*& node, const Cheese& cheese);
    void destroyTree(Node*& node);
    void saveToFile(Node* node, ofstream& file) const;
    void loadFromFile(Node*& node, ifstream& file);

public:
    BinaryTree() : root(nullptr) {}
    ~BinaryTree() { destroyTree(root); }

    void insert(const Cheese& cheese) { insert(root, cheese); }
    void remove(const float price, const float fat_content) { deleteNode(root, price, fat_content); }
    bool search(const float price, const float fat_content) const { return search(root, price, fat_content); }
    Cheese* toArray(int& size);
    void saveToFile(const char* filename) const;
    void loadFromFile(const char* filename);
    int countNodes(Node* node) const;
};

void BinaryTree::insert(Node*& node, const Cheese& cheese) {
    if (node == nullptr) {
        node = new Node;
        node->cheese = cheese;
        node->left = node->right = nullptr;
    }
    else if (cheese.price + cheese.fat_content < node->cheese.price + node->cheese.fat_content) {
        insert(node->left, cheese);
    }
    else if (cheese.price + cheese.fat_content > node->cheese.price + node->cheese.fat_content) {
        insert(node->right, cheese);
    }
    else {
        cout << "Сыр уже существует.\n";
    }
}

void BinaryTree::deleteNode(Node*& node, const float price, const float fat_content) {
    if (node == nullptr) {
        return;
    }

    if (price + fat_content < node->cheese.price + node->cheese.fat_content) {
        deleteNode(node->left, price, fat_content);
    }
    else if (price + fat_content > node->cheese.price + node->cheese.fat_content) {
        deleteNode(node->right, price, fat_content);
    }
    else {
        Node* temp = node;
        if (node->left == nullptr) {
            node = node->right;
        }
        else if (node->right == nullptr) {
            node = node->left;
        }
        else {
            Node* minNode = node->right;
            while (minNode->left != nullptr) {
                minNode = minNode->left;
            }
            node->cheese = minNode->cheese;
            deleteNode(node->right, minNode->cheese.price, minNode->cheese.fat_content);
        }
        delete temp;
    }
}

bool BinaryTree::search(Node* node, const float price, const float fat_content) const {
    if (node == nullptr) {
        return false;
    }

    if (price + fat_content < node->cheese.price + node->cheese.fat_content) {
        return search(node->left, price, fat_content);
    }
    else if (price + fat_content > node->cheese.price + node->cheese.fat_content) {
        return search(node->right, price, fat_content);
    }
    else {
        return true;
    }
}

void BinaryTree::printInOrder(Node* node, Cheese*& arr, int& index) const {
    if (node != nullptr) {
        printInOrder(node->left, arr, index);
        arr[index++] = node->cheese;
        printInOrder(node->right, arr, index);
    }
}

Cheese* BinaryTree::toArray(int& size) {
    if (root == nullptr) {
        size = 0;
        return nullptr;
    }

    int count = 0;
    Cheese* arr = new Cheese[countNodes(root)];
    printInOrder(root, arr, count);
    size = count;
    return arr;
}

void BinaryTree::destroyTree(Node*& node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
        node = nullptr;
    }
}

void BinaryTree::saveToFile(Node* node, ofstream& file) const {
    if (node != nullptr) {
        saveToFile(node->left, file);
        file << node->cheese.brand << "\n";
        file << node->cheese.manufacturer << "\n";
        file << node->cheese.fat_content << "\n";
        file << node->cheese.price << "\n";
        saveToFile(node->right, file);
    }
}

void BinaryTree::loadFromFile(Node*& node, ifstream& file) {
    if (!file.eof()) {
        Cheese cheese;
        file.getline(cheese.brand, 50);
        file.getline(cheese.manufacturer, 50);
        file >> cheese.fat_content;
        file >> cheese.price;
        insert(node, cheese);
        loadFromFile(node, file);
    }
}

void BinaryTree::saveToFile(const char* filename) const {
    ofstream file(filename, ios::out);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для записи.\n";
        return;
    }

    saveToFile(root, file);
    file.close();
}

void BinaryTree::loadFromFile(const char* filename) {
    ifstream file(filename, ios::in);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла для чтения.\n";
        return;
    }

    destroyTree(root);
    root = nullptr;
    loadFromFile(root, file);
    file.close();
}

int BinaryTree::countNodes(Node* node) const {
    if (node == nullptr) {
        return 0;
    }
    return 1 + countNodes(node->left) + countNodes(node->right);
}

int main() {
    setlocale(LC_ALL, "Russian");
    BinaryTree tree;
    Cheese cheese;
    char filename[100];
    int choice;

    while (true) {
        cout << "\n1. Добавить сыр\n2. Удалить сыр\n3. Найти сыр\n4. Получить все сыры\n5. Сохранить в файл\n6. Загрузить из файла\n7. Выйти\n";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Введите название бренда: ";
            cin.ignore();
            cin.getline(cheese.brand, 50);
            cout << "Введите производителя: ";
            cin.getline(cheese.manufacturer, 50);
            cout << "Введите содержание жира: ";
            cin >> cheese.fat_content;
            cout << "Введите цену: ";
            cin >> cheese.price;
            tree.insert(cheese);
            break;
        case 2:
            cout << "Введите цену: ";
            float price;
            cin >> price;
            cout << "Введите содержание жира: ";
            float fat_content;
            cin >> fat_content;
            tree.remove(price, fat_content);
            break;
        case 3:
            cout << "Введите цену: ";
            cin >> price;
            cout << "Введите содержание жира: ";
            cin >> fat_content;
            if (tree.search(price, fat_content))
                cout << "Сыр найден.\n";
            else
                cout << "Сыр не найден.\n";
            break;
        case 4:
        {
            int size;
            Cheese* arr = tree.toArray(size);
            for (int i = 0; i < size; ++i) {
                cout << "\nБренд: " << arr[i].brand
                    << "\nПроизводитель: " << arr[i].manufacturer
                    << "\nСодержание жира: " << arr[i].fat_content
                    << "\nЦена: " << arr[i].price << "\n";
            }
            delete[] arr;
            break;
        }
        case 5:
            cout << "Введите имя файла: ";
            cin >> filename;
            tree.saveToFile(filename);
            break;
        case 6:
            cout << "Введите имя файла: ";
            cin >> filename;
            tree.loadFromFile(filename);
            break;
        case 7:
            return 0;
        default:
            cout << "Неверный выбор.\n";
        }
    }

    return 0;
}
