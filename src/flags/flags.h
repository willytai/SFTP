#ifndef __FLAGS_H__
#define __FLAGS_H__

#define INT_FLAG_RECURSIVE 0x20000 // (1 << (int('r')-int('a')))
#define INT_FLAG_HUMAN     0x80    // (1 << (int('h')-int('a')))
#define INT_FLAG_FORCE     0x20    // (1 << (int('f')-int('a')))

void dummy();

#endif /* __FLAGS_H__ */
