#include <node.h>
#include <v8.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>

#include "extensions.hpp"
#include "arrayed.hpp"

extern "C" {
    char * extract_metadata_keys(char *text,unsigned long extensions);
}

using namespace v8;
using namespace std;


Handle<Value> ExtractMetadataKeys(const Arguments& args) {
    HandleScope scope;
    //uses v8::Value::IsString() method to determine if the arg is an string
    if(args.Length() < 1 || !args[0]->IsString()) {
        ThrowException(Exception::TypeError(String::New("Must pass file path as first argument")));
        return scope.Close(Undefined());
    }

    //A Local<T> type of Handle<T>; contrasted to a Persistent<T> Handle
    //Calls v8::Value::ToString(), returning an Local<String>
    Local<String> ls = args[0]->ToString();

    int stringLen = ls->Utf8Length();

    // Allocate memory for input string
    char *buf = (char*) malloc(stringLen + 1);
    memset(buf, 0, stringLen + 1);
    ls->WriteUtf8(buf, stringLen, NULL, 0);

    // Convert to keys list
    char *out = extract_metadata_keys(buf, EXT_SMART | EXT_NOTES);
    free(buf);

    Handle<Array> result = Arrayed(out);
    free(out);

    return scope.Close(result);



}

