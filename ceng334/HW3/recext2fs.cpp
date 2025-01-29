#include "ext2fs.h"
#include "identifier.h"
#include "ext2fs_print.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <vector>

using namespace std;

int image;
struct ext2_super_block superBlock;
unsigned int blockSize;
struct ext2_block_group_descriptor* groupDescriptors;
struct ext2_inode** inodes;
uint8_t** i_bitmap;
uint8_t** b_bitmap;

void read_inode(unsigned int inode_num, struct ext2_inode *inode);
void read_block(unsigned int block_num, void *buffer);
void print_directory(unsigned int inode_num, int depth);
void recover_inode_bitmap(unsigned int inode_num);
void traverse_dir_entry_block(struct ext2_dir_entry *dirEntry);
void set_b_bitmap(unsigned int inode_num);
void traverse_directory(unsigned int inode_num);
void set_1_bitmap(uint32_t block_num);
void traverse_pointer_blocks(unsigned int block_num, bool flag);
uint8_t* identifier;

int main(int argc, char* argv[])
{
    image = open(argv[1], O_RDWR);

    identifier = parse_identifier(argc, argv);

    lseek(image, 1024, SEEK_SET);//skip the boot block
    read(image, &superBlock, sizeof(superBlock));

    if(superBlock.magic != EXT2_SUPER_MAGIC)
    {
        printf("Not an ext2 filesystem\n");
        return 1;
    }
    
    blockSize = EXT2_UNLOG(superBlock.log_block_size);

    uint32_t numOfBlockGroups = CEIL(superBlock.block_count, superBlock.blocks_per_group);

    groupDescriptors = new struct ext2_block_group_descriptor[numOfBlockGroups];

    lseek(image, 2048, SEEK_SET);
    int a = read(image, groupDescriptors, sizeof(struct ext2_block_group_descriptor) * numOfBlockGroups);

    i_bitmap = new uint8_t*[numOfBlockGroups];
    b_bitmap = new uint8_t*[numOfBlockGroups];
    for(int i=0; i < numOfBlockGroups; i++)
    {
        int bitmapSize = (superBlock.inodes_per_group + 7) / 8;  // round up to nearest byte
        i_bitmap[i] = new uint8_t[bitmapSize];

        lseek(image, groupDescriptors[i].inode_bitmap * blockSize, SEEK_SET);
        read(image, i_bitmap[i], bitmapSize);
    }

    for(int i=0; i < numOfBlockGroups; i++)
    {
        int bitmapSize = (superBlock.blocks_per_group + 7) / 8;
        b_bitmap[i] = new uint8_t[bitmapSize];

        lseek(image, groupDescriptors[i].block_bitmap * blockSize, SEEK_SET);
        read(image, b_bitmap[i], bitmapSize);
    }

    for(int i=0; i < 11; i++)
    {
        int byteIndex = i / 8;
        int bitIndex = i % 8;
        i_bitmap[0][byteIndex] |= (1 << bitIndex); // if it is in group 0 !!!!!
    }

    for(int i=0; i < numOfBlockGroups; i++)
    {
        for(int j=0; j < superBlock.inodes_per_group; j++)
        {
            struct ext2_inode inode;
            read_inode(i * superBlock.inodes_per_group + j + 1, &inode);
            //print_inode(&inode, i * superBlock.inodes_per_group + j + 1);
            int byteIndex = j / 8;
            int bitIndex = j % 8;
            if((i_bitmap[i][byteIndex] >> bitIndex) & 1)
            {
                continue;
            }
            if(inode.mode == 0 || inode.deletion_time != 0 || inode.link_count == 0)
            {
                continue;
            }
            i_bitmap[i][byteIndex] |= (1 << bitIndex);
        }
    }
    // block bitmap repair

    for(int i=0; i < numOfBlockGroups-1; i++)
     {
        struct ext2_block_group_descriptor groupDescriptor;
        groupDescriptor = groupDescriptors[i];
        uint32_t block_number1, block_number2, *inode_blocks;
        block_number1 = groupDescriptor.inode_bitmap;
        block_number2 = groupDescriptor.block_bitmap;

        int number_of_blocks_required_for_inode_table = CEIL(superBlock.inodes_per_group * superBlock.inode_size, blockSize);
        inode_blocks = new uint32_t[number_of_blocks_required_for_inode_table+2];
        for(int j=0; j < number_of_blocks_required_for_inode_table; j++)
        {
            inode_blocks[j] = groupDescriptor.inode_table + j;
        }
        inode_blocks[number_of_blocks_required_for_inode_table] = block_number1;
        inode_blocks[number_of_blocks_required_for_inode_table+1] = block_number2;

        for(int j=0; j < number_of_blocks_required_for_inode_table+2; j++)
        {
            set_1_bitmap(inode_blocks[j]);
            //printf("group: %d, block_respect_to_group: %d,  j: %d, \n", inode_blocks[j] / superBlock.blocks_per_group, inode_blocks[j]%superBlock.blocks_per_group, j);
        }
    }
    int last_inode_table_block_for_last_group;
    //do it for the last group
    struct ext2_block_group_descriptor groupDescriptor;
    groupDescriptor = groupDescriptors[numOfBlockGroups-1];
    uint32_t block_number1, block_number2, *inode_blocks;
    block_number1 = groupDescriptor.inode_bitmap;
    block_number2 = groupDescriptor.block_bitmap;

    //printf("numOfBlockGroups: %d\n", numOfBlockGroups);
    //printf("superBlock.inode_count: %d\n", superBlock.inode_count);
    //printf("superBlock.inodes_per_group: %d\n", superBlock.inodes_per_group);
    int number_of_inodes_for_last_group = superBlock.inode_count - (numOfBlockGroups-1) * superBlock.inodes_per_group;

    int number_of_blocks_required_for_inode_table = CEIL(number_of_inodes_for_last_group * superBlock.inode_size, blockSize);
    inode_blocks = new uint32_t[number_of_blocks_required_for_inode_table+2];
    last_inode_table_block_for_last_group = groupDescriptor.inode_table + number_of_blocks_required_for_inode_table - 1;
    for(int j=0; j < number_of_blocks_required_for_inode_table; j++)
    {
        inode_blocks[j] = groupDescriptor.inode_table + j;
    }
    inode_blocks[number_of_blocks_required_for_inode_table] = block_number1;
    inode_blocks[number_of_blocks_required_for_inode_table+1] = block_number2;

    for(int j=0; j < number_of_blocks_required_for_inode_table+2; j++)
    {
        set_1_bitmap(inode_blocks[j]);
        //printf("group: %d, block_respect_to_group: %d,  j: %d, \n", inode_blocks[j] / superBlock.blocks_per_group, inode_blocks[j]%superBlock.blocks_per_group, j);
    }

    //printf("blockSize: %d\n", blockSize);
    for(int i=0; i < superBlock.block_count; i++)
    {
        int group_num = i / superBlock.blocks_per_group;
        int group_start_block = group_num * superBlock.blocks_per_group;
        int inode_table_end_for_group = groupDescriptors[group_num].inode_table + CEIL(superBlock.inodes_per_group * superBlock.inode_size, blockSize)-1;
        if(blockSize >= 2048)
        {
            lseek(image, i * blockSize, SEEK_SET);
        }
        else
        {
            group_start_block++;
            lseek(image, i * blockSize, SEEK_SET);
        }
        int group_end_block = group_start_block + superBlock.blocks_per_group - 1;
        uint8_t block[blockSize];
        read(image, block, blockSize);

        bool flag = false;
        for(int j=0; j < blockSize; j++)
        {
            if(block[j] != 0)
            {
                flag = true;
                break;
            }
        }
        if(blockSize == 1024 && i == 0)
        {
            continue;
        }
        if(i >= group_start_block && i <= inode_table_end_for_group)
        {
            set_1_bitmap(i);
        }
        //print inode_table_end_for_group and inode_table_end_for_group
        if(i > inode_table_end_for_group && i <= group_end_block)
        {
            traverse_pointer_blocks(i, false);
        }
        if(flag)
        {
            set_1_bitmap(i);
            //printf("group: %d, block_respect_to_group: %d\n", i / superBlock.blocks_per_group, i%superBlock.blocks_per_group);  // tek hata var
        }

    }

    traverse_directory(EXT2_ROOT_INODE);

    for(int i=0; i < numOfBlockGroups; i++)
    {
        for(int j=0; j < superBlock.inodes_per_group && i * superBlock.inodes_per_group + j < superBlock.inode_count; j++)
        {
            int byteIndex = j / 8;
            int bitIndex = j % 8;
            if((i_bitmap[i][byteIndex] >> bitIndex) & 1)
            {
                int inode_num = i * superBlock.inodes_per_group + j + 1;
                struct ext2_inode inode;
                read_inode(inode_num, &inode);
                traverse_directory(inode_num);
            }
        }
    }

    int bitmapSize = (superBlock.inodes_per_group + 7) / 8;
    for(int i=0; i < numOfBlockGroups; i++)
    {
        lseek(image, groupDescriptors[i].inode_bitmap * blockSize, SEEK_SET);
        write(image, i_bitmap[i], bitmapSize);
    }

    for(int i=0; i < numOfBlockGroups; i++)
    {
        bitmapSize = (superBlock.blocks_per_group + 7) / 8;
        lseek(image, groupDescriptors[i].block_bitmap * blockSize, SEEK_SET);
        write(image, b_bitmap[i], bitmapSize);
    }

    printf("- root/\n");
    print_directory(EXT2_ROOT_INODE, 2);



        //printing inode bitmap
    /*for(int i=0; i < numOfBlockGroups; i++)
    {
        for(int j=0; j < superBlock.inodes_per_group; j++)
        {
            int byteIndex = j / 8;
            int bitIndex = j % 8;
            printf("%d", (i_bitmap[i][byteIndex] >> bitIndex) & 1);
        }
        printf("\n");
    }*/

    close(image);
    return 0;

}

