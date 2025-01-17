[bits 32]

; initialize 4-level paging and all the page tables
; maybe do 5-level paging in the future
; will only be mapping the first 2 MB because that is all that is required

init_page_tables_protected:
    pushad

    ; clear the tables
    mov edi, 0x1000 ; base addr (PML4T start addr). 
    mov cr3, edi    ; save in cr3. Is the control register CPU uses to locate page table entries
    xor eax, eax    ; clear the A-register. xor-ing is smaller and faster than 'mov eax, 0' 
    mov ecx, 4096   ; repeat 4096 times. Each page table is 4096 bytes and we'll be writing 4 bytes (double word) each rep so this covers the 4 tables
    rep stosd       ; rep - repeat string operating prefix ; stosd - store string (double word)
    mov edi, cr3    ; set edi back to PML4T[0]

    ; Now that the page are clear we're going to set up the tables. The page tables are going to be located at these addresses:
    ; PML4T - 0x1000.
    ; PDPT  - 0x2000.
    ; PDT   - 0x3000.
    ; PT    - 0x4000.
    ; Must make PML4T[0] point to the PDPT and so on
    ; PT will point to 0x00000000 - 0x00200000
    
    ; a bit tricky. The page tables must be page aligned 
    ; so the lower 12 bits (3 hex digits) must be 0. Then, each page table entry can use the lower 12 bits as flags for that entry

    ; will be setting the flag bits to 0x003
    ; bit 0 - present bit
    ; bit 1 - read/write bit
     
    mov dword[edi], 0x2003  ; set PML4T[0] to address 0x2000 (PDPT) with flags 0x0003
    add edi, 0x1000         ; go to PDPT[0]
    mov dword[edi], 0x3003  
    add edi, 0x1000         ; go to PDT[0] 
    mov dword[edi], 0x4003  
    add edi, 0x1000         ; go to PT[0]

    ; make an identity mapping for the PT. Keep the same flags as above
    ; but also want to set PT[0].addr to 0x0000, PT[1].addr to 0x1000 ...
    mov ebx, 0x00000003
    mov ecx, 512

    add_page_entry_protected:
       mov dword[edi], ebx  ; i.e. address.append(flags)
       add ebx, 0x1000
       add edi, 8           ; add 8 to destination index (entries are 8 bytes)
       loop add_page_entry_protected


    ; setup PAE paging, but do not enable it yet
    mov eax, cr4            ; 
    or eax, 1 << 5          ; set PAE-bit (bit 5) in cr4
    mov cr4, eax

    ; now have page tables that identity maps the lowest 2MB of physical memory into virtual memory

    popad
    ret

