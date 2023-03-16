/**
 * 描述：跳表实现头文件
 * 作者：guchenfeng
 * 时间：2023/2/2
 * 
*/
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <mutex>

std::string STORE_FILE = "./dump.txt";
std::mutex locker; //互斥锁
std::string delimiter = ":"; //数据落盘的格式 K:V

/*
 * 跳表节点类
*/

template<typename K, typename V>
class Node {

public:

    //层级指针数组
    Node** forward;

    //层高
    int level;

    /*构造函数*/
    Node(K t_key, V t_val, int t_level);

    /*析构函数*/
    ~Node();

    /*获取key*/
    K getKey();

    /*获取value*/
    V getVal();

    /*更新value*/
    bool setVal(V t_val);

private:

    //节点的key
    K key;

    //节点的value
    V val;
    

};

/*构造函数*/
template<typename K, typename V>
Node<K, V>::Node(K t_key, V t_val, int t_level) : key(t_key), 
        val(t_val), level(t_level) {

    //初始化层级指针数组
    forward = new Node<K, V>*[level + 1];
    memset(forward, 0, sizeof(Node<K, V>*) * (level + 1));
}

/*析构函数*/
template<typename K, typename V>
Node<K, V>::~Node() {
    //回收层级指针数组的内存
    if (forward) {
        delete[] forward;
    }
}

/*获取key*/
template<typename K, typename V>
K Node<K, V>::getKey() {
    return key;
}

/*获取value*/
template<typename K, typename V>
V Node<K, V>::getVal() {
    return val;
}

/*更新节点value*/
template<typename K, typename V>
bool Node<K, V>::setVal(V t_val) {
    val = t_val;
    return true;
}

/*
 * 跳表类
 */

template<typename K, typename V>
class SkipList {
public:

    /*构造函数*/
    SkipList(int t_max_height);

    /*析构函数*/
    ~SkipList();

    /*查询操作*/
    Node<K, V>* find(K key);

    /*插入操作*/
    bool insert(K key, V val);

    /*删除操作*/
    bool remove(K key);

    /*更新操作*/
    bool update(K key, V val);

    /*数据落盘*/
    bool dump();

    /*盘加载数据*/
    bool load();

    /*获取随机层高*/
    int getRandomHeight();

    /*获取节点数*/
    int size();

    /*打印所有节点*/
    void display();

    /*行字符串转换成KV*/
    bool getKVFromString(std::string& line, std::string& key, std::string& val);

    /*判断行是否合法*/
    bool isValid(std::string& line);


private:

    //头结点
    Node<K, V>* head;

    //最大层级高度
    int max_height;

    //当前层级高度
    int level_height;

    //跳表节点数
    int length;

    //文件输出流
    std::ofstream out_file;

    //文件输入流
    std::ifstream in_file;

};

/*构造函数*/
template<typename K, typename V>
SkipList<K, V>::SkipList(int t_max_height) : max_height(t_max_height), 
        level_height(0), length(0) {

    //初始化头节点
    K k;
    V v;
    head = new Node<K, V>(k, v, max_height);
    

}

/*析构函数*/
template<typename K, typename V>
SkipList<K, V>::~SkipList() {
    //回收节点内存
    Node<K, V>* cur = head->forward[0];
    Node<K, V>* pre = NULL;
    while (cur) {
        pre = cur->forward[0];
        delete cur;
        cur = pre;
    }
    //最后释放头节点
    delete head;

    //关闭读写流
    if (in_file.is_open()) {
        in_file.close();
    }

    if (out_file.is_open()) {
        out_file.close();
    }
}

/*查询操作*/
template<typename K, typename V>
Node<K, V>* SkipList<K, V>::find(K key) {
    Node<K, V>* cur = head;

    //遍历每一层，直至0层，退出循环时，cur指向key插入位置的前一个位置
    for (int i = level_height; i >= 0; i -- ) {
        
        while (cur->forward[i] && cur->forward[i]->getKey() < key) {
            cur = cur->forward[i];
        }
    }
    //获取key的插入位置
    cur = cur->forward[0];

    //若key插入位置存在key，则表示存在
    if (cur != NULL && cur->getKey() == key) {
        std::cout << "SkipList: Searching key succeeded!" << std::endl;
        return cur;
    }

    //否则不存在
    std::cout << "SkipList: key does not exist!" << std::endl;
    return NULL;
}


