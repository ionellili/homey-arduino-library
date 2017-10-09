#ifndef _HOMEY_H_
#define _HOMEY_H_

// Settings
//#define HOMEY_USE_ETHERNET_V1 //Uncomment when using a legacy ethernet shield
#define DEBUG_ENABLE //Uncomment to have the library print debug messages

// Advanced settings
#define DEBUG_PRINTER Serial //Which class to use for printing debug mesages

/* -------------- DO NOT EDIT ANYTHING BELOW THIS LINE!  -------------- */
/* (If you do you might break compatibility with the Homeyduino app...) */

//
#define ENDPOINT_MAX_SIZE 17 //16 + null
#define ARGUMENT_MAX_SIZE 65 //64 + null
#define REQUEST_MAX_SIZE ENDPOINT_MAX_SIZE+ARGUMENT_MAX_SIZE
#define HEADER_MAX_SIZE REQUEST_MAX_SIZE+16
#define REQUEST_TIMEOUT 100

#define MAX_NAME_LENGTH 32
#define MAX_TYPE_LENGTH 32

#define TYPE_ACTION 		"act"
#define TYPE_CONDITION		"con"
#define TYPE_CAPABILITY		"cap"
#define TYPE_SYSTEM			"sys"
#define TYPE_TRIGGER		"trg"
#define TYPE_RAW			"raw"	//Not handled by Homeyduino app!

#define CTYPE_NULL			"null"
#define CTYPE_STRING		"String"
#define CTYPE_INT			"Number"
#define CTYPE_FLOAT			"Number"
#define CTYPE_DOUBLE		"Number"
#define CTYPE_BOOL			"Boolean"

#define BVAL_TRUE			"true"
#define BVAL_FALSE			"false"

#define DTYPE_UNKNOWN		"unknown"		//Unconfigured device
#define DTYPE_HOMEYDUINO 	"homeyduino"	//Homeyduino device

#define ENDPOINT_SET_MASTER "/sys/setmaster"

//Includes
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP8266)
	#include <ESP8266WiFi.h>
	#include <WiFiClient.h>
	#include <WiFiUdp.h>		
	#define CLIENT_TYPE WiFiClient
	#define UDP_SERVER_TYPE WiFiUDP
	#define TCP_SERVER_TYPE WiFiServer
	#define MAXCALLBACKS 10
#elif defined(ARDUINO_ARCH_ESP32)
	#include <WiFi.h>
	#include <WiFiUdp.h>
	#define CLIENT_TYPE WiFiClient
	#define UDP_SERVER_TYPE WiFiUDP
	#define TCP_SERVER_TYPE WiFiServer
	#define UDP_TX_PACKET_MAX_SIZE 1024
	#define MAXCALLBACKS 10
#elif defined(HOMEY_USE_ETHERNET_V1)
	#include <Ethernet.h>
	#include <EthernetUdp.h>
	#define CLIENT_TYPE EthernetClient
	#define UDP_SERVER_TYPE EthernetUDP
	#define TCP_SERVER_TYPE EthernetServer
	#define MAXCALLBACKS 10
	#define CAN_NOT_STOP_TCP
#else
	#include <Ethernet2.h>
	#include <EthernetUdp2.h>
	#define CLIENT_TYPE EthernetClient
	#define UDP_SERVER_TYPE EthernetUDP
	#define TCP_SERVER_TYPE EthernetServer
	#define MAXCALLBACKS 10
	#define CAN_NOT_STOP_TCP
#endif

//Debug function definition
#ifdef DEBUG_ENABLE
  #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif

//Type definitions
typedef void (*CallbackFunction)(void);

//Struct definitions
struct HomeyFunction {
	HomeyFunction(char* newName, char* newType, CallbackFunction newCallback);
	HomeyFunction* prevFunction;	//Linked list (<)
	HomeyFunction* nextFunction;	//Linked list (>)
	char* type;				//Type
	char* name;				//Name
	CallbackFunction callback;		//Function pointer
};

struct WebResponse {
	uint16_t code;
	String response;
	String type;
};

struct WebRequest {
	char endpoint[ENDPOINT_MAX_SIZE];
	String getArgs;
	String postArgs;
};

class HomeyClass {
	public:
		//Library and device management
		HomeyClass( uint16_t port = 46639 ); 									//Class constructor (port 46639 is t9 for HOMEY)
		void begin(const String& name, const String& type = DTYPE_HOMEYDUINO);	//Start responding to queries
		void stop();															//Stop responding to queries
		void name(const String& name);											//Change the device identifier
		void type(const String& type);											//Change the device type
		
		//API endpoint management
		bool onAction(const String& name, CallbackFunction fn);					//Wrapper for on(String&, String&,...) that supplies type as TYPE_ACTION
		bool onCondition(const String& name, CallbackFunction fn);				//Wrapper for on(String&, String&,...) that supplies type as TYPE_CONDITION
		bool onCapability(const String& name, CallbackFunction fn);				//Wrapper for on(String&, String&,...) that supplies type as TYPE_CAPABILITY
		HomeyFunction* findAction(const char* name);							//Wrapper for find(...) that supplies type as TYPE_ACTION
		HomeyFunction* findCondition(const char* name);							//Wrapper for find(...) that supplies type as TYPE_CONDITION
		HomeyFunction* findCapability(const char* name);						//Wrapper for find(...) that supplies type as TYPE_CAPABILITY
		bool removeAction(const char* name);									//Wrapper for remove(...) that supplies type as TYPE_ACTION
		bool removeCondition(const char* name);									//Wrapper for remove(...) that supplies type as TYPE_CONDITION
		bool removeCapability(const char* name);								//Wrapper for remove(...) that supplies type as TYPE_CAPABILITY
		void clear();															//Removes all endpoints
				
