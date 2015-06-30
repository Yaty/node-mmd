#include <node.h>
#include <v8.h>
#include <nan.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include "extensions.hpp"

/* Define output formats we support -- first in list is default */
enum export_formats {
        HTML_FORMAT,
        TEXT_FORMAT,
        LATEX_FORMAT,
        MEMOIR_FORMAT,
        BEAMER_FORMAT,
        OPML_FORMAT,
        ODF_FORMAT,
        RTF_FORMAT,
        ORIGINAL_FORMAT,                 /* Not currently used */
        CRITIC_ACCEPT_FORMAT,
        CRITIC_REJECT_FORMAT,
        CRITIC_HTML_HIGHLIGHT_FORMAT,
};

extern "C" {
    char * markdown_to_string(char *text, unsigned long extensions, int output_format);
}

using namespace v8;
using namespace std;

/**
 * Handle<T> is a v8 Class Template reference
 * ;an abstract class
 * */
NAN_METHOD(Convert) {
    NanScope();
    unsigned long extensions = EXT_SMART | EXT_NOTES | EXT_SNIPPET | EXT_RANDOM_FOOT;
    int format = HTML_FORMAT;

    //uses v8::Value::IsString() method to determine if the arg is an string
    if(args.Length() < 1 || !args[0]->IsString()) {
        return NanThrowError("Must pass string as first argument");
    }

    if(args.Length() == 2) {
        v8::Handle<v8::Object> cfg = v8::Handle<v8::Object>::Cast(args[1]);
        if(cfg->Has(NanNew<v8::String>("full"))){
            if((cfg->Get(NanNew<v8::String>("full"))->IsBoolean())) {
                bool _full  = cfg->Get(NanNew<v8::String>("full"))->ToBoolean()->Value();
                if(_full) {
                    extensions &= ~EXT_SNIPPET;
                    extensions = extensions | EXT_COMPLETE;
                }
            }
        }
        if(cfg->Has(NanNew<v8::String>("format"))) {
            if((cfg->Get(NanNew<v8::String>("format"))->IsString())) {
                Local<String> fmt = cfg->Get(NanNew<v8::String>("format"))->ToString();
                int fmtLen = fmt->Utf8Length();

                char *fmtBuf = (char*) malloc(fmtLen + 1);
                memset(fmtBuf,0,fmtLen+1);
                fmt->WriteUtf8(fmtBuf,fmtLen,NULL,0);
                if(strcmp(fmtBuf,"html")==0) {
                    format = HTML_FORMAT;
                } else if(strcmp(fmtBuf,"odf")==0) {
                    format = ODF_FORMAT;
                } else if(strcmp(fmtBuf,"text")==0) {
                    format = TEXT_FORMAT;
                } else if(strcmp(fmtBuf,"rtf")==0) {
                    format = RTF_FORMAT;
                } else {
                    free(fmtBuf);
                    return NanThrowError("Invalid format");
                }

                free(fmtBuf);


            }

        }

    }


    //A Local<T> type of Handle<T>; contrasted to a Persistent<T> Handle
    //Calls v8::Value::ToString(), returning an Local<String>
    v8::Local<v8::String> ls = args[0]->ToString();

    int stringLen = ls->Utf8Length();

    // Allocate memory for input string
    char *buf = (char*) malloc(stringLen + 1);
    memset(buf, 0, stringLen + 1);
    ls->WriteUtf8(buf, stringLen, NULL, 0);

    // Convert to markdown
    char *out = markdown_to_string(buf, extensions, format);
    free(buf);

    // Convert to V8 string
    v8::Local<v8::String> outString = NanNew<v8::String>(out);
    free(out);

    NanReturnValue(outString);
}


