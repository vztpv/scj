
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

namespace scj {

	

	class json {
	private:
		class json_ref {
			friend class json;
		private:
			int state = 0;
			claujson::UserType* node;
		public:
			explicit json_ref(claujson::UserType* other, int state = 0) { // chk other is not nullptr?
				node = other;
				this->state = state;
			}
		};
	public:
		bool is_ref()const { return is_ref_; }
	private:
		bool is_ref_ = false;
		int state = 0; // 1 object 2 array 0 item
		claujson::UserType* node = nullptr;
	private:
		// deep copy??
		explicit json(claujson::UserType* other_node) {
			node = other_node->clone();
		
			// cf) is_root()?
			if (other_node->is_array()) {
				state = 2;
			}
			else if (other_node->is_object()) {
				state = 1;
			}
			else { // else if(other_node->is_item_type()) {
				state = 0;
			}
		}
		explicit json(json_ref other) {
			is_ref_ = true;
			node = other.node;

			if (other.state == 0) {
				if (node->is_array()) {
					state = 2;
				}
				else if (node->is_object()) {
					state = 1;
				}
				else { // else if(other_node->is_item_type()) {
					state = 0;
				}
			}
		}	
	public:
		json() { node = new claujson::UserType(); }
		
		json(const json& other) {
			state = other.state;
			node = other.node->clone();
		}

		virtual ~json() {
			if (node && !is_ref_) {
				delete node;
			}
		}
		
		json& operator=(const json& other) {
			if (this == &other) {
				return *this;
			}

			state = other.state;
			
			if (node && !is_ref_) {
				delete node;
			}
			
			if (other.is_ref_) {
				node = other.node;
			}
			else {
				node = other.node->clone();
			}
			return *this;
		}
		
		json& operator=(json&& other) noexcept {
			std::swap(state, other.state);
			std::swap(node, other.node);
			std::swap(is_ref_, other.is_ref_);

			return *this;
		}

		json operator[](std::string_view key) {
			if (node->is_root()) {
				if (state == 0) {
					state = 1; // now, object.
					node->add_object_with_no_key(claujson::UserType::make_object());
				}

				if (state == 1) {
					if (auto* x = node->get_data_list(0)->find(key); x != nullptr) {
						return json(json_ref(x));
					}
					else {
						claujson::Data temp;
						temp.set_str(key.data(), key.size());
						return json(json_ref(node->get_data_list(0)->add_object_element(std::move(temp), claujson::Data())));
					}
				}
				else {
					throw "it is not object \n";
				}
			}
			else {
				if (state == 0) {
					state = 1; // now, object.
					node->to_object();
				}

				if (state == 1) {
					if (auto* x = node->find(key); x != nullptr) {
						return json(json_ref(x));
					}
					else {
						claujson::Data temp;
						temp.set_str(key.data(), key.size());
						return json(json_ref(node->add_object_element(std::move(temp), claujson::Data())));
					}
				}
				else {
					throw "it is not object \n";
				}
			}
		}

		template <class T>
		bool operator=(const T& x) {
			if (node && node->is_item_type()) {
				node->get_value().data = claujson::Data(x);
				return true;
			}
			return false;
		}

		template <class T>
		bool push_back(const T& data) {
			{
				if (state == 0) {
					state = 2; // now, array.
					node->to_array();
				}

				if (state == 2) {
					if (auto* x = node; x != nullptr) {
						node->add_array_element(claujson::Data(data));
						return true;
					}
					return false;
				}
				else {
					return false;
				}
			}
		}

		size_t size() const { 
			if (node->is_root()) {
				return node->get_data_list(0)->get_data_size();
			}
			return node->get_data_size();
		}

		bool empty() const {
			if (node->is_root()) {
				return node->get_data_list(0)->get_data_size() == 0;
			}
			return node->get_data_size() == 0;
		}

		void clear() {
			node->remove_all();
			state = 0;
		}

		// array item type
		simdjson::internal::tape_type type() const {
			return node->get_value().data.type();
		}
		
		bool is_null() const {
			return node->get_value().data.type() == simdjson::internal::tape_type::NULL_VALUE;
		}

		bool is_array() const {
			if (node->is_root()) {
				node->get_data_list(0)->is_array();
			}
			return node->is_array();
		}

		bool is_object() const {
			if (node->is_root()) {
				node->get_data_list(0)->is_object();
			}
			return node->is_object();
		}

		bool is_number() const {
			return node->get_value().data.type() == simdjson::internal::tape_type::INT64 ||
				node->get_value().data.type() == simdjson::internal::tape_type::UINT64 ||
				node->get_value().data.type() == simdjson::internal::tape_type::DOUBLE;
		}

		bool is_string() const {
			return node->get_value().data.type() == simdjson::internal::tape_type::STRING;
		}

		bool is_boolean() const {
			return node->get_value().data.type() == simdjson::internal::tape_type::TRUE_VALUE ||
				node->get_value().data.type() == simdjson::internal::tape_type::FALSE_VALUE;
		}

		bool contains(std::string_view key) {
			if (node->is_root()) {
				for (size_t i = 0; i < node->get_data_list(0)->get_data_size(); ++i) {
					if (node->get_data_list(0)->get_data_list(i)->get_value().key.get_str_val() == key) {
						return true;
					}
				}
			}
			else {
				for (size_t i = 0; i < node->get_data_size(); ++i) {
					if (node->get_data_list(i)->get_value().key.get_str_val() == key) {
						return true;
					}
				}
			}
			return false;
		}

		bool erase(std::string_view key) {
			if (node->is_root()) {
				for (size_t i = 0; i < node->get_data_list(0)->get_data_size(); ++i) {
					if (node->get_data_list(0)->get_data_list(i)->get_value().key.get_str_val() == key) {
						node->get_data_list(0)->remove_data_list(i);
						return true;
					}
				}
			}
			else {
				for (size_t i = 0; i < node->get_data_size(); ++i) {
					if (node->get_data_list(i)->get_value().key.get_str_val() == key) {
						node->remove_data_list(i);
						return true;
					}
				}
			}
			return false;
		}

		json push_array_with_no_key() {
			node->add_array_with_no_key(claujson::UserType::make_array());
			return json(json_ref(node->get_data().back()));
		}

		json push_object_with_no_key() {
			node->add_object_with_no_key(claujson::UserType::make_object());
			return json(json_ref(node->get_data().back()));
		}

		friend std::ostream& operator<<(std::ostream& stream, const json& j) {
			claujson::LoadData::save(stream, *j.node);
			return stream;
		}
	};
}

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