void traverse_pointer_blocks(unsigned int block_num, bool flag)
{
    uint8_t *block = new uint8_t[blockSize / sizeof(uint8_t)];
    read_block(block_num, block);
    bool isFile = true;
    bool isDirectory = false;
    bool isBlockEmpty = true;
    bool isPointerBlock = false;
    for(int i=0; i < 32; i++)
    {
        if(block[i] != identifier[i])
        {
            isFile = false;
            break;
        }
    }

    struct ext2_dir_entry *dirEntry = (struct ext2_dir_entry *)block;
    int size = 0;
    for(int i=0; size < blockSize; i++)
    {
        if(dirEntry->length == 0)
        {
            break;
        }
        size += dirEntry->length;
        dirEntry = (struct ext2_dir_entry *)((char *)dirEntry + dirEntry->length);
        if(size == blockSize)
        {
            isDirectory = true;
        }
    }

    for(int i=0; i < blockSize; i++)
    {
        if(block[i] != 0)
        {
            isBlockEmpty = false;
            break;
        }
    }

    if(!isBlockEmpty && !isFile && !isDirectory)
    {
        isPointerBlock = true;
    }

    uint32_t* pointerBlock = (uint32_t *)block;
    for(int i=0; i < blockSize / sizeof(uint32_t); i++)
    {
        if(pointerBlock[i] > superBlock.block_count)
        {
            isPointerBlock = false;
            break;
        }
    }

    if(!isFile && !isDirectory && !isBlockEmpty && isPointerBlock)  //pointer block
    {
        for(int i=0; i < blockSize / sizeof(uint32_t); i++)
        {
            if(pointerBlock[i] == 0)
            {
                continue;
            }
            traverse_pointer_blocks(pointerBlock[i], true);
        }
    }
    //printf("isFile: %d, isDirectory: %d, isBlockEmpty: %d, isPointerBlock: %d\n", isFile, isDirectory, isBlockEmpty, isPointerBlock);

    if(isFile || isDirectory || isPointerBlock || flag )
    {
        //printf("group: %d, block_respect_to_group: %d\n", block_num / superBlock.blocks_per_group, block_num%superBlock.blocks_per_group);
        set_1_bitmap(block_num);
    }
    
}