/*插入操作*/
template<typename K, typename V>
bool SkipList<K, V>::insert(K key, V val) {
    
    //上互斥锁
    locker.lock();
    //生成预插入节点高度
    int height = getRandomHeight();

    //为插入节点选择合适位置
    Node<K, V>* cur = head;
    Node<K, V>* update[height + 1]; //存储该节点在某一层插入位置的前一个位置
    memset(update, 0, sizeof(Node<K, V>*) * (height + 1));

    //生成节点
    Node<K, V>* node = new Node<K, V>(key, val, height);
    
    //最后处理在跳表层高一下的插入位置
    for (int i = level_height; i >= 0; i -- ) {
        while (cur->forward[i] && cur->forward[i]->getKey() < key) {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }

    //判断key是否存在，存在则不插入
    cur = cur->forward[0];
    if (cur && cur->getKey() == key) {
        std::cout << "SkipList: insert fail! Key exists!" << std::endl;
        delete node;
        //解除锁
        locker.unlock();
        return false;
    }
    //插入处理
    //先处理节点层高高于当前跳表层高的插入位置
    for (int i = height; i > level_height; i -- ) {
        update[i] = head;
    }

    //更新节点位置
    for (int i = 0; i <= height; i ++ ) {
        node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = node;
    }
    
    level_height = std::max(level_height, height); //更新高度
    length ++ ; //节点数增加

    std::cout << "SkipList: insert succeeded!" << std::endl;

    //解除互斥锁
    locker.unlock();
    return true;

}

/*删除操作*/
template<typename K, typename V>
bool SkipList<K, V>::remove(K key) {
    //上锁操作
    locker.lock();
    //获取key在0层的前一节点
    Node<K, V>* cur = head;
    Node<K, V>* update[level_height + 1];
    memset(update, 0, sizeof(Node<K, V>*) * (level_height + 1));
    for (int i = level_height; i >= 0; i -- ) {
        while (cur->forward[i] && cur->forward[i]->getKey() < key) {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }

    //若key不存在，则无法删除
    cur = cur->forward[0];
    if (cur == NULL || cur->getKey() != key) {
        std::cout << "SkipList: remove fail! Key does not exist!" << std::endl;
        locker.unlock();
        return false;
    }

    //key存在，删除key
    for (int i = 0; i <= level_height; i ++ ) {
        //已删除完毕
        if (update[i]->forward[i] != cur) {
            break;
        }
        
        update[i]->forward[i] = cur->forward[i];
    }
    length -- ; //节点数减少

    //删除空层高
    while (level_height > 0 && head->forward[level_height] == NULL) {
        level_height -- ;
    }
    delete cur;

    //删除成功
    std::cout << "SkipList: delete key succeeded!" << std::endl;
    //解锁
    locker.unlock(); 
    return true;
}

/*更新操作*/
template<typename K, typename V>
bool SkipList<K, V>::update(K key, V val) {
    Node<K, V>* ret = find(key);
    //key不存在
    if (!ret) {
        std::cout << "SkipList: Update failed! Key does not exist!" << std::endl;
        return false;
    }

    //key存在则更新
    locker.lock();
    ret->setVal(val);
    locker.unlock();

    std::cout << "SkipList: Update failed! Key does not exist!" << std::endl;
    return true;
}

/*数据落盘*/
template<typename K, typename V>
bool SkipList<K, V>::dump() {
    out_file.open(STORE_FILE);
    //文件打开失败
    if (!out_file.is_open()) {
        std::cout << "SkipList: Cannot dump!" << std::endl;
        return false;
    }
    
    //数据写入写缓冲
    Node<K, V>* cur = head->forward[0];
    while (cur) {
        out_file << cur->getKey() << ":" << cur->getVal() << "\n";
        cur = cur->forward[0];
    }

    //文件刷缓冲
    out_file.flush();
    //关闭文件
    out_file.close();

    std::cout << "SkipList: Dump succeeded!" << std::endl;
    return true;
}

/*盘加载数据*/
template<typename K, typename V>
bool SkipList<K, V>::load() {
    in_file.open(STORE_FILE);
    //文件打开失败
    if (!in_file.is_open()) {
        std::cout << "SkipList: Cannot read file!" << std::endl;
        return false;
    }
    
    //数据读入
    std::string line = "";
    std::string key = "";
    std::string val = "";

    while (getline(in_file, line)) {
        
        getKVFromString(line, key, val);
        if (key.empty() || val.empty()) {
            continue;
        }
        std::cout << "load 1 data: " << key << ":" << val << std::endl;
        insert(key, val);
    }

    //关闭文件
    in_file.close();

    std::cout << "SkipList: Load succeeded!" << std::endl;
    return true;
}

/*获取随机层高*/
template<typename K, typename V>
int SkipList<K, V>::getRandomHeight() {
    //层高最少到0
    int k = 1;
    
    //若随机数为1则增加层高
    while (rand() % 2) {
        ++ k;
    }
    return std::min(k, max_height);
}

/*获取节点数*/
template<typename K, typename V>
int SkipList<K, V>::size() {

    return length;
}

/*打印所有节点*/
template<typename K, typename V>
void SkipList<K, V>::display() {
    //打印每一层
    for (int i = level_height; i >= 0; i -- ) {
        std::cout << "level " << i << ": ";
        Node<K, V>* cur = head->forward[i];
        while (cur) {
            std::cout << cur->getKey() << ":" << cur->getVal() << " ";
            cur = cur->forward[i];
        }
        std::cout << std::endl;
    }
}

/*行字符串转换成KV*/
template<typename K, typename V>
bool SkipList<K, V>::getKVFromString(std::string& line, std::string& key, std::string& val) {

    if (!isValid(line)) {
        return false;
    }

    key = line.substr(0, line.find(delimiter));
    val = line.substr(line.find(delimiter) + 1);
    return true;
}


/*判断行是否合法*/
template<typename K, typename V>
bool SkipList<K, V>::isValid(std::string& line) {

    if (line.empty() || line.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}