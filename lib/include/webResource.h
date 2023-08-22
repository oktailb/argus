#pragma once
#include <cstddef>

class webResource {
public:
    webResource(const char *start, const char *end)
        :
          mData(start),
          mSize(end - start)
    {}

    const char * const &data() const { return mData; }
    const size_t &      size() const { return mSize; }

    const char *        begin() const { return mData; }
    const char *        end() const { return mData + mSize; }

private:
    const char *        mData;
    size_t              mSize;
};

#define LOAD_RESOURCE(x, to)                                     \
    extern const char _binary_##x##_start, _binary_##x##_end;    \
    to = webResource(&_binary_##x##_start, &_binary_##x##_end);  \