void traverse_dir_entry_block(struct ext2_dir_entry *dirEntry)
{
    unsigned int size = 0;
        while(size < blockSize && dirEntry->length != 0)
        {
            if(dirEntry->inode == 0 || dirEntry->name_length == 0)
            {
                size += dirEntry->length;
                dirEntry = (struct ext2_dir_entry *)((char *)dirEntry + dirEntry->length);
                continue;
            }

            char name[dirEntry->name_length + 1];
            memcpy(name, dirEntry->name, dirEntry->name_length);
            name[dirEntry->name_length] = '\0';

            if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            {
                size += dirEntry->length;// !!!!! size ekle
                dirEntry = (struct ext2_dir_entry *)((char *)dirEntry + dirEntry->length);
                continue;
            }

            if(dirEntry->file_type == EXT2_D_DTYPE)
            {
                traverse_directory(dirEntry->inode);
            }
            else
            {
                set_b_bitmap(dirEntry->inode);
            }

            size += dirEntry->length;
            dirEntry = (struct ext2_dir_entry *)((char *)dirEntry + dirEntry->length);
        }
}

void set_b_bitmap(unsigned int inode_num)
{
    struct ext2_inode inode;
    read_inode(inode_num, &inode);
    for(int i=0; i < EXT2_NUM_DIRECT_BLOCKS; i++)
    {
        if(inode.direct_blocks[i] == 0)
        {
            continue;
        }
        set_1_bitmap(inode.direct_blocks[i]);
        //printf("raw_block: %d, group: %d, block_respect_to_group: %d\n", inode.direct_blocks[i], inode.direct_blocks[i] / superBlock.blocks_per_group, inode.direct_blocks[i]%superBlock.blocks_per_group);
    }

    if(inode.single_indirect != 0)
    {
        unsigned int *block = new unsigned int[blockSize / sizeof(unsigned int)];
        read_block(inode.single_indirect, block);
        for(int i=0; i < blockSize / sizeof(unsigned int); i++)
        {
            if(block[i] == 0)
            {
                continue;
            }
            set_1_bitmap(block[i]);
            //printf("raw_block: %d, group: %d, block_respect_to_group: %d\n", block[i], block[i] / superBlock.blocks_per_group, block[i]%superBlock.blocks_per_group);
        }
    }

    if(inode.double_indirect != 0)
    {
        unsigned int *block = new unsigned int[blockSize / sizeof(unsigned int)];
        read_block(inode.double_indirect, block);
        for(int i=0; i < blockSize / sizeof(unsigned int); i++)
        {
            if(block[i] == 0)
            {
                continue;
            }
            unsigned int *block2 = new unsigned int[blockSize / sizeof(unsigned int)];
            read_block(block[i], block2);
            for(int j=0; j < blockSize / sizeof(unsigned int); j++)
            {
                if(block2[j] == 0)
                {
                    continue;
                }
                set_1_bitmap(block2[j]);
                //printf("raw_block: %d, group: %d, block_respect_to_group: %d\n", block2[j], block2[j] / superBlock.blocks_per_group, block2[j]%superBlock.blocks_per_group);
            }
        }
    }

    if(inode.triple_indirect != 0)
    {
        unsigned int *block = new unsigned int[blockSize / sizeof(unsigned int)];
        read_block(inode.triple_indirect, block);
        for(int i=0; i < blockSize / sizeof(unsigned int); i++)
        {
            if(block[i] == 0)
            {
                continue;
            }
            unsigned int *block2 = new unsigned int[blockSize / sizeof(unsigned int)];
            read_block(block[i], block2);
            for(int j=0; j < blockSize / sizeof(unsigned int); j++)
            {
                if(block2[j] == 0)
                {
                    continue;
                }
                unsigned int *block3 = new unsigned int[blockSize / sizeof(unsigned int)];
                read_block(block2[j], block3);
                for(int k=0; k < blockSize / sizeof(unsigned int); k++)
                {
                    if(block3[k] == 0)
                    {
                        continue;
                    }
                    set_1_bitmap(block3[k]);
                    //printf("raw_block: %d, group: %d, block_respect_to_group: %d\n", block3[k], block3[k] / superBlock.blocks_per_group, block3[k]%superBlock.blocks_per_group);
                }
            }
        }
    }
}



