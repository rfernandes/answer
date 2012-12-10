#include "answer/Utils.hh"

#include <string>
#include <iostream>
#include <sstream>
#include <string.h>

using namespace std;

namespace answer {

string encodeForXml( const string &sSrc, bool encode_special ) {
    ostringstream sRet;
    for ( string::const_iterator iter = sSrc.begin(); iter!=sSrc.end(); ++iter ) {
        unsigned char c = (unsigned char)*iter;
        switch ( c ) {
        case '&':
            sRet << "&amp;";
            break;
        case '<':
            sRet << "&lt;";
            break;
        case '>':
            sRet << "&gt;";
            break;
        case '"':
            sRet << "&quot;";
            break;
        case '\'':
            sRet << "&apos;";
            break;

        default:
            sRet << c;
        }
    }
    return sRet.str();
}


string decodeFromXml(const string &in) {
	static const struct entities_s {
		string name;
		int	value;
	} entities[] = {
		{"quot", 34},
		{"amp", 38},
		{"apos", 39},
		{"lt", 60},
		{"gt", 62}
	};

	#define NR_OF_ENTITIES 5

	string _ent;
	int i = 0;
	string ret = "";
	size_t pos = 0, wpos = 0, prev = 0;
	bool found_entity = false;
	bool found_num_ent = false;

	for(; pos < in.length() && wpos != string::npos;) {
		wpos = in.find('&', pos);
		prev = pos;
		pos = wpos;
		//found a '&'
		if(pos != string::npos) {
			//going to check if it is an html entity
			for(i = 0; i < NR_OF_ENTITIES; i++) {
				_ent = "&";
				_ent.append(entities[i].name);
				_ent.append(";");
				//if are equal
				if(in.compare(pos, _ent.length(), _ent) == 0) {
					found_entity = true;

					ret.append(in, prev, pos - prev);
					wchar_t c(entities[i].value);
					ret.push_back(c);
					prev = pos + _ent.length();
					pos = prev - 1;
					break;
				}

			}
			if(!found_entity && in.at(pos + 1) == '#') {
				found_num_ent = true;
				size_t pos_aux = 0;
				char *num_ent;
				ret.append(in, prev, pos - prev);
				pos_aux = in.find(';', pos);

				num_ent = (char *) calloc(pos_aux - pos, sizeof(char));
				in.copy(num_ent, (pos_aux - 1) - (pos + 1), pos + 2);

				wchar_t c(atoi(num_ent));
				ret.push_back(c);
				prev = pos + strlen(num_ent) + 3; // add 3 because of the 3 characters we erased
				pos = prev - 1;

				if(num_ent)
					free(num_ent);
			}

			if(!found_entity && !found_num_ent)
				ret.append(in, prev, pos - prev);

			pos++;
			found_entity = false;
			found_num_ent = false;
		}
	}

	if(!ret.empty())
		ret.append(in, prev, pos - prev);
	else
		ret.append(in);

	return ret;

}

}

