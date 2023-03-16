/**
 * 描述：测试性能
 * 作者：guchenfeng
 * 时间：2023/2/7
 * 
*/

#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include "../my_skiplist.hpp"

#define NUM_THREADS 1
#define TEST_COUNT 100000
SkipList<int, std::string> skipList(18);

void *insertTest(void* threadid) {
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.insert(rand() % TEST_COUNT, "a"); 
	}
    pthread_exit(NULL);
}

void *getTest(void* threadid) {
    long tid; 
    tid = (long)threadid;
    std::cout << tid << std::endl;  
    int tmp = TEST_COUNT/NUM_THREADS; 
	for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
		skipList.find(rand() % TEST_COUNT); 
	}
    pthread_exit(NULL);
}

int main() {
    srand (time(NULL));  
    //测试写性能
    {

        pthread_t threads[NUM_THREADS];
        int rc;
        int i;

        auto start = std::chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, insertTest, (void *)i);

            if (rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {
                perror("pthread_create() error"); 
                exit(3);
            }
        }
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    }
    skipList.display();

    //测试读性能
    // {
    //     pthread_t threads[NUM_THREADS];
    //     int rc;
    //     int i;
    //     auto start = std::chrono::high_resolution_clock::now();

    //     for( i = 0; i < NUM_THREADS; i++ ) {
    //         std::cout << "main() : creating thread, " << i << std::endl;
    //         rc = pthread_create(&threads[i], NULL, getTest, (void *)i);

    //         if (rc) {
    //             std::cout << "Error:unable to create thread," << rc << std::endl;
    //             exit(-1);
    //         }
    //     }

    //     void *ret;
    //     for( i = 0; i < NUM_THREADS; i++ ) {
    //         if (pthread_join(threads[i], &ret) !=0 )  {
    //             perror("pthread_create() error"); 
    //             exit(3);
    //         }
    //     }

    //     auto finish = std::chrono::high_resolution_clock::now(); 
    //     std::chrono::duration<double> elapsed = finish - start;
    //     std::cout << "get elapsed:" << elapsed.count() << std::endl;
    // }

	pthread_exit(NULL);
    return 0;

}
