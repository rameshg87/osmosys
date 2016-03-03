OUTPUT_FORMAT("binary")
ENTRY(_start)
phys = 0x100000;
SECTIONS
{
  .text phys : AT(phys) {
    code = .;
    *(.text)
    *(.rodata)
  }

  .data : AT(phys + (data - code))
  {
    data = .;
   start_ctors = .;
   *(.ctor*)
   end_ctors = .;
   start_dtors = .;
   *(.dtor*)
   end_dtors = .;
   *(.data)

    *(.data)
  }
  .bss : AT(phys + (bss - code))
  {
    bss = .;
    *(.bss)
  }
  end = .;
}

