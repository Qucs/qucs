/***************************************************************************
                              helloworld.cpp
                                ----------
    begin                : yes
    copyright            : 2016 by Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This plugin is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// testing basic load/unload capabilities.

#include <iostream>

namespace{
using namespace std;
class EXIT{
	public:
		EXIT(){
			std::cout << "hello\n";
		}
		~EXIT(){
			std::cout << "goodbye\n";
		}
}exit;
}
