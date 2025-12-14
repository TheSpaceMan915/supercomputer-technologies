#ifndef ASSIGNMENT5_DIST_H
#define ASSIGNMENT5_DIST_H

namespace a5 {

void row_block_partition(int N, int P, int rank, int& offset, int& count);
int owner_of_row(int N, int P, int row);

} // namespace a5

#endif
