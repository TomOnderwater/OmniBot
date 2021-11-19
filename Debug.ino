void emptyDeBuff()
{
  deBuff[0] = '\0';
  /*
  for (int i = 0; i < BUFFSIZE; i++)
  {
    deBuff[i] = 0;
  }
  */
}

void printDebug()
{
  Serial.print(*deBuff);
  //  for (int i = 0; i < 256; i++)
  //  {
  //    if (deBuff[i]) Serial.print(deBuff[i]);
  //    else break;
  //  }
  emptyDeBuff();
}