void traverse_directory(unsigned int inode_num)
{
    struct ext2_inode inode;
    read_inode(inode_num, &inode);
    if (!(inode.mode & EXT2_I_DTYPE)) 
    {
        return; // Not a directory
    }
    char block[blockSize];
    struct ext2_dir_entry *dirEntry;
    
    for(int i=0; i < EXT2_NUM_DIRECT_BLOCKS; i++)
    {
        if(inode.direct_blocks[i] == 0)
        {
            continue;
        }
        set_1_bitmap(inode.direct_blocks[i]);

        read_block(inode.direct_blocks[i], block);
        dirEntry = (struct ext2_dir_entry *)block;
        traverse_dir_entry_block(dirEntry);   
    }

    if(inode.single_indirect != 0)
    {
        //printf("inode.single_indirect: %d\n", inode.single_indirect);
        set_1_bitmap(inode.single_indirect);

        unsigned int *block = new unsigned int[blockSize / sizeof(unsigned int)];
        read_block(inode.single_indirect, block);

        for(int i=0; i < blockSize / sizeof(unsigned int); i++)
        {
            if(block[i] == 0)
            {
                continue;
            }
            set_1_bitmap(block[i]);
            
            unsigned int *tempBlock = new unsigned int[blockSize / sizeof(unsigned int)];
            read_block(block[i], tempBlock);
            dirEntry = (struct ext2_dir_entry *)tempBlock;
            traverse_dir_entry_block(dirEntry);
        }
    }
    if(inode.double_indirect != 0)
    {
        set_1_bitmap(inode.double_indirect);
        
        unsigned int *block = new unsigned int[blockSize / sizeof(unsigned int)];
        read_block(inode.double_indirect, block);
        for(int i=0; i < blockSize / sizeof(unsigned int); i++)
        {
            if(block[i] == 0)
            {
                continue;
            }
            set_1_bitmap(block[i]);

            unsigned int *block2 = new unsigned int[blockSize / sizeof(unsigned int)];
            read_block(block[i], block2);
            for(int j=0; j < blockSize / sizeof(unsigned int); j++)
            {
                if(block2[j] == 0)
                {
                    continue;
                }
                set_1_bitmap(block2[j]);

                unsigned int *tempBlock = new unsigned int[blockSize / sizeof(unsigned int)];
                read_block(block2[j], tempBlock);
                dirEntry = (struct ext2_dir_entry *)tempBlock;
                traverse_dir_entry_block(dirEntry);
            }
        }
    }
    if(inode.triple_indirect != 0)
    {
        set_1_bitmap(inode.triple_indirect);

        unsigned int *block = new unsigned int[blockSize / sizeof(unsigned int)];
        read_block(inode.triple_indirect, block);
        for(int i=0; i < blockSize / sizeof(unsigned int); i++)
        {
            if(block[i] == 0)
            {
                continue;
            }
            set_1_bitmap(block[i]);

            unsigned int *block2 = new unsigned int[blockSize / sizeof(unsigned int)];
            read_block(block[i], block2);
            for(int j=0; j < blockSize / sizeof(unsigned int); j++)
            {
                if(block2[j] == 0)
                {
                    continue;
                }
                set_1_bitmap(block2[j]);

                unsigned int *block3 = new unsigned int[blockSize / sizeof(unsigned int)];
                read_block(block2[j], block3);
                for(int k=0; k < blockSize / sizeof(unsigned int); k++)
                {
                    if(block3[k] == 0)
                    {
                        continue;
                    }
                    set_1_bitmap(block3[k]);

                    unsigned int *tempBlock = new unsigned int[blockSize / sizeof(unsigned int)];
                    read_block(block3[k], tempBlock);
                    dirEntry = (struct ext2_dir_entry *)tempBlock;
                    traverse_dir_entry_block(dirEntry);
                }
            }
        }
    }
}

