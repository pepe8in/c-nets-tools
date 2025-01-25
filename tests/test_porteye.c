#include "../include/porteye.h"

void test_checkIp() {
    assert(checkIp("192.168.0.1") == 0);
    assert(checkIp("256.256.256.256") == -1); 
    assert(checkIp("123.45.67.89") == 0);
    assert(checkIp("0.0.0.0") == 0);
    printf("test_checkIp() passe\n");
}

void test_checkPort() {
    assert(checkPort(80, 80) == 0);
    assert(checkPort(0, 1024) == 0);
    assert(checkPort(1024, 0) == 0); 
    assert(checkPort(-1, 1024) == -1); 
    assert(checkPort(70000, 1024) == -1); 
    printf("test_checkPort() passe\n");
}

void test_scanPort() {
    assert(scanPort("127.0.0.1", 80) == 0); 
    assert(scanPort("127.0.0.1", 3306) == 0);
    assert(scanPort("127.0.0.1", 9999) == 1); 
    printf("test_scanPort() passe\n");
}

void test_scanRange() {
    assert(scanRange("127.0.0.1", 80, 81) == 0); 
    assert(scanRange("127.0.0.1", 1024, 1025) == 0); 
    printf("test_scanRange() passe\n");
}

void test_scanOpenPort() {
    assert(scanOpenPort("127.0.0.1", 80, 443) == 0);  
    printf("test_scanOpenPort() passe\n\n");
}
