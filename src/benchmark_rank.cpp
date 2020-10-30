#include "RankSupport.hpp"
#include <chrono>

int main() { 
    for (int64_t i=10; i < 10000000000; i*=10) {
	auto t1 = std::chrono::high_resolution_clock::now();
        bit_vector b(i);
        rank_support r(&b);
        std::cerr<<i << "\t" << r.overhead()<<"\n";
	auto t2 = std::chrono::high_resolution_clock::now();
	for(int j = i; j > i - 10000; j--) {
	    r.rank1(std::abs(j)%i);
	}
	auto t3 = std::chrono::high_resolution_clock::now();
	auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
	auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>( t3 - t2 ).count();
	std::cerr<<"build-time: " << duration1 << "\t rank-time:" << duration2 <<"\n";
    }
    return 0; 
}