void set_1_bitmap(uint32_t block_num)
{
    if(blockSize == 1024)
    {
        block_num = block_num - 1;
    }
    int byteIndex = (block_num%superBlock.blocks_per_group) / 8;
    int bitIndex = (block_num%superBlock.blocks_per_group) % 8;
    b_bitmap[block_num / superBlock.blocks_per_group][byteIndex] |= (1 << bitIndex);
}


/*void recover_inode_bitmap(unsigned int inode_num)
{
    struct ext2_inode inode;
    read_inode(inode_num, &inode);

    unsigned int bitmap_num  = (inode_num - 1) / superBlock.inodes_per_group;
    unsigned int bitmap_offset = (inode_num - 1) % superBlock.inodes_per_group;

    i_bitmap[bitmap_num] |= 1 << bitmap_offset;

    if(!(inode.mode & EXT2_I_DTYPE))
    {
        return;
    }

    for(int i=0; i < EXT2_NUM_DIRECT_BLOCKS; i++)
    {
        if(inode.direct_blocks[i] == 0)
        {
            break;
        }
        char block[blockSize];
        read_block(inode.direct_blocks[i], block);
        struct ext2_dir_entry *dirEntry = (struct ext2_dir_entry *)block;
        unsigned int size = 0;
        while(size < blockSize && dirEntry->length != 0)
        {
            if(dirEntry->inode == 0)
            {
                size += dirEntry->length;
                dirEntry = (struct ext2_dir_entry *)((char *)dirEntry + dirEntry->length);
                continue;
            }

            char name[dirEntry->name_length + 1];
            memcpy(name, dirEntry->name, dirEntry->name_length);
            name[dirEntry->name_length] = '\0';

            if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            {
                size += dirEntry->length;
                dirEntry = (struct ext2_dir_entry *)((char *)dirEntry + dirEntry->length);
                continue;
            }

            recover_inode_bitmap(dirEntry->inode);
            size += dirEntry->length;
            dirEntry = (struct ext2_dir_entry *)((char *)dirEntry + dirEntry->length);
        }
    }
}*/

