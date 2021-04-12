#ifndef MAPPING_H_
#define MAPPING_H_

void *mapping_file(const char *filename, int file_size);
void mapping_cleanup(void *mapping, int file_size);

#endif /* MAPPING_H_ */
