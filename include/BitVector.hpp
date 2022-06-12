#ifndef __BIT_VECTOR__
#define __BIT_VECTOR__

#include <vector>

class bit_vector{
public:
    bit_vector(uint64_t l);
    bit_vector(uint64_t l, bool set);
    bit_vector(uint64_t l, uint64_t bs);
    bit_vector() { }
    ~bit_vector();

    void set_index(uint64_t index);
    void reset_index(uint64_t index);

    void set_value(uint64_t index, uint64_t value);
    uint64_t get_value(uint64_t index);

    bool read_bit(uint64_t index);
    void show();
    void show_range(uint64_t from, uint64_t to);

    uint64_t get_length() { return length; }
    uint64_t get_bsize() { return bsize; }
    uint64_t get_size_in_bits() { return arr.size()*64; }

    std::vector<uint64_t> get_arr() { return arr; }
    void set_arr(uint64_t i, uint64_t arr_item) { arr[i] = arr_item; }
private:
    uint64_t length;
    uint8_t bsize;
    std::vector<uint64_t> arr;
};

#endif
