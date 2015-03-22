{ print("\n        Vypis ASCII tabulky\n\n   ");
  for (i=0;i<16;++i) print(" %X",i);
  print("\n   ");
  for (i=0;i<32;++i) print("-");

  for (znak=' '; znak<256; znak+=16) { print("\n%X|",znak);
                                       for (j=0;j<16;++j) print(" %c",znak+j); }
  
  print("\n\n");
}