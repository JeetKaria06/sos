#ifndef paging_h
#define paging_h

#include "multiboot.h"

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 512
#define PAGE_TABLE4 0xfffffffffffff000

typedef uint64_t page_t;
typedef uint64_t frame_t;
typedef void *virtual_address;
typedef uint64_t physical_address;

typedef struct {
    union {
        uint64_t packed;
        struct {
            uint8_t present : 1;
            uint8_t writable : 1;
            uint8_t user_accessable : 1;
            uint8_t write_thru_cache : 1;
            uint8_t disable_cache : 1;
            uint8_t accessed : 1;
            uint8_t dirty : 1;
            uint8_t huge_page : 1;
            uint8_t global : 1;
            uint8_t OS_1 : 1;
            uint8_t OS_2 : 1;
            uint8_t OS_3 : 1;
            uint64_t _addr_mask : 40;
            uint8_t OS_4 : 1;
            uint8_t OS_5 : 1;
            uint8_t OS_6 : 1;
            uint8_t OS_7 : 1;
            uint8_t OS_8 : 1;
            uint8_t OS_9 : 1;
            uint8_t OS_A : 1;
            uint8_t OS_B : 1;
            uint8_t OS_C : 1;
            uint8_t OS_D : 1;
            uint8_t OS_E : 1;
            uint8_t no_execute : 1;
        };
    };
}__attribute__((packed)) page_entry_t;
typedef page_entry_t *page_table_t;

typedef struct {
    uint64_t current_frame_index;
    struct memory_area *area;
    struct memory_map_tag *mem_info;
    physical_address kernel_start;
    physical_address kernel_end;
    physical_address mboot_start;
    physical_address mboot_end;
} frame_allocator;


uint64_t allocate_frame(frame_allocator *);
frame_allocator init_allocator(struct memory_map_tag *, physical_address,
                                                        physical_address,
                                                        physical_address,
                                                        physical_address);



uint64_t containing_address(uint64_t addr);
void unmap_page(page_t, frame_allocator *);
uint64_t starting_address(uint64_t page_frame);
page_table_t referenced_table(page_entry_t entry);
void map_page(page_t, uint8_t, frame_allocator *);
physical_address translate_address(virtual_address);
void identity_map(frame_t, uint8_t, frame_allocator *);
void map_page_to_frame(page_t, frame_t, uint8_t, frame_allocator *);


#endif

