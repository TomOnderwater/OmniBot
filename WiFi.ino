#ifdef APINPUT
void configureAP()
{
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // HTTP handler assignment
  webserver.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html_gz, sizeof(index_html_gz));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  //start server
  webserver.begin();
  server.listen(82);
  Serial.print("Is server live? ");
  Serial.println(server.available());

}

void handle_message(WebsocketsMessage msg) {
  //String lines[] = msg.data().split(',');
  char number[5] = {'\0','\0','\0','\0','\0'};
  int l = 0;
  int values[4] = {0,0,0,0};
  int count = 0;
  for (int i = 0; i < sizeof(msg.data()); i++)
  {
   char c = msg.data()[i];
   if ((c >= 48 && c <= 57) || c == '-') 
   {
    number[l] = c;
    l++;
   }
   if (c == ',' || i == sizeof(msg.data()) - 1) 
   {
    sscanf(number, "%d", &values[count]);
    for (int j = 0; j < 5; j++)
    {
      number[j] = '\0';
    }
    l = 0;
    count ++;
   }
  }
  input.x = constrain(values[0], -128, 127);
  input.y = constrain(values[1], -128, 127);
  //Serial.println(msg.data());
  //commaIndex = msg.data().indexOf(',');
  //LValue = msg.data().substring(0, commaIndex).toInt();
  //RValue = msg.data().substring(commaIndex + 1).toInt();
 //do something with the data here:
 //input.x = map(L
 //input.x = map(LValue, -100, 100, -127, 128);
 //input.y = map(RValue, -100, 100, -127, 128);
}





#endif
