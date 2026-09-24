// reinterpreted castos olvasás
// fix chunkokra mutató uint8 pointerek
#include <vector>
#include <cstdint>
#include <stdexcept>
class MemoryPool
{

    std::vector<uint8_t *> chunks;
    // 64Kb L1/l2 cache barát
    static constexpr size_t CHUNK_SIZE = 65536;
    size_t currentSize = 0;

public:
    MemoryPool()
    {
        // Allocate the first chunk
        makeChunk();
    }

    ~MemoryPool()
    {
        for (auto chunk : chunks)
        {
            delete[] chunk;
        }
    }

    int allocate(size_t length_in_bits) 
    {
        if(bit_to_byte(length_in_bits) >= CHUNK_SIZE) throw std::invalid_argument("Requested allocation size exceeds chunk size.");
        size_t bytes = bit_to_byte(length_in_bits);
        currentSize += bytes;
        if (currentSize / CHUNK_SIZE > chunks.size()-1)
        {
            makeChunk();
            currentSize = (currentSize / CHUNK_SIZE)*CHUNK_SIZE + bytes;
        }
            return currentSize-bytes;
        
    }


    
 template <typename T>
    T& getAccess(size_t byte_offset) {
        uint8_t& byte_ref = chunks[byte_offset / CHUNK_SIZE][byte_offset % CHUNK_SIZE];
        return reinterpret_cast<T&>(byte_ref);
    }


    // segéd fügvények
    void makeChunk()
    {
        chunks.push_back(new uint8_t[CHUNK_SIZE]);
    }

    int bit_to_byte(size_t bit)
    {
    if (bit <= 8)  return 1;
    if (bit <= 16) return 2;
    if (bit <= 32) return 4;
    if (bit <= 64) return 8;
    
    // 64 felett pedig 64 bites szavak többszöröse:
    return ((bit + 63) / 64) * 8;
    }
    

};