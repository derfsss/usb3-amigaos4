ENTRY(_start)

PHDRS {
  text    PT_LOAD FLAGS(5);   /* Read Exe */
  data    PT_LOAD FLAGS(7);   /* Read Write Exe */
}

SECTIONS
{
  /* ---- CODE SEGMENT (R+X) ---- */
  .text 0x01000000 : ALIGN(4)
  {
    *(.text)
    *(.text.*)
  } :text

  /* ---- DATA SEGMENT (R+W+X) ---- */
  .data : ALIGN(4)
  {
    *(.rodata)
    *(.rodata.*)
    *(.data)
    *(.data.*)
    *(.bss)
    *(.bss.*)
  } :data
}
