
// now, test only haswell..

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <ctime>

#include "mimalloc-new-delete.h"

#include "claujson.h" // using simdjson 2.0.0

using namespace std::literals::string_view_literals;

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
			//claujson::LoadData::save("output8.json", ut);
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

