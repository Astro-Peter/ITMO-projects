#include <lib/CCircularBuffer.h.>
#include <gtest/gtest.h>

TEST(fixed_buffer, buffer_push_back_test) {
    CircBuff::CCircularBuffer<int> buffer(10);
    buffer.push_back(0);
    ASSERT_EQ(0, *buffer.begin());
}

TEST(fixed_buffer, buffer_circular_test) {
    CircBuff::CCircularBuffer<int> buffer(5);
    for (int i = 0; i < 6; i++){
        buffer.push_back(i);
    }
    ASSERT_EQ(1, *buffer.begin());
}

TEST(fixed_buffer, buffer_equal_test) {
    CircBuff::CCircularBuffer<int> buffer(10, 5);
    CircBuff::CCircularBuffer<int> buffer2(10, 5);
    CircBuff::CCircularBuffer<int> buffer3(10, 10);
    ASSERT_TRUE(buffer == buffer2);
    ASSERT_FALSE(buffer != buffer3);
}

TEST(fixed_buffer, erase_test) {
    CircBuff::CCircularBuffer<int> buffer(5);
    for (int i = 0; i < 13; i++) {
        buffer.push_back(i);
    }
    ASSERT_EQ(12, *(buffer.end() - 1));
    buffer.erase(buffer.begin(), buffer.begin() + 3);
    ASSERT_EQ(2, buffer.size());
    ASSERT_EQ(11, *(buffer.begin()));
}

TEST(fixed_buffer, assign_test){
    CircBuff::CCircularBuffer<int> buffer(10);
    for (int i = 0; i < 10; i++) {
        buffer.push_back(i);
    }
    buffer.assign(5, 18);
    ASSERT_EQ(5, buffer.size());
    ASSERT_EQ(18, *buffer.begin());
}
TEST(iterator_test, iterator_difference_test) {
    CircBuff::CCircularBuffer<int> buffer(10, 5);
    buffer.pop();
    ASSERT_EQ(9, buffer.end() - buffer.begin());
}


TEST(iterator_test, addition_test) {
    CircBuff::CCircularBuffer<int> buffer(10);
    for (int i = 0; i < 10; i++) {
        buffer.push_back(i);
    }
    auto iter = buffer.begin();
    ASSERT_EQ(4, (iter + 4).GetPos());
}

TEST(iterator_test, difference_test) {
    CircBuff::CCircularBuffer<int> buffer(10, 10);
    auto iter = buffer.end();
    ASSERT_EQ(6, (iter - 4).GetPos());
}

TEST(iterator_test, access_test) {
    CircBuff::CCircularBuffer<int> buffer(10);
    for (int i = 0; i < 10; i++) {
        buffer.push_back(i);
    }
    ASSERT_EQ(3, buffer.begin()[3]);
}

TEST(iterator_test, comparison_test) {
    CircBuff::CCircularBuffer<int> buffer(10, 10);
    ASSERT_TRUE(buffer.begin() < buffer.end());
}


TEST(iterator_test, equality_test) {
    CircBuff::CCircularBuffer<int> buffer(10);
    ASSERT_TRUE(buffer.end() == buffer.begin());
}

TEST(extended_buffer, extention_test) {
    CircBuff::CCircularBufferExtended<int> buffer(10, 10);
    buffer.push_back(10);
    buffer.push_back(10);
    ASSERT_EQ(12, buffer.size());
}

TEST(extended_buffer, extention_correctness_test) {
    CircBuff::CCircularBufferExtended<int> buffer(10, 19);
    buffer.push_back(10);
    buffer.push_back(10);
    ASSERT_EQ(19, *(buffer.begin() + 5));
}

TEST(extended_buffer, insert_object_test) {
    CircBuff::CCircularBufferExtended<int> buffer(10);
    for (int i = 0; i < 8; i++) {
        buffer.push_back(i);
    }
    buffer.insert(buffer.begin() + 2, 12);
    //buffer.insert(buffer.begin() + 4, 26, 4);
    ASSERT_EQ(12, buffer.begin()[2]);
}

TEST(extended_buffer, insert_iterators_test) {
    CircBuff::CCircularBufferExtended<int> buffer(10);
    CircBuff::CCircularBufferExtended<int> buffer3(10, 10);
    for (int i = 0; i < 4; i++) {
        buffer.push_back(i);
    }
    buffer.insert(buffer.begin(), buffer3.begin(), buffer3.end());
    buffer.insert(buffer.begin(), 40, 4);
    ASSERT_EQ(4, buffer.begin()[2]);
    ASSERT_EQ(10, *(buffer.end() - 6));
    ASSERT_EQ(54, buffer.size());
}