void read_inode_bitmap(uint32_t* i_bitmap, unsigned int group_num)
{
    lseek(image, groupDescriptors[group_num].inode_bitmap * blockSize, SEEK_SET);
    read(image, i_bitmap, superBlock.inodes_per_group/8);
}

void read_block(unsigned int block_num, void *buffer)
{
    lseek(image, block_num * blockSize, SEEK_SET);
    read(image, buffer, blockSize);
}

void read_inode(unsigned int inode_num, struct ext2_inode *inode) 
{
    unsigned int group_num = (inode_num - 1) / superBlock.inodes_per_group;
    unsigned int inode_table_block = groupDescriptors[group_num].inode_table;

    unsigned int inode_size = superBlock.inode_size;
    unsigned int inode_index = (inode_num - 1) % superBlock.inodes_per_group;

    unsigned int block_offset = (inode_index * inode_size) / blockSize;
    unsigned int inode_offset = (inode_index * inode_size) % blockSize;
    
    char block[blockSize];
    read_block(inode_table_block + block_offset, block);
    memcpy(inode, block + inode_offset, sizeof(struct ext2_inode));
}

void print_dir_entry_block(struct ext2_dir_entry *dirEntry, int depth)
{
    unsigned int size = 0;
        while(size < blockSize && dirEntry->length != 0)
        {
            if(dirEntry->inode == 0 || dirEntry->name_length == 0)
            {
                size += dirEntry->length;
                dirEntry = (struct ext2_dir_entry *)((char *)dirEntry + dirEntry->length);
                continue;
            }

            char name[dirEntry->name_length + 1];
            memcpy(name, dirEntry->name, dirEntry->name_length);
            name[dirEntry->name_length] = '\0';

            if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            {
                size += dirEntry->length;
                dirEntry = (struct ext2_dir_entry *)((char *)dirEntry + dirEntry->length);
                continue;
            }

            for(int j=0; j < depth; j++) 
            {
                printf("-");
            }
            //printing dir_entry
            //print_dir_entry(dirEntry, name);
            if(dirEntry->file_type == EXT2_D_DTYPE)
            {
                printf(" %s/\n", name);
                print_directory(dirEntry->inode, depth + 1);
            }
            else
            {
                printf(" %s\n", name);
            }
            size += dirEntry->length;
            dirEntry = (struct ext2_dir_entry *)((char *)dirEntry + dirEntry->length);
        }
}

