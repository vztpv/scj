
// now, test only haswell..
// need C++17, 64bit..

#define _CRT_SECURE_NO_WARNINGS

#include "mimalloc-new-delete.h"


#include <iostream>
#include <string>
#include <ctime>



#include "claujson.h" // using simdjson 2.0.0

#include <cstring>

using namespace std::literals::string_view_literals;


using namespace scj;

void test() {
	{
		// create an empty structure (null)
		json j;

		// add a number that is stored as double (note the implicit conversion of j to an object)
		j["pi"] = 3.141;

		// add a Boolean that is stored as bool
		j["happy"] = true;

		// add a string that is stored as std::string
		
			j["name"] = "Niels";
			
			j["name"] = "eee";
		
		// add another null object by passing nullptr
		j["nothing"] = nullptr;

		// add an object inside the object
		j["answer"]["everything"] = 42;
		
		std::cout << j << "\n";

		j["answer"]["wow"] = 33;

		std::cout << j << "\n";

		//j["array_name"] = { 3, 4, 5 } <- not supported.
		json temp = j["array_name"];

		temp.push_back(1);
		temp.push_back(2.5);
		temp.push_back("test");

		std::cout << j << "\n";
		// at? vs []
	}

	{
		json j;
		j.push_back("foo");
		j.push_back(1);
		j.push_back(true); 

		std::cout << j << "\n";


		// other stuff
		j.size();  
		j.empty();    
		j.type();     
		j.clear();   

		// convenience type checkers
		j.is_null();
		j.is_boolean();
		j.is_number();
		j.is_object();
		j.is_array();
		j.is_string();

		// create an object
		json o;
		o["foo"] = 23;
		o["bar"] = false;
		o["baz"] = 3.141;



		// find an entry
		if (o.contains("foo")) {
			// there is an entry with key "foo"
		}

		std::cout << o << "\n";

		// delete an entry
		o.erase("foo");


		std::cout << o << "\n";
	}
}


int main(int argc, char* argv[])
{
	//getchar();
	std::cout << sizeof(claujson::ItemType) << "\n";
	std::cout << sizeof(claujson::UserType) << "\n";
	for (int i = 0; i < 1; ++i) {
		claujson::UserType ut;
		try {
			int a = clock();
			
			auto x = claujson::Parse(argv[1], 64, &ut);
			if (!x.first) {
				std::cout << "fail\n";
				return 1;
			}

			int b = clock();
			std::cout << "total " << b - a << "ms\n";


			//claujson::LoadData::_save(std::cout, &ut);
			//claujson::LoadData::save(std::cout, ut);
			//claujson::LoadData::save("output9.json", ut);
			int c = clock();
			std::cout << c - b << "ms\n";

			//test2(&ut);

			{/*
				//claujson::ChkPool(ut.get_data_list(0), poolManager2);

				//poolManager.Clear();

				//claujson::LoadData::_save(std::cout, &ut);

				for (int i = 0; i < 5; ++i)
				{
					int a = clock();
					double sum = 0;
					int64_t chk = 0;

					claujson::UserType* A = ut.get_data_list(0)->find_ut("features"sv);

					// no l,u,d  any
					// true      true
					// false     true

					for (auto iter = A->get_data().begin(); iter != A->get_data().end(); ++iter)
					{
							claujson::UserType* y = (*iter)->find_ut("geometry"sv); // as_array()[t].as_object()["geometry"];

							//chk = (int)y;


						if(y) {
						//	chk += y->get_data_size();
							claujson::UserType* yyy =  y->find_ut("coordinates"sv);

							//chk += (int)yyy;

							//if (yyy) {
								yyy = yyy->get_data_list(0);
							//}

							//if (yyy) {
								//chk += yyy->get_data_size();
								for (claujson::UserType* z : yyy->get_data()) {
									for (claujson::UserType* _z : z->get_data()) {  //size3; ++w2) {
										if (_z->get_value().data.type == simdjson::internal::tape_type::DOUBLE) {
											sum += _z->get_value().data.float_val;
										}
									}
								}
							//}

							//	//std::cout << dur.count() << "ns\n";

						}

					}


					std::cout << sum << "\n";
					std::cout << clock() - a << "ms\n";
					std::cout << "chk " <<  chk << "\n";
					////std::cout << "time " << std::chrono::duration_cast<std::chrono::milliseconds>(time).count() << "ms\n";

				}*/
			}

			//getchar();

			bool ok = x.first;
			
			//ut.remove_all(poolManager);
			
			if (ok) {
				test();

				try {

					scj::json x{ &ut };

					std::cout << x.is_object() << "\n";
					std::cout << x["type"] << "\n";
					std::cout << x["features"][0] << "\n"; // no features -> make features. but no idx -> throw const_char*.
					
					x["features"][0].clear();


				}
				catch (const char* cstr) {
					std::cout << cstr << "\n";
				}
				catch (...) {
					std::cout << "Error..\n";
				}
			}

			return !ok;
		}
		catch (...) {
			std::cout << "internal error\n";
			return 1;
		}
	}
	
	//getchar();

	return 0;
}

