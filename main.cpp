/**
 * 描述：跳表使用样例
 * 作者：guchenfeng
 * 时间：2023/2/4
 * 
*/
#include <iostream>
#include "my_skiplist.hpp"


int main() {

    //测试开始
    //文档加载，目前只支持 <string, string>
    // SkipList<std::string, std::string> skip_list(6);
    // skip_list.load();
    // skip_list.display();

    std::cout << "main: start testing skiplist!" << std::endl;
    
    SkipList<int, std::string> skip_list(6);
    skip_list.insert(1, "welcome");
    skip_list.insert(2, "to");
    skip_list.insert(3, "guchenfeng's");
    skip_list.insert(4, "skiplist");
    skip_list.insert(5, "demo");
    skip_list.insert(6, "this");
    skip_list.insert(7, "is");
    skip_list.insert(8, "skiplist");

    std::cout << "skiplist size: " << skip_list.size() << std::endl;
    skip_list.display();
    
    //插入重复key
    skip_list.insert(1, "replication");
    skip_list.remove(1);
    skip_list.insert(1, "new welcome");

    skip_list.display();
    skip_list.update(1, "welcome");
    skip_list.display();
    skip_list.dump(); //落盘

    //测试结束
    std::cout << "main: end testing skiplist!" << std::endl;

    return 0;
}