		//Send a trigger event to a Homey flow
		bool trigger(const String& name);										//Wrapper for emit(...) with NULL argument and type set to trigger
		bool trigger(const String& name, const char* value);					//Wrapper for emit(...) with char array argument and type set to trigger
		bool trigger(const String& name, const String& value);					//Wrapper for emit(...) with String argument and type set to trigger
		bool trigger(const String& name, bool value);							//Wrapper for emit(...) with bool argument and type set to trigger
		bool trigger(const String& name, int value);							//Wrapper for emit(...) with int argument and type set to trigger
		bool trigger(const String& name, float value);							//Wrapper for emit(...) with float argument and type set to trigger
		bool trigger(const String& name, double value);							//Wrapper for emit(...) with double argument and type set to trigger
		
		
		//Update a capability value
		bool setCapabilityValue(const String& name);							//Wrapper for emit(...) with NULL argument and type set to capability
		bool setCapabilityValue(const String& name, const char* value);			//Wrapper for emit(...) with char array argument type set to capability
		bool setCapabilityValue(const String& name, const String& value);		//Wrapper for emit(...) with String argument type set to capability
		bool setCapabilityValue(const String& name, bool value);				//Wrapper for emit(...) with bool argument type set to capability
		bool setCapabilityValue(const String& name, int value);					//Wrapper for emit(...) with int argument type set to capability
		bool setCapabilityValue(const String& name, float value);				//Wrapper for emit(...) with float argument type set to capability
		bool setCapabilityValue(const String& name, double value);				//Wrapper for emit(...) with double argument type set to capability
		
		//Send a raw event (not handled by Homeyduino app!)
		bool emit(const String& name);											//Wrapper for emit(...) with NULL argument and type set to raw
		bool emit(const String& name, const char* value);						//Wrapper for emit(...) with char array argument type set to raw
		bool emit(const String& name, const String& value);						//Wrapper for emit(...) with String argument type set to raw
		bool emit(const String& name, bool value);								//Wrapper for emit(...) with bool argument type set to raw
		bool emit(const String& name, int value);								//Wrapper for emit(...) with int argument type set to raw
		bool emit(const String& name, float value);								//Wrapper for emit(...) with float argument type set to raw
		bool emit(const String& name, double value);							//Wrapper for emit(...) with double argument type set to raw
		
		//Set the answer returned from within an api endpoint
		void returnIndex();														//Return the API index
		void returnNothing();													//Return nothing
		void returnError(const String& error, uint16_t code = 500);				//Return an error message
		void returnResult(const char* result);									//Return a Char array
		void returnResult(const String& result);								//Return a String
		void returnResult(bool result);											//Return a bool
		void returnResult(int result);											//Return an int
		void returnResult(float result);										//Return a float
		void returnResult(double result);										//Return a double
		
		//Handle incoming connections
		void loop();															//Wrapper that runs both TCP and UDP handlers
		
		//Public variables
		String value;															//The argument supplied by the Homey flow
		
	private:
		//Helper functions
		bool split(char* buffer, char*& a, char*& b, char separator,			//Splits a buffer into separate parts
		uint16_t size);				
		char* copyCharArray(const char* input, uint16_t maxlen);				//Allocates memory and copies a char array
		bool parseHttpHeaders(CLIENT_TYPE* client);								//Parse HTTP headers and fill _request
		
		//API endpoint management
		bool on(const char* name, const char* type, CallbackFunction cb);		//Create an endpoint
		bool on(const String& name, const String& type, CallbackFunction fn);	//Wrapper for on(char*,char*,...) to allow for supplying string arguments
		HomeyFunction* find(const char* name, const char* type);				//Find an endpoint
		bool remove(const char* name, const char* type);						//Remove an endpoint

		//Request handling
		void handleRequest(char* endpoint, const char* argument);				//Handle API call
		bool handleTcp();														//Handle incoming TCP connections
		bool handleUdp();														//Handle incoming UDP connections
				
		//Event transmission
		bool _emit(const char* name, const char* argType, const String& value,	//Emit an event
		const char* evType);
				
		//Internal variables
		TCP_SERVER_TYPE _tcpServer;												//The TCP server
		UDP_SERVER_TYPE _udpServer;												//The UDP server
		uint16_t _port;															//The listening port for incoming connections
		String _deviceName;														//The device identifier
		String _deviceType;														//The device type
		HomeyFunction *callbacks[MAXCALLBACKS];									//The registered actions and conditions
		WebRequest _request;													//API request parameter storage
		WebResponse _response;													//API response parameter storage
		IPAddress _master_host;													//Master IP address
		uint16_t _master_port;													//Master port
		HomeyFunction* firstHomeyFunction = NULL;								//API callbacks linked list entry point
};

extern HomeyClass Homey;

#endif
