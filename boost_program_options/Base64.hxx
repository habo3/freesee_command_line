/*
 *  Base64.hxx
 *  FreeSeeClient
 *
 *  Created by Eugene Nikolsky on 4/4/12.
 *  Copyright 2012 SoftServe. All rights reserved.
 *
 */

#ifndef _BASE64_HXX_
#define _BASE64_HXX_

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

#endif // _BASE64_HXX_