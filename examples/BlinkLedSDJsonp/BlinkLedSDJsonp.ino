// -*- c++ -*-
//
// Copyright 2010 Ovidiu Predescu <ovidiu@gmail.com>
// Date: December 2010
// Updated: 08-JAN-2012 for Arduno IDE 1.0 by <Hardcore@hardcoreforensics.com>
// Updated: 27-AUG-2014 added function get_jsonpid_from_path() by <stexe@systemfailure.it>
//

#include <SPI.h>
#include <Ethernet.h>
#include <Flash.h>
#include <SD.h>
#include <TinyWebServer.h>

const int SD_CS = 4;
const int ETHER_CS = 10;
byte ip[] = { 192, 168, 1, 177 };
byte gateway[] = { 192, 168, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const int port = 80;

boolean file_handler(TinyWebServer& web_server);
boolean index_handler(TinyWebServer& web_server);
boolean jsonp_handler(TinyWebServer& web_server);
boolean toogle_handler(TinyWebServer& web_server);

TinyWebServer::PathHandler handlers[] = {
  {"/", TinyWebServer::GET, &index_handler },
  {"/upload/" "*", TinyWebServer::PUT, &TinyWebPutHandler::put_handler },
  {"/jsonp" "*", TinyWebServer::GET, &jsonp_handler },
  {"/toogleled" "*", TinyWebServer::GET, &toogle_handler },
  {"/" "*", TinyWebServer::GET, &file_handler },
  {NULL},
};

const char* headers[] = {"Content-Length",NULL};
TinyWebServer web = TinyWebServer(handlers, headers, port);

boolean has_filesystem = true;
Sd2Card card;
SdVolume volume;
SdFile root;
SdFile file;

void send_file_name(TinyWebServer& web_server, const char* filename) {
  if (!filename) {
    web_server.send_error_code(404);
    web_server << F("Could not parse URL");
  } else {
    TinyWebServer::MimeType mime_type = TinyWebServer::get_mime_type_from_filename(filename);
    web_server.send_error_code(200);
    web_server.send_content_type(mime_type);
    web_server.end_headers();
    if (file.open(&root, filename, O_READ)) 
    {
      Serial << F("Read file "); Serial.println(filename);
      web_server.send_file(file);
      file.close();
    } else {
      web_server << F("Could not find file: ") << filename << "\n";
    }
  }
}

boolean jsonp_handler(TinyWebServer& web_server) {
  char* jsonp = TinyWebServer::get_jsonpid_from_path(web_server.get_path());
  web_server.send_error_code(200);
  web_server.send_content_type("application/json");
  web_server.end_headers();
  
  // Send JSON string
  web_server << ("jsonp");
  web_server << (jsonp);
  web_server << F("({\"id\" : \"AjaxUI\", ");
  web_server << F("\"light\" : ");
  web_server << F("[{ \"status\" : \"");
  web_server << (digitalRead(7));
  web_server << F("\"}");
  web_server << F("]})");
  free(jsonp);
  return true;
}

boolean file_handler(TinyWebServer& web_server) {
  char* filename = TinyWebServer::get_file_from_path(web_server.get_path());
  send_file_name(web_server, filename);
  free(filename);
  return true;
}

boolean index_handler(TinyWebServer& web_server) {
  send_file_name(web_server, "index.htm");
  return true;
}

boolean toogle_handler(TinyWebServer& web_server) {
  web_server.send_error_code(200);
  web_server.end_headers();
  web_server << F ("({})");
  // Toogle the LED status
  if(digitalRead(7)){
    digitalWrite(7, LOW);
  } else{
    digitalWrite(7, HIGH);
  }
  return true;
}

void setup() {
  Serial.begin(9600);
  pinMode(SS_PIN, OUTPUT);	// set the SS pin as an output                                
  digitalWrite(SS_PIN, HIGH);	// and ensure SS is high
  pinMode(ETHER_CS, OUTPUT); 	// Set the CS pin as an output
  digitalWrite(ETHER_CS, HIGH); // Turn off the W5100 chip! (wait for                              
  pinMode(SD_CS, OUTPUT);       // Set the SDcard CS pin as an output
  digitalWrite(SD_CS, HIGH); 	// Turn off the SD card! (wait for
                               
  card.init(SPI_FULL_SPEED, SD_CS); 
  volume.init(&card);  
  root.openRoot(&volume); 
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  web.begin();
  Serial << F("Ready to accept HTTP requests.\n");
  
  // Use Pin13 to drive a LED
  pinMode(13, OUTPUT);
}

void loop() 
{  
    web.process();
}
