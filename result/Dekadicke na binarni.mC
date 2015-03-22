{ print("Zadej nezaporne cislo: ");
  scan(a);

  if (a<0) print("\n\nChybne cislo\n");

  else { print("\n\nBinarni zapis: ");
  
         nula=true;
  
         for (b=0x40000000; b>0; b>>=1) { podil=a/b;
                                          if (podil!=0) nula=false;
                                          if (!nula || b==1) { if (podil!=0) print("1"); else print("0"); }
                                          a%=b;
                                        }
         print("\n\n");
       }
}