void print_directory(unsigned int inode_num, int depth)
{
    struct ext2_inode inode;
    read_inode(inode_num, &inode);
    if (!(inode.mode & EXT2_I_DTYPE)) 
    {
        return; // Not a directory
    }

    char block[blockSize];
    struct ext2_dir_entry *dirEntry;
    
    for(int i=0; i < EXT2_NUM_DIRECT_BLOCKS; i++)
    {
        if(inode.direct_blocks[i] == 0)
        {
            continue;
        }
        read_block(inode.direct_blocks[i], block);
        dirEntry = (struct ext2_dir_entry *)block;
        print_dir_entry_block(dirEntry, depth);   
    }

    if(inode.single_indirect != 0)
    {
        unsigned int *block = new unsigned int[blockSize / sizeof(unsigned int)];
        read_block(inode.single_indirect, block);
        for(int i=0; i < blockSize / sizeof(unsigned int); i++)
        {
            if(block[i] == 0)
            {
                continue;
            }
            unsigned int *tempBlock = new unsigned int[blockSize / sizeof(unsigned int)];
            read_block(block[i], tempBlock);
            dirEntry = (struct ext2_dir_entry *)tempBlock;
            print_dir_entry_block(dirEntry, depth);
        }
    }
    if(inode.double_indirect != 0)
    {
        unsigned int *block = new unsigned int[blockSize / sizeof(unsigned int)];
        read_block(inode.double_indirect, block);
        for(int i=0; i < blockSize / sizeof(unsigned int); i++)
        {
            if(block[i] == 0)
            {
                continue;
            }
            unsigned int *block2 = new unsigned int[blockSize / sizeof(unsigned int)];
            read_block(block[i], block2);
            for(int j=0; j < blockSize / sizeof(unsigned int); j++)
            {
                if(block2[j] == 0)
                {
                    continue;
                }
                unsigned int *tempBlock = new unsigned int[blockSize / sizeof(unsigned int)];
                read_block(block2[j], tempBlock);
                dirEntry = (struct ext2_dir_entry *)tempBlock;
                print_dir_entry_block(dirEntry, depth);
            }
        }
    }
    if(inode.triple_indirect != 0)
    {
        unsigned int *block = new unsigned int[blockSize / sizeof(unsigned int)];
        read_block(inode.triple_indirect, block);
        for(int i=0; i < blockSize / sizeof(unsigned int); i++)
        {
            if(block[i] == 0)
            {
                continue;
            }
            unsigned int *block2 = new unsigned int[blockSize / sizeof(unsigned int)];
            read_block(block[i], block2);
            for(int j=0; j < blockSize / sizeof(unsigned int); j++)
            {
                if(block2[j] == 0)
                {
                    continue;
                }
                unsigned int *block3 = new unsigned int[blockSize / sizeof(unsigned int)];
                read_block(block2[j], block3);
                for(int k=0; k < blockSize / sizeof(unsigned int); k++)
                {
                    if(block3[k] == 0)
                    {
                        continue;
                    }
                    unsigned int *tempBlock = new unsigned int[blockSize / sizeof(unsigned int)];
                    read_block(block3[k], tempBlock);
                    dirEntry = (struct ext2_dir_entry *)tempBlock;
                    print_dir_entry_block(dirEntry, depth);
                }
            }
        }
    }
}

    